#include "FileSystem.h"
#include "t2fs_record.h"
#include "DiscAccessManager.h"
#include "FreeSpaceManager.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

FileSystem fileSystem;
static BOOL initialized = FALSE;

#define numOfPointersInBlock(blockSize)     (blockSize / sizeof(DWORD))
#define numOfEntriesInBlock(blockSize)      (blockSize / sizeof(Record))
#define blockNumberByPosition(position, blockSize)      (position / blockSize)
#define blockOffsetByPosition(position, blockSize)      (position % blockSize)
#define min(a,b)    (a > b ? b : a)
#define maxBlocksInFile(directPtrs, blockSize, pointerSize) (directPtrs + (blockSize / pointerSize) + (blockSize / pointerSize) * (blockSize / pointerSize))

int FS_initilize(){
    if (!initialized) {
        fileSystem.SUPERBLOCK_ADDRESS = FS_SUPERBLOCK_ADDRESS;
        if (DAM_read(fileSystem.SUPERBLOCK_ADDRESS, (BYTE*)(&(fileSystem.superBlock)), TRUE) == 0){
            //Initialize with certain values, so that create handle won't mess things up
            memset(fileSystem.openRecords, TYPEVAL_INVALIDO, FS_OPENRECORDS_MAXSIZE);
            memset(fileSystem.openFiles, -1, FS_OPENFILES_MAXSIZE);
            
            initialized = TRUE;
            return FS_SUCCESS;
        }else{
            return FS_CREATE_FAIL;
        }
    }
}

t2fs_file FS_create(FilePath* const filePath, BYTE typeVal)
{
    if (filePath == NULL){
        return FS_INVALID_PATH;
    }
    //Directory Path until folder to create new file
	FilePath dirPath;
    FP_withoutLastNode(filePath, &dirPath);
    //Record of the parent who will get new child
    assert(fileSystem.superBlock.BlockSize > 0);
    OpenRecord parentOpenRecord;
    BYTE parentBlock[fileSystem.superBlock.BlockSize];
    
    //Find parent, starting from the root
    Record* parentRecord = TR_find(&fileSystem.superBlock.RootDirReg, &dirPath, &parentOpenRecord, parentBlock, DB_findByName, NULL, NULL);
    FP_destroy(&dirPath);
    
    if (parentRecord != NULL){
        Record newRecord;
        TR_Record(&newRecord, typeVal, FP_getLastNode(filePath), 0, 0);

        OpenRecord newOpenRecord;
        int addRecordSignal = TR_addRecord(parentRecord, newRecord, &newOpenRecord);
        //Update open record too
        parentOpenRecord.record = *parentRecord;

        int writingSignal;
        if (addRecordSignal == T2FS_RECORD_MODIFIED){
            //If modified a directory and it is not the root directory
            if (parentRecord != &fileSystem.superBlock.RootDirReg){
                writingSignal = DAM_write(parentOpenRecord.blockAddress, parentBlock, FALSE);
            }else{
                //If modified the root directory then save it. Actually save the superblock that has the root directory
                //Supose SuperBlock doesn't have alligment paddings, i.e., sizeof(SuperBlock) <= SuperBlock.SuperBlockSize
                writingSignal = DAM_write(FS_SUPERBLOCK_ADDRESS, (BYTE*)&fileSystem.superBlock, TRUE);
            }
            if (writingSignal != 0){
                addRecordSignal = T2FS_IOERROR;
            }
        }
        if(addRecordSignal == T2FS_SUCCESS || addRecordSignal == T2FS_RECORD_MODIFIED){
            t2fs_file newHandle = FS_createHandle(newOpenRecord);
            if (newHandle < 0){
                return FS_CREATESUCCESS_BUT_OPENPROBLEM;
            }else{
                return newHandle;
            }
        }else{
            //ERROR
            return FS_CREATE_FAIL;
        }
    }else{
        return FS_INVALID_PATH;
    }
}

t2fs_file FS_createHandle(OpenRecord openRecord)
{
    assert(openRecord.record.TypeVal != TYPEVAL_INVALIDO);
    assert(openRecord.record.name != NULL);
    //assert(openRecord.blockAddress != 0);
    
    //Verify if there is place in fileSytem open records and if this record is already open
    int i;
    BOOL alreadyOpen = FALSE;
    int possiblePlaceRecords = -1;
    for (i = 0; i < FS_OPENRECORDS_MAXSIZE; i++){
        if (fileSystem.openRecords[i].record.TypeVal != TYPEVAL_INVALIDO){
            if (OR_equals(&fileSystem.openRecords[i], openRecord)){
                alreadyOpen = TRUE;
                possiblePlaceRecords = i;
                break;
            }
        }else{
            possiblePlaceRecords = i;
        }
    }
    
    //Verify if there is place in fileSytem open files
    int possiblePlaceFiles = -1;
    for (i = 0; i < FS_OPENFILES_MAXSIZE; i++){
        if (fileSystem.openFiles[i].recordIndex < 0){
            possiblePlaceFiles = i;
            break;
        }
    }
    
    if (alreadyOpen){
        assert(possiblePlaceRecords >= 0);
        if (possiblePlaceFiles >= 0){
            //New openfile will point this record
            assert(fileSystem.openRecords[possiblePlaceRecords].count > 0);
            fileSystem.openRecords[possiblePlaceRecords].count++;

            OpenFile newOpenFile;
            OF_OpenFile(&newOpenFile, possiblePlaceRecords);
            fileSystem.openFiles[possiblePlaceFiles] = newOpenFile;
            //The address of the handle just created
            return possiblePlaceFiles;
        }else{
            return FS_FILEPOINTER_SPACE_UNAVAILABLE;
        } 
    }else{
        //If file is not open yet and there is place in fileSystem open record then...
        if (possiblePlaceRecords >= 0){
            if (possiblePlaceFiles >= 0){
                //Copies the new record into fileSystem's openRecords
                fileSystem.openRecords[possiblePlaceRecords] = openRecord;
                fileSystem.openRecords[possiblePlaceRecords].count = 1;
                
                OpenFile newOpenFile;
                OF_OpenFile(&newOpenFile, possiblePlaceRecords);
                fileSystem.openFiles[possiblePlaceFiles] = newOpenFile;
                //The address of the handle just created
                return possiblePlaceFiles;
                
            }else{
                return FS_FILEPOINTER_SPACE_UNAVAILABLE;
            } 
        }else{
            return FS_RECORDPOINTER_SPACE_UNAVAILABLE;
        }
    }
}

Record* FS_findRecordInArray(DWORD dataPtr[], BYTE* block, DWORD* blockAddress, Record*(*find)(const DirectoryBlock* const,const char* param), char* name, int count, int* indexFound)
{
    if (count <= 0 || dataPtr == NULL)
        return NULL;
    
    //Iterates over data pointer, looking for the directory block that has the record of the file of name "name"
    int i;
	for (i = 0; i < count; i++){
        //Read this block from the disc
        if (!DAM_read(dataPtr[i], block, FALSE)){
            
            DirectoryBlock directoryBlock;
            DB_DirectoryBlock(&directoryBlock, block);
            //Try to find the target record  in this directory block
            Record* foundRecord = find(&directoryBlock, name);
            
            if (foundRecord != NULL){
                *blockAddress = dataPtr[i];
                if (indexFound)
                    *indexFound = i;
                return foundRecord;
            }
        }
        //else do nothing, just keep trying
    }
    //Did not find any record with this file name
    return NULL;
}

int FS_delete(FilePath* const filePath)
{
	if (filePath == NULL){
        return FS_INVALID_PATH;
    }
    
    int returnCode;
    OpenRecord targetOpenRecord;
    BYTE targetBlock[fileSystem.superBlock.BlockSize];
    BYTE blockTrace[FS_BLOCK_TRACE_DEPTH_LENGTH][fileSystem.superBlock.BlockSize];
    DWORD* blockAddressTrace[FS_TRACE_DEPTH_LENGTH];
    DWORD traceRootIndicator = FS_NULL_BLOCK_POINTER;
    
    // Initialize trace (FS_NULL_BLOCK_POINTER indicates that first block is the superblock)
    blockAddressTrace[0] = &traceRootIndicator;
    
    //Find parent, starting from the root
    Record* targetRecord = TR_find(&fileSystem.superBlock.RootDirReg, filePath, &targetOpenRecord, targetBlock, DB_findByName, blockTrace, blockAddressTrace);
    
    if (targetRecord) {
        // if it is a directory, it must be empty
        if ((targetRecord->TypeVal == TYPEVAL_DIRETORIO)) {

            for (int i=0; i<TR_DATAPTRS_IN_RECORD ; i++) {
                if (targetRecord->dataPtr[i] != FS_NULL_BLOCK_POINTER) {
                    returnCode = FS_DELETEPROBLEM_DIRECTORY_NOT_EMPTY;
                    break;
                }
            }

            if ((returnCode == 0)
            && ((targetRecord->singleIndPtr != FS_NULL_BLOCK_POINTER)
                || (targetRecord->doubleIndPtr != FS_NULL_BLOCK_POINTER))
            ) {
                returnCode = FS_DELETEPROBLEM_DIRECTORY_NOT_EMPTY;
            }

        } else {
        // if it is a regular file, free its blocks
            returnCode = TR_freeBlocks(targetRecord);
        }

        // if not a problem yet ...
        if (returnCode == 0) {
            targetRecord->TypeVal = TYPEVAL_INVALIDO;

            unsigned int numOfEntriesInBlock = numOfEntriesInBlock(fileSystem.superBlock.BlockSize);
            unsigned int numOfPointersInBlock = numOfPointersInBlock(fileSystem.superBlock.BlockSize);

            // chech if the block where is this directory entry is now empty
            DirectoryBlock dirBlock;
            DB_DirectoryBlock(&dirBlock, targetBlock);
            int i;
            for (i=0; i<numOfEntriesInBlock; i++) {
                if ((dirBlock.entries[i].TypeVal == TYPEVAL_DIRETORIO)
                || (dirBlock.entries[i].TypeVal == TYPEVAL_REGULAR)) {
                    break;
                }
            }

            // if it every entry is empty, then free this block,
            // else, just save this block
            if (i == numOfEntriesInBlock) {
                if ((returnCode = FSM_delete(targetOpenRecord.blockAddress)) == 0) {
                    int j;
                    // find the tail of the trace
                    for (j=FS_TRACE_DEPTH_LENGTH - 1 ; (blockAddressTrace[j]==NULL) ; j--);
                    
                    // set the pointer of the deleted block to FS_NULL_BLOCK_POINTER
                    *blockAddressTrace[j] = FS_NULL_BLOCK_POINTER;
                    
                    // for each block since the entry of the directory in which this file is in,
                    // check if removing this entry will free any block, if so do it
                    for (--j; j>0; j--) {
                        
                        // Check if the block[j] is empty (have no more valid pointers)
                        int k;
                        for (k=0; k<numOfPointersInBlock; k++) {
                            if (blockTrace[j][k] != FS_NULL_BLOCK_POINTER) {
                                break;
                            }
                        }
                        // if every pointer in this block is empty, then free it
                        // else break this loop, because there won't be any other to free
                        if (k == numOfPointersInBlock) {
                            if ((returnCode = FSM_delete(*blockAddressTrace[j])) != 0) {
                                break;
                            }
                            *blockAddressTrace[j] = FS_NULL_BLOCK_POINTER;
                        } else {
                            break;
                        }
                    }

                    // if no error happened, then save the modified block (may be the superblock as well)
                    if (returnCode == 0) {
                        // if *blockAddressTrace[j] is FS_NULL_BLOCK_POINTER then it is to save the superblock
                        if (*blockAddressTrace[j] == FS_NULL_BLOCK_POINTER) {
                            returnCode = DAM_write(0, NULL, TRUE);
                        } else {
                            returnCode = DAM_write(*blockAddressTrace[j], blockTrace[j], FALSE);
                        }
                    }
                }
            } else {

                returnCode = DAM_write(targetOpenRecord.blockAddress, targetBlock, FALSE);
            }
        }
    } else {
        returnCode = FS_INVALID_PATH;
    }
    
    return returnCode;
}

t2fs_file FS_open(FilePath* const filePath)
{
    if (filePath == NULL){
        return FS_INVALID_PATH;
    }
    
    OpenRecord openRecord;
    BYTE block[fileSystem.superBlock.BlockSize];
    
    //Find parent, starting from the root
    Record* parentRecord = TR_find(&fileSystem.superBlock.RootDirReg, filePath, &openRecord, block, DB_findByName, NULL, NULL);
    
    if (parentRecord != NULL){
        
        return FS_createHandle(openRecord);
    }else{
        return FS_INVALID_PATH;
    }
}

static int FS_validateHandle(t2fs_file handle)
{
    if (handle < 0
    || handle >= FS_OPENFILES_MAXSIZE) {
        return T2FS_INVALID_ARGUMENT;
    }
    
    int recordIndex = fileSystem.openFiles[handle].recordIndex;
    assert(recordIndex >=0 && recordIndex < FS_OPENRECORDS_MAXSIZE);
    
    if (fileSystem.openFiles[handle].currentPosition < 0
    || fileSystem.openRecords[recordIndex].record.TypeVal == TYPEVAL_INVALIDO) {
        return T2FS_INVALID_ARGUMENT;
    }
    
    assert(fileSystem.openFiles[handle].currentPosition <= fileSystem.openRecords[recordIndex].record.bytesFileSize);
    assert(fileSystem.openRecords[recordIndex].count);
    return FS_SUCCESS;
}

int FS_close(t2fs_file handle)
{
    int validation;
    if ((validation = FS_validateHandle(handle))) {
        return validation;
    }
    
    // if the counter of open files now is zero, then set the "openRecord" free
    int recordIndex = fileSystem.openFiles[handle].recordIndex;
    if ((--fileSystem.openRecords[recordIndex].count) <= 0) {
        fileSystem.openRecords[recordIndex].record.TypeVal = TYPEVAL_INVALIDO;
    }
    
    // free this openFile struct for future use
    fileSystem.openFiles[handle].recordIndex = FS_OPENFILE_FREE;
    
    return FS_SUCCESS;
}

int FS_read(t2fs_file handle, char* buffer, int size)
{
    int validation;
    if ((validation = FS_validateHandle(handle))) {
        return validation;
    }
    
    DWORD blockAddress;
    unsigned int* currentPos = &fileSystem.openFiles[handle].currentPosition;
    int returnCode = 0,
        initialSize = size,
        blockNo = blockNumberByPosition(*currentPos, fileSystem.superBlock.BlockSize),
        blockOffset = blockOffsetByPosition(*currentPos, fileSystem.superBlock.BlockSize),
        recordIndex = fileSystem.openFiles[handle].recordIndex,
        sizeToRead;
    
    // Validations
    if (!buffer
    || size<0
    || fileSystem.openRecords[recordIndex].record.TypeVal != TYPEVAL_REGULAR) {
        return T2FS_INVALID_ARGUMENT;
    }
    
    BYTE dataBlock[fileSystem.superBlock.BlockSize];
    
    while (size > 0 && blockNo < fileSystem.openRecords[recordIndex].record.blocksFileSize) {

        // read the block from disc
        if ((returnCode = TR_findBlockByNumber(&fileSystem.openRecords[recordIndex].record, blockNo, dataBlock, &blockAddress, NULL))) {
            break;
        }

        sizeToRead = min(fileSystem.superBlock.BlockSize - blockOffset, size);

        // if it the reading will imply in advancing beyond the end of file, the read only until the end
        if ((*currentPos + sizeToRead) > fileSystem.openRecords[recordIndex].record.bytesFileSize) {
            sizeToRead = blockOffsetByPosition(fileSystem.openRecords[recordIndex].record.bytesFileSize, fileSystem.superBlock.BlockSize)
                    - blockOffset;
        }

        // copy data to the buffer
        memcpy(buffer, dataBlock + blockOffset, sizeToRead);

        buffer += sizeToRead;
        *currentPos += sizeToRead;
        size -= sizeToRead;

        blockOffset = 0;
        blockNo++;
    }
    
    return returnCode ? T2FS_IOERROR : (initialSize - size);
}

int FS_write(t2fs_file handle, char* buffer, int size)
{
    int validation;
    if ((validation = FS_validateHandle(handle))) {
        return validation;
    }
    
    DWORD blockAddress;
    int const maxBlocksInFile = maxBlocksInFile(TR_DATAPTRS_IN_RECORD, fileSystem.superBlock.BlockSize, sizeof(DWORD));
    unsigned int* currentPos = &fileSystem.openFiles[handle].currentPosition;
    int returnCode = 0,
        initialSize = size,
        blockNo = blockNumberByPosition(*currentPos, fileSystem.superBlock.BlockSize),
        blockOffset = blockOffsetByPosition(*currentPos, fileSystem.superBlock.BlockSize),
        recordIndex = fileSystem.openFiles[handle].recordIndex,
        sizeToWrite,
        increasingSize;
    
    // Validations
    if (!buffer
    || size<0
    || fileSystem.openRecords[recordIndex].record.TypeVal != TYPEVAL_REGULAR) {
        return T2FS_INVALID_ARGUMENT;
    }
    
    BYTE recordBlock[fileSystem.superBlock.BlockSize];
    BYTE dataBlock[fileSystem.superBlock.BlockSize];
    Record *directoryEntry;
    
    // loads the block where this record is in
    if ((returnCode = DAM_read(fileSystem.openRecords[recordIndex].blockAddress, recordBlock, FALSE)) == 0) {
        DirectoryBlock dirBlock;
        DB_DirectoryBlock(&dirBlock, recordBlock);
        if ((directoryEntry = DB_findByName(&dirBlock, fileSystem.openRecords[recordIndex].record.name)) == NULL) {
            returnCode = T2FS_DIDNT_FIND;
        }
    }
    
    if (returnCode == FS_SUCCESS) {
        while (size > 0 && blockNo < maxBlocksInFile) {

            if (blockNo < directoryEntry->blocksFileSize) {
                // block exists, so read it from disc
                if ((returnCode = TR_findBlockByNumber(directoryEntry, blockNo, dataBlock, &blockAddress, NULL))) {
                    break;
                }
            } else {
                // block doesn't exist, so append a new one in the file
                if ((returnCode = TR_appendNewBlock(directoryEntry, &blockAddress))) {
                    break;
                }
            }

            sizeToWrite = min(fileSystem.superBlock.BlockSize - blockOffset, size);

            // copy data from the buffer to the dataBlock
            memcpy(dataBlock + blockOffset, buffer, sizeToWrite);

            buffer += sizeToWrite;
            *currentPos += sizeToWrite;
            size -= sizeToWrite;

            // increase the filesize, if the writing did it
            if ((increasingSize = *currentPos - directoryEntry->bytesFileSize) > 0) {
                directoryEntry->bytesFileSize += increasingSize;
            }

            // write the current block
            if ((returnCode = DAM_write(blockAddress, dataBlock, FALSE))) {
                break;
            }

            blockOffset = 0;
            blockNo++;
        }

        // if no errors happened, the saves the block where this record is in
        if (returnCode == FS_SUCCESS) {
            returnCode = DAM_write(fileSystem.openRecords[recordIndex].blockAddress, recordBlock, FALSE);

            // update the value of the record kept in memory
            fileSystem.openRecords[recordIndex].record = *directoryEntry;
        }
    }
    
    return returnCode ? T2FS_IOERROR : (initialSize - size);
}

int FS_seek(t2fs_file handle, unsigned int offset)
{
    int validation;
    if ((validation = FS_validateHandle(handle))) {
        return validation;
    }
    
    int recordIndex = fileSystem.openFiles[handle].recordIndex;
    unsigned int fileSize = fileSystem.openRecords[recordIndex].record.bytesFileSize;
    
    // Validations
    if ((fileSystem.openRecords[recordIndex].record.TypeVal != TYPEVAL_REGULAR)
    || (offset != -1 && offset > fileSize)) {
        return T2FS_INVALID_ARGUMENT;
    }
    
    fileSystem.openFiles[handle].currentPosition = (offset == -1) ? fileSize : offset;
    
    return FS_SUCCESS;
}

// Extra functions for the utilities
int FS_applyCallbackToDirectory(t2fs_file handle, void(*callback)(const Record* const))
{
    int validation;
    if ((validation = FS_validateHandle(handle))) {
        return validation;
    }
    
    int const maxBlocksInFile = maxBlocksInFile(TR_DATAPTRS_IN_RECORD, fileSystem.superBlock.BlockSize, sizeof(DWORD));
    DWORD blockAddress;
    DWORD blockNo = 0;
    int returnCode = 0,
        recordIndex = fileSystem.openFiles[handle].recordIndex;
    
    // Validations
    if (fileSystem.openRecords[recordIndex].record.TypeVal != TYPEVAL_DIRETORIO) {
        return T2FS_INVALID_ARGUMENT;
    }
    
    BYTE dataBlock[fileSystem.superBlock.BlockSize];
    DirectoryBlock dirBlock;
    
    int i = 0;
    while ((i < fileSystem.openRecords[recordIndex].record.blocksFileSize)
    && (blockNo < maxBlocksInFile)) {
        
        // read the block from disc, if it is allocated, call the callback
        if ((returnCode = TR_findBlockByNumber(&fileSystem.openRecords[recordIndex].record, blockNo, dataBlock, &blockAddress, NULL)) == FS_SUCCESS) {

            DB_DirectoryBlock(&dirBlock, dataBlock);
            DB_forEachEntry(&dirBlock, callback);
            
            i++;
        }
        
        blockNo++;
    }
    
    return returnCode;
    
}

BOOL FS_isDirectory(t2fs_file handle)
{
    int validation;
    if ((validation = FS_validateHandle(handle))) {
        return validation;
    }
    
    int recordIndex = fileSystem.openFiles[handle].recordIndex;
    
    // Validations
    return (fileSystem.openRecords[recordIndex].record.TypeVal == TYPEVAL_DIRETORIO)
            ? TRUE
            : FALSE;
}
#include "FileSystem.h"
#include "t2fs_record.h"
#include "DiscAccessManager.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

FileSystem fileSystem;

t2fs_file FS_create(FilePath* const filePath)
{
    if (filePath == NULL){
        return FS_CREATEPROBLEM_INVALID_PATH;
    }
    //Directory Path until folder to create new file
	FilePath dirPath;
    FP_withoutLastNode(filePath, &dirPath);
    //Record of the parent who will get new child
    assert(fileSystem.superBlock.BlockSize > 0);
    OpenRecord parentOpenRecord;
    BYTE parentBlock[fileSystem.superBlock.BlockSize];
    
    //Find parent, starting from the root
    Record* parentRecord = TR_find(&fileSystem.superBlock.RootDirReg, &dirPath, &parentOpenRecord, parentBlock, DB_findByName, NULL, NULL, NULL);
    FP_destroy(&dirPath);
    
    if (parentRecord != NULL){
        Record newRecord;
        TR_t2fs_record(&newRecord, TYPEVAL_REGULAR, FP_getLastNode(filePath), 0, 0);

        OpenRecord newOpenRecord;
        int addRecordSignal = TR_addRecord(parentRecord, newRecord, &newOpenRecord);
        //Update open record too
        parentOpenRecord.record = *parentRecord;

        int writingSignal;
        if (addRecordSignal == TR_RECORD_MODIFIED){
            //If modified a directory and it is not the root directory
            if (parentBlock != NULL){
                writingSignal = DAM_write(parentOpenRecord.blockAddress, parentBlock);
            }else{
                //If modified the root directory then save it. Actually save the superblock that has the root directory
                //Supose SuperBlock doesn't have alligment paddings, i.e., sizeof(SuperBlock) <= SuperBlock.SuperBlockSize
                writingSignal = DAM_write(FS_SUPERBLOCK_ADDRESS, &fileSystem.superBlock);
            }
            if (writingSignal != 0){
                addRecordSignal = TR_ADDRECORD_FAIL;
            }
        }
        if(addRecordSignal == TR_ADDRECORD_SUCCESS || addRecordSignal == TR_RECORD_MODIFIED){
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
        return FS_CREATEPROBLEM_INVALID_PATH;
    }
}

t2fs_file FS_createHandle(OpenRecord openRecord)
{
    assert(openRecord.record.TypeVal != TYPEVAL_INVALIDO);
    assert(openRecord.record.name != NULL);
    assert(openRecord.blockAddress != 0);
    
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
            return FS_OPENPROBLEM_FILEPOINTER_SPACE_UNAVAILABLE;
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
                return FS_OPENPROBLEM_FILEPOINTER_SPACE_UNAVAILABLE;
            } 
        }else{
            return FS_OPENPROBLEM_RECORDPOINTER_SPACE_UNAVAILABLE;
        }
    }
}

Record* FS_findRecordInArray(DWORD dataPtr[], BYTE* block, DWORD* blockAddress, Record*(*find)(DirectoryBlock*, char* param), char* name, int count)
{
    if (count <= 0 || name != NULL || dataPtr != NULL)
        return NULL;
    
    //Iterates over data pointer, looking for the directory block that has the record of the file of name "name"
    int i;
	for (i = 0; i < count; i++){
        //Read this block from the disc
        if (!DAM_read(dataPtr[i], block)){
            
            DirectoryBlock directoryBlock;
            DB_DirectoryBlock(&directoryBlock, block);
            //Try to find the target record  in this directory block
            Record* foundRecord = find(&directoryBlock, name);
            
            if (foundRecord != NULL){
                *blockAddress = dataPtr[i];
                return foundRecord;
            }
        }
        //else do nothing, just keep trying
    }
    //Did not find any record with this file name
    return NULL;
}

int FS_findEmptyInArray(DWORD dataPtr[], BYTE* block, DWORD* blockAddress, int count)
{
	return 0;
}

int FS_delete(FilePath* const filePath)
{
	return 0;
}

t2fs_file FS_open(FilePath* const filePath)
{
	return 0;
}

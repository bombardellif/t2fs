#include "t2fs_record.h"
#include "DiscAccessManager.h"
#include "FileSystem.h"
#include "IndirectionBlock.h"
#include "FreeSpaceManager.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <malloc.h>

extern FileSystem fileSystem;

#define numOfPointersInBlock(blockSize)     (blockSize / sizeof(DWORD))
#define numOfEntriesInBlock(blockSize)      (blockSize / sizeof(Record))

void TR_Record(Record* this, BYTE typeVal, char* name, DWORD blocksFileSize, DWORD bytesFileSize)
{
    strncpy(this->name, name, TR_FILENAME_MAXSIZE);
    
    this->TypeVal = typeVal;
    this->blocksFileSize = blocksFileSize;
    this->bytesFileSize = bytesFileSize;
    this->dataPtr[0] = this->dataPtr[1] = FS_NULL_BLOCK_POINTER;
    this->singleIndPtr = FS_NULL_BLOCK_POINTER;
    this->doubleIndPtr = FS_NULL_BLOCK_POINTER;
}

Record* TR_find(Record* this, FilePath* const filePath, OpenRecord* openRecord, BYTE* block, Record*(*find)(const DirectoryBlock* const this, const char* const notUsed), BYTE blockTrace[][FS_BLOCK_TRACE_DEPTH_LENGTH], DWORD* blockAddressTrace[])
{
    if (this == NULL || openRecord == NULL || block == NULL)
        return NULL;
    
    char* currentNode = FP_getNextNode(filePath);
    //Does not have current Node ( filepath == "/" or filepath == "" )
    if (currentNode == NULL){
        openRecord->blockAddress = FS_NULL_BLOCK_POINTER;
        openRecord->record = *this;
        block = NULL;
        return this;
    }
    
    Record* foundRecord = TR_findRecordInRecord(this, openRecord, block, find, currentNode, blockTrace, blockAddressTrace);
    
    if (foundRecord != NULL){
        if (FP_hasNextNode(filePath)){
            // Make trace[0]
            // Copy the block of this directory entry to start a new trace of the search for records
            if (blockTrace && blockAddressTrace) {
                memcpy(blockTrace[0], block, sizeof(blockTrace[0]));
                *blockAddressTrace[0] = openRecord->blockAddress;
            }
            
            //Keep surfing in directory until find the target
            Record foundRecordCopy = *foundRecord;
            return TR_find(&foundRecordCopy, filePath, openRecord, block, find, blockTrace, blockAddressTrace);
        } else {
            // Make trace[3]
            // Copy just the adress to the directory block where the target file is in (do not copy the block)
            if (blockTrace && blockAddressTrace) {
                // find the tail of the trace
                int j;
                for (j=FS_TRACE_DEPTH_LENGTH - 1 ; (blockAddressTrace[j]==NULL) ; j--);
                
                IndirectionBlock ibTrace;
                IB_IndirectionBlock(&ibTrace, blockTrace[j]);
                blockAddressTrace[j+1] = IB_findDataPtrByValue(&ibTrace, openRecord->blockAddress);
            }
            
            //COPY found record into openRecord
            openRecord->record = *foundRecord;
            //Can return foundRecord, because it points to an address into the "block", which is returned through argument reference
            return foundRecord;
        }
    }else{
        return NULL;
    }
}

Record* TR_findRecordInRecord(Record* this, OpenRecord* openRecord, BYTE* block, Record*(*find)(const DirectoryBlock* const this, const char* const notUsed), char* param, BYTE blockTrace[][FS_BLOCK_TRACE_DEPTH_LENGTH], DWORD* blockAddressTrace[])
{
    int indexFound;
    Record* foundRecord;
    //First try to find in the dataPtrs of this record
    foundRecord = FS_findRecordInArray(this->dataPtr, block, &openRecord->blockAddress, find, param, TR_DATAPTRS_IN_RECORD, &indexFound);
    
    // Anula trace[2] e trace[3] (se encontrou)
    if (foundRecord && blockAddressTrace && blockTrace) {
        blockAddressTrace[1] = &this->dataPtr[indexFound];
        blockAddressTrace[2] = blockAddressTrace[3] = NULL;
    }
    
    //If did not find, searches through Single Indirection Pointer
    if (foundRecord == NULL){
        BYTE blockOfIndirection[fileSystem.superBlock.BlockSize];
        if (!DAM_read(this->singleIndPtr, blockOfIndirection, FALSE)){
        
            // Make trace[1] - null trace[2] and trace[3]
            if (blockAddressTrace && blockTrace) {
                memcpy(blockTrace[1], blockOfIndirection, sizeof(blockTrace[1]));
                IndirectionBlock ibTrace;
                IB_IndirectionBlock(&ibTrace, blockTrace[0]);
                blockAddressTrace[1] = IB_findDataPtrByValue(&ibTrace, this->singleIndPtr);
                blockAddressTrace[2] = blockAddressTrace[3] = NULL;
            }
            
            IndirectionBlock indirectionBlock;
            IB_IndirectionBlock(&indirectionBlock, blockOfIndirection);

            foundRecord = IB_find(&indirectionBlock, param, 1, block, &(openRecord->blockAddress), find, blockTrace, blockAddressTrace);
        }
    }
    //If hasn't found yet, searches through Double Indirection Pointer
    if (foundRecord == NULL){
        BYTE blockOfIndirection[fileSystem.superBlock.BlockSize];
        if (!DAM_read(this->doubleIndPtr, blockOfIndirection, FALSE)){
        
            // Make trace[1] - null trace[2] and trace[3]
            if (blockAddressTrace && blockTrace) {
                memcpy(blockTrace[1], blockOfIndirection, sizeof(blockTrace[1]));
                IndirectionBlock ibTrace;
                IB_IndirectionBlock(&ibTrace, blockTrace[0]);
                blockAddressTrace[1] = IB_findDataPtrByValue(&ibTrace, this->doubleIndPtr);
                blockAddressTrace[2] = blockAddressTrace[3] = NULL;
            }
            
            IndirectionBlock indirectionBlock;
            IB_IndirectionBlock(&indirectionBlock, blockOfIndirection);

            foundRecord = IB_find(&indirectionBlock, param, 2, block, &(openRecord->blockAddress), find, blockTrace, blockAddressTrace);
        }
    }
    return foundRecord;
}

int TR_addRecord(Record* this, Record newRecord, OpenRecord* newOpenRecord)
{
    if (this == NULL || newOpenRecord == NULL || newRecord.name == NULL)
        return T2FS_INVALID_ARGUMENT;
    
    FilePath filePath;
	FP_FilePath(&filePath, newRecord.name);
    
    //Try to find the record of the file with the same name as the new file (newRecord), begining in this record.
    //Since the filePath has only one node (the name of the new file), find will look only in its own children
    Record* targetRecord;
    assert (fileSystem.superBlock.BlockSize > 0);
    BYTE modifiedBlock[fileSystem.superBlock.BlockSize];
    targetRecord = TR_find(this, &filePath, newOpenRecord, modifiedBlock, DB_findByName, NULL, NULL);
    FP_destroy(&filePath);
    
    if (targetRecord != NULL){
        //Found, then free all its blocks
        TR_freeBlocks(targetRecord);
    }else{
        //Didn't find, then look for an empty entry in the directory
        assert(modifiedBlock != NULL);
        targetRecord = TR_findRecordInRecord(this, newOpenRecord, modifiedBlock, DB_findEmpty, NULL, NULL, NULL);
        
        //If did not find an empty entry, then create one
        if (targetRecord == NULL){
            //This will create a new directory block, if possible, return into modifiedBlock. And save it into disc
            assert(modifiedBlock != NULL);
            int allocateSignal = TR_allocateNewDirectoryBlock(this, modifiedBlock, &(newOpenRecord->blockAddress));
            
            if (allocateSignal != T2FS_SUCCESS){
                return T2FS_CANT_ALLOCATE;
            }
            
            // the pointer targetRecord will point to the first position of the just createdBlock
            targetRecord = (Record*)modifiedBlock;
        }
        
        // Increment the bytes counter of the directory
        this->bytesFileSize += sizeof(Record);
    }
    //Before returning, updates the newOpenRecord
    newOpenRecord->record = newRecord;
    if (targetRecord != NULL){
        //targetRecord points to the empty entry inside the modifiedBlock. Then updates it. They're all in main memory
        *targetRecord = newRecord;
    }
    //Saves the modification into disc
    assert(modifiedBlock != NULL);
    if (DAM_write(newOpenRecord->blockAddress, modifiedBlock, FALSE) == 0){ //Success
        //Return modified signal, because the allocation may have altered this record (new dataPtr for example)
        return T2FS_RECORD_MODIFIED;
    }else{
        return T2FS_IOERROR;
    }
}

int TR_freeBlocks(Record* this)
{
    if (this == NULL || this->TypeVal != TYPEVAL_REGULAR)
        return T2FS_INVALID_ARGUMENT;
    
    BYTE block[fileSystem.superBlock.BlockSize];
    int returnCode = 0;
    
    // direct blocks
    for (int i=0; i<TR_DATAPTRS_IN_RECORD; i++) {
        if (this->dataPtr[i] != FS_NULL_BLOCK_POINTER) {
            
            if ((returnCode = FSM_delete(this->dataPtr[i])) != 0) {
                break;
            }
            this->dataPtr[i] = FS_NULL_BLOCK_POINTER;
        }
    }
    // 1 level blocks
    if ((returnCode == 0) && (this->singleIndPtr != FS_NULL_BLOCK_POINTER)) {
        // Read the single indirection block
        if ((returnCode = DAM_read(this->singleIndPtr, block, FALSE)) == 0) {

            IndirectionBlock indirectionBlock;
            IB_IndirectionBlock(&indirectionBlock, block);
            
            if ((returnCode = IB_freeBlocks(&indirectionBlock, 1)) == 0) {
                this->singleIndPtr = FS_NULL_BLOCK_POINTER;
            }
        }
    }
    // 2 level blocks
    if ((returnCode == 0) && (this->doubleIndPtr != FS_NULL_BLOCK_POINTER)) {
        // Read the double indirection block
        if ((returnCode = DAM_read(this->doubleIndPtr, block, FALSE)) == 0) {

            IndirectionBlock indirectionBlock;
            IB_IndirectionBlock(&indirectionBlock, block);
            
            if ((returnCode = IB_freeBlocks(&indirectionBlock, 2)) == 0) {
                this->doubleIndPtr = FS_NULL_BLOCK_POINTER;
            }
        }
    }
    
    return returnCode;
}

int TR_allocateNewDirectoryBlock(Record* this, BYTE* block, DWORD* blockAddress)
{
    if (this == NULL || block == NULL || blockAddress == NULL)
        return T2FS_INVALID_ARGUMENT;
    
    //First try to find in the dataPtrs of this record
    assert(this->dataPtr);
    int writtenPos = TR_findEmptyPositionInArray(this->dataPtr, TR_DATAPTRS_IN_RECORD);
    
    //If found
    if (writtenPos >= 0){
        if (TR_allocateNewBlock(blockAddress) == T2FS_SUCCESS){
            //Initialize block with null pointers
            memset(block, FS_NULL_BLOCK_POINTER, fileSystem.superBlock.BlockSize);
            //updates data pointer with new block address
            this->dataPtr[writtenPos] = *blockAddress;
            //Saves modification to disc
            if (DAM_write(*blockAddress, block, FALSE) != 0){
                return T2FS_IOERROR;
            }else{
                // Increments the blocks counter of the directory
                this->blocksFileSize++;
                return T2FS_SUCCESS;
            }
        }else{
            return T2FS_CANT_ALLOCATE;
        }
    }else{
        //If did not find, searches through Single Indirection Pointer
        BYTE indirectionBlockMem[fileSystem.superBlock.BlockSize];
        //If the single indirection pointer is null...
        if (this->singleIndPtr == FS_NULL_BLOCK_POINTER){
            //Allocate a new Indirection Block on it
            DWORD indirectionBlockAddress;
            
            if (TR_allocateNewIndirectionBlock(indirectionBlockMem, &indirectionBlockAddress) == T2FS_SUCCESS){
                this->singleIndPtr = indirectionBlockAddress;
                //Write to disc the new indirection block just created
                if (DAM_write(this->singleIndPtr, indirectionBlockMem, FALSE) != 0){
                    return T2FS_IOERROR;
                }
            }else{
                return T2FS_CANT_ALLOCATE;
            }
        }else{
            //Read the single indirection pointer
            if (DAM_read(this->singleIndPtr, indirectionBlockMem, FALSE) != 0){
                return T2FS_IOERROR;
            }
        }
        //Look inside this single indirection block
        IndirectionBlock singleIndirectionBlock;
        IB_IndirectionBlock(&singleIndirectionBlock, indirectionBlockMem);
        
        writtenPos = TR_findEmptyPositionInArray(singleIndirectionBlock.dataPtr, numOfEntriesInBlock(fileSystem.superBlock.BlockSize));
        
        if (writtenPos >= 0){
            //If found an empty space in this single indirection pointer
            if (TR_allocateNewBlock(blockAddress) == T2FS_SUCCESS){
                //Initialize block with null pointers
                memset(block, FS_NULL_BLOCK_POINTER, fileSystem.superBlock.BlockSize);
                //updates data pointer with new block address
                singleIndirectionBlock.dataPtr[writtenPos] = *blockAddress;
                //Saves modification to disc (diirectory block)
                if (DAM_write(*blockAddress, block, FALSE) != 0){
                    return T2FS_IOERROR;
                }else{
                    // Increments the blocks counter of the directory
                    this->blocksFileSize++;
                    //Write to disc the indirection block just modified
                    if (DAM_write(this->singleIndPtr, indirectionBlockMem, FALSE) != 0){
                        return T2FS_IOERROR;
                    }else
                        return T2FS_SUCCESS;
                }
            }else{
                return T2FS_CANT_ALLOCATE;
            }
        }else{
            //Here, it hasn't found in the single Ind pointer. Try to find in the double
            BYTE indirectionBlockMem[fileSystem.superBlock.BlockSize];
            
            if (this->doubleIndPtr == FS_NULL_BLOCK_POINTER){
                //Allocate a new Indirection Block on it
                DWORD indirectionBlockAddress;

                if(TR_allocateNewIndirectionBlock(indirectionBlockMem, &indirectionBlockAddress) == T2FS_SUCCESS){
                    this->doubleIndPtr = indirectionBlockAddress;
                    //Write to disc the new indirection block just created
                    if (DAM_write(this->doubleIndPtr, indirectionBlockMem, FALSE) != 0){
                        return T2FS_IOERROR;
                    }
                }else{
                    return T2FS_CANT_ALLOCATE;
                }
            }else{
                //Read the single indirection pointer
                if (DAM_read(this->doubleIndPtr, indirectionBlockMem, FALSE) != 0){
                    return T2FS_IOERROR;
                }
            }
            
            IndirectionBlock doubleIndirectionBlock;
            IB_IndirectionBlock(&doubleIndirectionBlock, indirectionBlockMem);
            if (IB_allocateNewDirectoryBlock(&doubleIndirectionBlock, 2, block, blockAddress) >= 0){ //Success
                //Write to disc the new indirection block just created
                if (DAM_write(this->doubleIndPtr, indirectionBlockMem, FALSE) != 0){
                    return T2FS_IOERROR;
                }else{
                    // Increments the blocks counter of the directory
                    this->blocksFileSize++;
                    return T2FS_SUCCESS;
                }
            }else{
                return T2FS_CANT_ALLOCATE;
            }
        }
    }
    return T2FS_SUCCESS;
}


int TR_allocateNewIndirectionBlock(BYTE* indirectionBlockMem, DWORD* indirectionBlockAddress)
{
    if (TR_allocateNewBlock(indirectionBlockAddress) == T2FS_SUCCESS){
        //Initialize block with null pointers
        memset(indirectionBlockMem, FS_NULL_BLOCK_POINTER, fileSystem.superBlock.BlockSize);

        if (!DAM_write(*indirectionBlockAddress, indirectionBlockMem, FALSE)){
            return T2FS_SUCCESS;
        }else{
            return T2FS_IOERROR;
        }
    }else{
        return T2FS_CANT_ALLOCATE;
    }
}

int TR_allocateNewBlock(DWORD* blockAddress){
    //Allocate new block for the directory block
    int freeAddressSignal = FSM_getFreeAddress(blockAddress);
    if (freeAddressSignal == 0){ //Success
        if (!FSM_markAsUsed(*blockAddress)){ //Success
            return T2FS_SUCCESS;
        }else{
            return T2FS_IOERROR;
        }
    }else{
        return T2FS_CANT_ALLOCATE;
    }
}

int TR_findEmptyPositionInArray(const DWORD const dataPtr[], const unsigned int count)
{
    if (dataPtr == NULL)
        return T2FS_DIDNT_FIND;
    
    //Iterates over data pointer, looking for one empty
	for (int i = 0; i < count; i++){
        if (dataPtr[i] == FS_NULL_BLOCK_POINTER){
            return i;
        }
        //else do nothing, just keep trying
    }
    //Did not find any empty position
    return T2FS_DIDNT_FIND;
}

/**
 * If blockAddressPtr is NULL then finds and read from disc the block which number in the record is the parameter
 * "number". The first block in the record is the 0, the second is the 1, and so on.
 * 
 * if blockAddressPtr is not NULL then finds the memory address of the block pointer whose number is the number in
 * the parameter, but do not load it, instead load the block with this block pointer.
 * 
 * @param this
 * @param number            number of the the block in this record to load
 * @param block             the pointer to the area of memory where to load the block data
 * @param blockAddress      output variable, the disc address of the loaded block
 * @param blockAddressPtr   output variable, the memory address of the disc block adress pointer to the block with thie number
 * @return  0 in case of success, a number different of zero in case of error
 */
int TR_findBlockByNumber(Record* this, DWORD number, BYTE* block, DWORD* blockAddress, DWORD** blockAddressPtr)
{
    if (this == NULL || block == NULL)
        return T2FS_INVALID_ARGUMENT;
    
    int returnCode = T2FS_DIDNT_FIND;
    unsigned int numOfPointersInBlock = numOfPointersInBlock(fileSystem.superBlock.BlockSize);
    unsigned int numOfPointersInIndirectionBlock = numOfPointersInBlock * numOfPointersInBlock;
    
    // if the block is in the direct pointers
    if (number < TR_DATAPTRS_IN_RECORD) {
        
        if (blockAddressPtr) {
            
            *blockAddress = FS_NULL_BLOCK_POINTER;
            *blockAddressPtr = &this->dataPtr[number];
            returnCode = T2FS_SUCCESS;
        } else {
            *blockAddress = this->dataPtr[number];
            // Read the desired block
            returnCode = DAM_read(this->dataPtr[number], block, FALSE);
        }
    }
    // subtract the number of dataPtr in the record from the "number" and check
    // if the block is in the first indirection pointer
    else if ((number -= TR_DATAPTRS_IN_RECORD) < numOfPointersInBlock) {
        
        // the singleIndirectionPointer may not be allocated, so do it
        if (blockAddressPtr && this->singleIndPtr == FS_NULL_BLOCK_POINTER) {
            
            // allocate a new indirection pointer and continue to find
            if ((returnCode = TR_allocateNewIndirectionBlock(block, &this->singleIndPtr) == 0)) {
                
                IndirectionBlock indirectionBlock;
                IB_IndirectionBlock(&indirectionBlock, block);
                
                // even though blockAddress may be changed by the following call, if blockAddressPtr is not NULL
                // then it won't, thus blockAddress will keep the address of this indirection block
                *blockAddress = this->singleIndPtr;
                
                // find in the indirection block the required block
                returnCode = IB_findBlockByNumber(&indirectionBlock, 1, number, block, blockAddress, blockAddressPtr);
            }
        } else {
            
            // Read the single indirection block
            if ((returnCode = DAM_read(this->singleIndPtr, block, FALSE)) == 0) {

                IndirectionBlock indirectionBlock;
                IB_IndirectionBlock(&indirectionBlock, block);
                
                // even though blockAddress may be changed by the following call, if blockAddressPtr is not NULL
                // then it won't, thus blockAddress will keep the address of this indirection block
                *blockAddress = this->singleIndPtr;
                
                // find in the indirection block the required block
                returnCode = IB_findBlockByNumber(&indirectionBlock, 1, number, block, blockAddress, blockAddressPtr);
            }
        }
    }
    // subtract the number of dataPtr in the double indirection pointer from the
    // "number" and check if the block is in the second indirection pointer
    else if ((number -= numOfPointersInBlock) < numOfPointersInIndirectionBlock) {
        
        if (blockAddressPtr && this->doubleIndPtr == FS_NULL_BLOCK_POINTER) {
            
            // allocate a new indirection pointer and continue to find
            if ((returnCode = TR_allocateNewIndirectionBlock(block, &this->doubleIndPtr) == 0)) {
                
                IndirectionBlock indirectionBlock;
                IB_IndirectionBlock(&indirectionBlock, block);
                
                // even though blockAddress may be changed by the following call, if blockAddressPtr is not NULL
                // then it won't, thus blockAddress will keep the address of this indirection block
                *blockAddress = this->doubleIndPtr;
                
                // find in the indirection block the required block
                returnCode = IB_findBlockByNumber(&indirectionBlock, 2, number, block, blockAddress, blockAddressPtr);
            }
        } else {
            
            // Read the double indirection block
            if ((returnCode = DAM_read(this->doubleIndPtr, block, FALSE)) == 0) {

                IndirectionBlock indirectionBlock;
                IB_IndirectionBlock(&indirectionBlock, block);
                
                // even though blockAddress may be changed by the following call, if blockAddressPtr is not NULL
                // then it won't, thus blockAddress will keep the address of this indirection block
                *blockAddress = this->doubleIndPtr;
                
                // find in the indirection block the required block
                returnCode = IB_findBlockByNumber(&indirectionBlock, 2, number, block, blockAddress, blockAddressPtr);
            }
        }
    }
    
    return returnCode;
}

int TR_appendNewBlock(Record* this, DWORD* newBlockAddress)
{
    int returnCode;
    BYTE changedBlock[fileSystem.superBlock.BlockSize];
    DWORD changedBlockAddress;
    DWORD* newBlockAddressPtr;
    
    // find the block pointer where to allocate the new data block (newBlockAddressPtr)
    // and the block address where this pointer is (alteredBlockAddress)
    if ((returnCode = TR_findBlockByNumber(this, this->blocksFileSize, changedBlock, &changedBlockAddress, &newBlockAddressPtr)) == 0) {
        
        // Allocate a new block in disc and set the value of blockAddressPtr to the new block address in disc
        if ((returnCode = TR_allocateNewBlock(newBlockAddressPtr)) == 0) {
            
            // if allocated successfully, then increments blockFileSize. Note that
            // this function doesn't have the duty to save the record after blockFileSize was modified
            this->blocksFileSize++;
            *newBlockAddress = *newBlockAddressPtr;
            
            // Save the block with the changed pointer, but only if it was an indirection Block,
            // because, if changedBlockAddress == FS_NULL_BLOCK_POINTER it means that the changed block
            // is the same block where this record is in (a directroy block), and will be saved ouside this function
            if (changedBlockAddress != FS_NULL_BLOCK_POINTER) {
                returnCode = DAM_write(changedBlockAddress, changedBlock, FALSE);
            }
        }
    }
    
    return returnCode;
}
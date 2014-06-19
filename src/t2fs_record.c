#include "t2fs_record.h"
#include "DiscAccessManager.h"
#include "FileSystem.h"
#include "IndirectionBlock.h"
#include <stdlib.h>
#include <assert.h>

extern FileSystem fileSystem;

void TR_t2fs_record(Record* this, BYTE typeVal, char* name, DWORD blocksFileSize, DWORD bytesFileSize)
{
}

Record* TR_find(Record* this, FilePath* const filePath, OpenRecord* openRecord, BYTE* block, Record*(*find)(const DirectoryBlock* const this, const char* const notUsed), BYTE blockTrace[], DWORD* recordPointerTrace[], DWORD blockAddress[])
{
    if (this == NULL || openRecord == NULL || block == NULL)
        return NULL;
    
    char* currentNode = FP_getNextNode(filePath);
    
    Record* foundRecord = TR_findRecordInRecord(this, openRecord, block, find, currentNode);
    
    if (foundRecord != NULL){
        if (FP_hasNextNode(filePath)){
            /** @TODO Keep track */
            //Keep surfing in directory until find the target
            return TR_find(foundRecord, filePath, openRecord, block, find, NULL, NULL, NULL);
        }else{
            //COPY found record into openRecord
            openRecord->record = *foundRecord;
            //Can return foundRecord, because it points to an address into the "block", which is returned through argument reference
            return foundRecord;
        }
    }else{
        return NULL;
    }
}

Record* TR_findRecordInRecord(Record* this, OpenRecord* openRecord, BYTE* block, Record*(*find)(const DirectoryBlock* const this, const char* const notUsed), char* param)
{
    Record* foundRecord;
    //First try to find in the dataPtrs of this record
    foundRecord = FS_findRecordInArray(this->dataPtr, block, openRecord->blockAddress, find, param, TR_DATAPTRS_IN_RECORD);
    
    //If did not find, searches through Single Indirection Pointer (if not null)
    if (foundRecord == NULL){
        BYTE blockOfIndirection[fileSystem.superBlock.BlockSize];
        if (!DAM_read(this->singleIndPtr, blockOfIndirection)){
        
            IndirectionBlock indirectionBlock;
            IB_IndirectionBlock(&indirectionBlock, blockOfIndirection);

            foundRecord = IB_find(&indirectionBlock, param, 1, block, &(openRecord->blockAddress), find);
        }
    }
    //If hasn't found yet, searches through Double Indirection Pointer (if not null)
    if (foundRecord == NULL){
        BYTE blockOfIndirection[fileSystem.superBlock.BlockSize];
        if (!DAM_read(this->doubleIndPtr, blockOfIndirection)){
        
            IndirectionBlock indirectionBlock;
            IB_IndirectionBlock(&indirectionBlock, blockOfIndirection);

            foundRecord = IB_find(&indirectionBlock, param, 2, block, &(openRecord->blockAddress), find);
        }
    }
    return foundRecord;
}

int TR_addRecord(Record* this, Record newRecord, OpenRecord* newOpenRecord)
{
    if (this == NULL || newOpenRecord == NULL || newRecord->name == NULL)
        return T2FS_ADDRECORD_INVALID_ARGUMENT;
    
    FilePath filePath;
	FP_FilePath(&filePath, newRecord->name);
    
    //Try to find the record of the file with the same name as the new file (newRecord), begining in this record.
    //Since the filePath has only one node (the name of the new file), find will look only in its own children
    Record* targetRecord;
    assert (fileSystem.superBlock.BlockSize > 0);
    BYTE modifiedBlock[fileSystem.superBlock.BlockSize];
    targetRecord = TR_find(this, filePath, newOpenRecord, &modifiedBlock, DB_findByName, NULL, NULL, NULL);
    FP_destroy(&filePath);
    
    if (targetRecord != NULL){
        //Found, then free all its blocks
        TR_freeBlocks(targetRecord);
    }else{
        //Didn't find, then look for an empty entry in the directory
        assert(&modifiedBlock != NULL);
        targetRecord = TR_findRecordInRecord(this, newOpenRecord, &modifiedBlock, DB_findEmpty, NULL);
        
        //If did not find an empty entry, then create one
        if (targetRecord == NULL){
            //This will create a new directory block, if possible, return into modifiedBlock. And save it into disc
            assert(&modifiedBlock != NULL);
            int allocateSignal = TR_allocateNewDirectoryBlock(this, &modifiedBlock, &(newOpenRecord->blockAddress));
            
            if (allocateSignal == T2FS_ALLOCATENEWDIRBLOCK_SUCCESS){
                //Before returning, updates the newOpenRecord
                newOpenRecord->record = newRecord;
                //Doesn't need to save anything on disc. the allocation has done that
                //Return modified signal, because the allocation may have altered this record (new dataPtr for example)
                return T2FS_RECORD_MODIFIED;
            }else{
                return T2FS_ADDRECORD_CANT_ALLOCATE;
            }
        }else{
            //Before returning, updates the newOpenRecord
            newOpenRecord->record = newRecord;
            //targetRecord points to the empty entry inside the modifiedBlock. Then updates it. They're all in main memory
            *targetRecord = newRecord;
            //Saves the modification into disc
            assert(&modifiedBlock != NULL);
            if (DAM_write(newOpenRecord->blockAddress, &modifiedBlock) == 0){ //Success
                return T2FS_ADDRECORD_SUCCESS;
            }else{
                return T2FS_ADDRECORD_IOERROR;
            }
        }
    }
}

void TR_freeBlocks(Record* this)
{
}

Record* TR_findEmptyEntry(Record* this, OpenRecord* openRecord, BYTE* block)
{
	return 0;
}

int TR_allocateNewDirectoryBlock(Record* this, BYTE* block, DWORD* blockAddress)
{
}

/**
 * Finds and read from disc the block which number in the record is the parameter
 * "number". The first block in the record is the 0, the second is the 1, and so on.
 * 
 * @param this
 * @param number    number of the the block in this record to load
 * @param block     the pointer to the area of memory where to load the block data
 * @param blockAddress output variable, the address of the loaded block
 * @return  0 in case of success, a number different of zero in case of error
 */
int TR_findBlockByNumber(Record* this, DWORD number, BYTE* block, DWORD* blockAddress)
{
    if (this == NULL || block == NULL)
        return NULL;
    
    int returnCode;
    unsigned int numOfPointersInBlock = fileSystem.superBlock.BlockSize / sizeof(DWORD);
    unsigned int numOfPointersInIndirectionBlock = numOfPointersInBlock * numOfPointersInBlock;
    
    // if the block is in the direct pointers
    if (number < TR_DATAPTRS_IN_RECORD) {
        
        *blockAddress = this->dataPtr[number];
        // Read the desired block
        returnCode =  DAM_read(this->dataPtr[number], block);
    }
    // subtract the number of dataPtr in the record from the "number" and check
    // if the block is in the first indirection pointer
    else if ((number -= TR_DATAPTRS_IN_RECORD) < numOfPointersInBlock) {
        
        // Read the single indirection block
        if ((returnCode = DAM_read(this->singleIndPtr, block)) == 0) {

            IndirectionBlock* indirectionBlock;
            IB_IndirectionBlock(indirectionBlock, block);
            // find in the indirection block the required block
            returnCode = IB_findBlockByNumber(indirectionBlock, 1, number, block, blockAddress);
        }
    }
    // subtract the number of dataPtr in the double indirection pointer from the
    // "number" and check if the block is in the second indirection pointer
    else if ((number -= numOfPointersInBlock) < numOfPointersInIndirectionBlock) {
        
        // Read the double indirection block
        if ((returnCode = DAM_read(this->doubleIndPtr, block)) == 0) {

            IndirectionBlock* indirectionBlock;
            IB_IndirectionBlock(indirectionBlock, block);
            // find in the indirection block the required block
            returnCode = IB_findBlockByNumber(indirectionBlock, 2, number, block, blockAddress);
        }
    }
    
    return returnCode;
}

#include "t2fs_record.h"
#include <stdlib.h>
#include <assert.h>

extern FileSystem fileSystem;

void TR_t2fs_record(Record* this, BYTE typeVal, char* name, DWORD blocksFileSize, DWORD bytesFileSize)
{
}

Record* TR_find(Record* this, FilePath* const filePath, OpenRecord* openRecord, BYTE* block, BYTE blockTrace[], DWORD* recordPointerTrace[], DWORD blockAddress[])
{
    if (this == NULL || openRecord == NULL || block == NULL)
        return NULL;
    
    char* currentNode = FP_getNextNode(filePath);
    
    Record* foundRecord;
    
    //First try to find in the dataPtrs of this record
    foundRecord = FS_findRecordInArray(this->dataPtr, block, openRecord->blockAddress, currentNode, TR_DATAPTRS_IN_RECORD);
    
    //If did not find, searches through Single Indirection Pointer (if not null)
    if (foundRecord == NULL){
        if (!DAM_read(this->singleIndPtr, block)){
        
            IndirectionBlock indirectionBlock;
            IB_IndirectionBlock(&indirectionBlock, block);

            foundRecord = IB_find(&indirectionBlock, currentNode, 1, block, blockAddress);
        }
    }
    //If hasn't found yet, searches through Double Indirection Pointer (if not null)
    if (foundRecord == NULL){
        if (!DAM_read(this->doubleIndPtr, block)){
        
            IndirectionBlock indirectionBlock;
            IB_IndirectionBlock(&indirectionBlock, block);

            foundRecord = IB_find(&indirectionBlock, currentNode, 2, block, blockAddress);
        }
    }
    
    if (foundRecord != NULL){
        if (FP_hasNextNode(filePath)){
            /** @TODO Keep track */
            //Keep surfing in directory until find the target
            return TR_find(foundRecord, filePath, openRecord, block, NULL, NULL, NULL);
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
    targetRecord = TR_find(this, filePath, newOpenRecord, &modifiedBlock, NULL, NULL, NULL);
    FP_destroy(&filePath);
    
    if (targetRecord != NULL){
        //Found, then free all its blocks
        TR_freeBlocks(targetRecord);
    }else{
        //Didn't find, then look for an empty entry in the directory
        assert(&modifiedBlock != NULL);
        targetRecord = TR_findEmptyEntry(this, newOpenRecord, &modifiedBlock);
        
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
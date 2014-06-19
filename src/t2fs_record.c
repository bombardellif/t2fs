#include "t2fs_record.h"
#include <stdlib.h>

extern FileSystem fileSystem;

void TR_t2fs_record(Record* this, BYTE typeVal, char* name, DWORD blocksFileSize, DWORD bytesFileSize)
{
}

Record* TR_find(Record* this, FilePath filePath, OpenRecord* openRecord, BYTE* block, BYTE blockTrace[], DWORD* recordPointerTrace[], DWORD blockAddress[])
{
    if (this == NULL || openRecord == NULL || block == NULL)
        return NULL;
    
    char* currentNode = FP_getNextNode(filePath);
    
    Record* foundRecord;
    
    //First try to find in the dataPtrs of this record
    foundRecord = FS_findRecordInArray(this->dataPtr, block, openRecord->blockAddress, currentNode, TR_DATAPTRS_IN_RECORD);
    
    //If did not find, searches through Single Indirection Pointer (if not null)
    if (foundRecord == NULL && this->singleIndPtr != T2FS_NULL_BLOCK_POINTER){
        DAM_read(this->singleIndPtr, block);
        
        IndirectionBlock indirectionBlock;
        IB_IndirectionBlock(&indirectionBlock, block);
        
        foundRecord = IB_find(&indirectionBlock, currentNode, 1, block, blockAddress);
    }
    //If hasn't found yet, searches through Double Indirection Pointer (if not null)
    if (foundRecord == NULL && this->doubleIndPtr != T2FS_NULL_BLOCK_POINTER){
        DAM_read(this->doubleIndPtr, block);
        
        IndirectionBlock indirectionBlock;
        IB_IndirectionBlock(&indirectionBlock, block);
        
        foundRecord = IB_find(&indirectionBlock, currentNode, 2, block, blockAddress);
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

int TR_addRecord(Record* this, t2fs_record newRecord, OpenRecord* newOpenRecord)
{
	return 0;
}

void TR_freeBlocks(Record* this)
{
}

Record* TR_findEmptyEntry(Record* this, OpenRecord* openRecord, BYTE* block)
{
	return 0;
}

void TR_allocateNewDirectoryBlock(Record* this, BYTE* block, DWORD* blockAddress)
{
}
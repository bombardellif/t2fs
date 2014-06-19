#include "t2fs_record.h"
#include "DiscAccessManager.h"
#include "FileSystem.h"
#include "IndirectionBlock.h"
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
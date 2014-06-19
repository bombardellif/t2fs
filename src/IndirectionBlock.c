#include "IndirectionBlock.h"
#include <stdlib.h>


void IB_IndirectionBlock(IndirectionBlock* this, BYTE* block)
{
    this->dataPtr = (DWORD*) block;
}

Record* IB_find(IndirectionBlock* this, char* name, int level, BYTE* block, DWORD* blockAddress)
{
    if (this == NULL || this->dataPtr == NULL || name == NULL)
        return NULL;
    
    //If it is a single indirection
    if (level == 1){
        return FS_findRecordInArray(this->dataPtr, block, blockAddress, name, fileSystem.superBlock.BlockSize / sizeof(DWORD));
    }else if (level == 2){
        //If it is double indirection
        //Iterates over this indirection block (which is a double one). for each indirection block apointed tries to find
        int i;
        for(i = 0; i < fileSystem.superBlock.BlockSize / sizeof(DWORD);i++){
            if (this->dataPtr[i] != IB_NULL_BLOCK_POINTER){
                //Reads one single indirection block
                DAM_read(this->dataPtr[i], block);
                
                IndirectionBlock indirectionBlock;
                IB_IndirectionBlock(&indirectionBlock, block);

                Record* foundRecord = IB_find(&indirectionBlock, name, 1, block, blockAddress);
                if (foundRecord != NULL){
                    return foundRecord;
                }
            }
        }
    }else{
        return NULL;
    }
}

int IB_allocateNewDirectoryBlock(IndirectionBlock* this, int level, BYTE* block, DWORD* blockAddress)
{
	return 0;
}

int IB_findBlockByNumber(IndirectionBlock* this, int level, DWORD number, BYTE* block, DWORD* blockAddress)
{
    int returnCode;
    if (level == 1) {
        
        *blockAddress = this->dataPtr[number];
        returnCode = DAM_read(this->dataPtr[number], block);
    } else if (level == 2) {
        
        unsigned int numOfPointersInBlock = fileSystem.superBlock.BlockSize / sizeof(DWORD);
        unsigned int numOfPointersInIndirectionBlock = numOfPointersInBlock * numOfPointersInBlock;
        unsigned int singleIndPointerNumber = number / numOfPointersInIndirectionBlock;
        unsigned int numberInIndirectionPointer = number % numOfPointersInIndirectionBlock;
        
        // Read the single indirection block
        if ((returnCode = DAM_read(this->dataPtr[singleIndPointerNumber], block)) == 0) {

            IndirectionBlock* indirectionBlock;
            IB_IndirectionBlock(indirectionBlock, block);
            // find in the indirection block the required block
            returnCode = IB_findBlockByNumber(indirectionBlock, 1, numberInIndirectionPointer, block, blockAddress);
        }
    }
    
    return returnCode;
}

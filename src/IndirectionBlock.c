#include "IndirectionBlock.h"
#include "FileSystem.h"
#include "DiscAccessManager.h"
#include <stdlib.h>

extern FileSystem fileSystem;

void IB_IndirectionBlock(IndirectionBlock* this, BYTE* block)
{
    this->dataPtr = (DWORD*) block;
}

Record* IB_find(IndirectionBlock* this, char* name, int level, BYTE* block, DWORD* blockAddress, Record*(*find)(DirectoryBlock*, char* param))
{
    if (this == NULL || this->dataPtr == NULL || name == NULL)
        return NULL;
    
    //If it is a single indirection
    if (level == 1){
        return FS_findRecordInArray(this->dataPtr, block, blockAddress, find, name, fileSystem.superBlock.BlockSize / sizeof(DWORD));
    }else if (level == 2){
        //If it is double indirection
        //Iterates over this indirection block (which is a double one). for each indirection block apointed tries to find
        int i;
        for(i = 0; i < fileSystem.superBlock.BlockSize / sizeof(DWORD);i++){
            //Reads one single indirection block
            BYTE blockOfIndirection[fileSystem.superBlock.BlockSize];
            if (!DAM_read(this->dataPtr[i], blockOfIndirection)){

                IndirectionBlock indirectionBlock;
                IB_IndirectionBlock(&indirectionBlock, blockOfIndirection);

                Record* foundRecord = IB_find(&indirectionBlock, name, 1, block, blockAddress, find);
                if (foundRecord != NULL){
                    return foundRecord;
                }
            }
            //else do nothing, jsut keep on trying
        }
        return NULL;
    }else{
        return NULL;
    }
}

int IB_allocateNewDirectoryBlock(IndirectionBlock* this, int level, BYTE* block, DWORD* blockAddress)
{
	if (this == NULL)
        return IB_INVALID_ARGUMENT;
    
    int writtenPos = TR_findEmptyPositionInArray(this->dataPtr, fileSystem.superBlock.BlockSize / sizeof(DWORD));

    if (writtenPos >= 0){
        if (TR_allocateNewBlock(blockAddress) == TR_ADDRECORD_SUCCESS){
            //Initialize block with null pointers
            memset(block, FS_NULL_BLOCK_POINTER, fileSystem.superBlock.BlockSize);
            //updates data pointer with new block address
            this->dataPtr[writtenPos] = blockAddress;
            //@TODO save
            return IB_SUCCESS;
        }else{
            return IB_CANT_ALLOCATE;
        }
    }
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

#include "IndirectionBlock.h"
#include "FileSystem.h"
#include "DiscAccessManager.h"
#include "FreeSpaceManager.h"
#include <stdlib.h>

extern FileSystem fileSystem;

#define numOfPointersInBlock(blockSize)     (blockSize / sizeof(DWORD))

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
        return FS_findRecordInArray(this->dataPtr, block, blockAddress, find, name, numOfPointersInBlock(fileSystem.superBlock.BlockSize));
    }else if (level == 2){
        //If it is double indirection
        //Iterates over this indirection block (which is a double one). for each indirection block apointed tries to find
        int i;
        for(i = 0; i < numOfPointersInBlock(fileSystem.superBlock.BlockSize); i++){
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
	return 0;
}

int IB_findBlockByNumber(IndirectionBlock* this, int level, DWORD number, BYTE* block, DWORD* blockAddress)
{
    int returnCode;
    if (level == 1) {
        
        *blockAddress = this->dataPtr[number];
        returnCode = DAM_read(this->dataPtr[number], block);
    } else if (level == 2) {
        
        unsigned int numOfPointersInBlock = numOfPointersInBlock(fileSystem.superBlock.BlockSize);
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

int IB_freeBlocks(IndirectionBlock* this, int level)
{
    int returnCode;
    unsigned int numOfPointersInBlock = numOfPointersInBlock(fileSystem.superBlock.BlockSize);
    
    if (level == 1) {
        
        for (int i=0; i<numOfPointersInBlock; i++) {
            
            // free this block, if an error happens, break the loop and returns the error code
            if (this->dataPtr[i] != FS_NULL_BLOCK_POINTER) {
                if ((returnCode = FSM_delete(this->dataPtr[i]) != 0)) {
                    break;
                }
            }
        }
    } else if (level == 2) {
        
        BYTE* block;
        for (int i=0; i<numOfPointersInBlock; i++) {
            
            // Read the single indirection block
            if (this->dataPtr[i] != FS_NULL_BLOCK_POINTER) {
                if ((returnCode = DAM_read(this->dataPtr[i], &block)) == 0) {

                    IndirectionBlock indirectionBlock;
                    IB_IndirectionBlock(&indirectionBlock, &block);

                    // free all the blocks in this inode, if an error happens, break the loop and returns the error code
                    if ((returnCode = IB_freeBlocks(&indirectionBlock, 1)) != 0) {
                        break;
                    }
                } else {
                    break;
                }
            }
        }
    }
    
    return returnCode;
}
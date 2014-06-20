#include "IndirectionBlock.h"
#include "FileSystem.h"
#include "DiscAccessManager.h"
#include "FreeSpaceManager.h"
#include "t2fs_record.h"
#include <stdlib.h>
#include <string.h>

extern FileSystem fileSystem;

#define numOfPointersInBlock(blockSize)     (blockSize / sizeof(DWORD))

void IB_IndirectionBlock(IndirectionBlock* this, BYTE* block)
{
    this->dataPtr = (DWORD*) block;
}

Record* IB_find(IndirectionBlock* this, char* name, int level, BYTE* block, DWORD* blockAddress, Record*(*find)(const DirectoryBlock* const, const char* param))
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
            if (!DAM_read(this->dataPtr[i], blockOfIndirection, FALSE)){

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
    
    if (level == 2){
        int writtenPos = TR_findEmptyPositionInArray(this->dataPtr, numOfPointersInBlock(fileSystem.superBlock.BlockSize));
        
        if (writtenPos >= 0){
            //Create new indirection block (child), which will be updated in fact
            DWORD childIndirectionBlockAddress;
            BYTE childIndirectionBlockMem[fileSystem.superBlock.BlockSize];
            if (TR_allocateNewIndirectionBlock(childIndirectionBlockMem, &childIndirectionBlockAddress) == TR_SUCCESS){
            
                //Allocate New Block, pointed by the new indirection block
                if (TR_allocateNewBlock(blockAddress)){
                    //Initialize block with null pointers
                    memset(block, FS_NULL_BLOCK_POINTER, fileSystem.superBlock.BlockSize);
                    //Saves new block to disc
                    if (DAM_write(*blockAddress, block, FALSE) != 0){
                        return IB_IOERROR;
                    }

                    IndirectionBlock childIndirectionBlock;
                    IB_IndirectionBlock(&childIndirectionBlock, childIndirectionBlockMem);
                    //The first position of the new ind block will be updated with the new block address
                    childIndirectionBlock.dataPtr[0] = *blockAddress;

                    //updates data pointer of doubleindptr with new indirection block address
                    this->dataPtr[writtenPos] = childIndirectionBlockAddress;
                    
                    //Write to disc the indirection block just modified
                    if (DAM_write(childIndirectionBlockAddress, childIndirectionBlockMem, FALSE) != 0){
                        return T2FS_IOERROR;
                    }else
                        return T2FS_SUCCESS;
                    
                    return IB_SUCCESS;
                }else{
                    return IB_CANT_ALLOCATE;
                }
            }else{
                return IB_CANT_ALLOCATE;
            }
        }else{
            return IB_CANT_ALLOCATE;
        }        
    }else{
        //Not implemented yet
        return IB_INVALID_ARGUMENT;
    }
}

int IB_findBlockByNumber(IndirectionBlock* this, int level, DWORD number, BYTE* block, DWORD* blockAddress)
{
    int returnCode;
    if (level == 1) {
        
        *blockAddress = this->dataPtr[number];
        returnCode = DAM_read(this->dataPtr[number], block, FALSE);
    } else if (level == 2) {
        
        unsigned int numOfPointersInBlock = numOfPointersInBlock(fileSystem.superBlock.BlockSize);
        unsigned int numOfPointersInIndirectionBlock = numOfPointersInBlock * numOfPointersInBlock;
        unsigned int singleIndPointerNumber = number / numOfPointersInIndirectionBlock;
        unsigned int numberInIndirectionPointer = number % numOfPointersInIndirectionBlock;
        
        // Read the single indirection block
        if ((returnCode = DAM_read(this->dataPtr[singleIndPointerNumber], block, FALSE)) == 0) {

            IndirectionBlock indirectionBlock;
            IB_IndirectionBlock(&indirectionBlock, block);
            // find in the indirection block the required block
            returnCode = IB_findBlockByNumber(&indirectionBlock, 1, numberInIndirectionPointer, block, blockAddress);
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
        
        BYTE block[fileSystem.superBlock.SuperBlockSize];
        for (int i=0; i<numOfPointersInBlock; i++) {
            
            // Read the single indirection block
            if (this->dataPtr[i] != FS_NULL_BLOCK_POINTER) {
                if ((returnCode = DAM_read(this->dataPtr[i], block, FALSE)) == 0) {

                    IndirectionBlock indirectionBlock;
                    IB_IndirectionBlock(&indirectionBlock, block);

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

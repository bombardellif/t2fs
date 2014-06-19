#include "IndirectionBlock.h"
#include <stdlib.h>


void IB_IndirectionBlock(IndirectionBlock* this, BYTE* block)
{
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
            //Reads one single indirection block
            if (!DAM_read(this->dataPtr[i], block)){

                IndirectionBlock indirectionBlock;
                IB_IndirectionBlock(&indirectionBlock, block);

                Record* foundRecord = IB_find(&indirectionBlock, name, 1, block, blockAddress);
                if (foundRecord != NULL){
                    return foundRecord;
                }
            }
            //else do nothing, jsut keep on trying
        }
    }else{
        return NULL;
    }
}

int IB_allocateNewDirectoryBlock(IndirectionBlock* this, int level, BYTE* block, DWORD* blockAddress)
{
	return 0;
}

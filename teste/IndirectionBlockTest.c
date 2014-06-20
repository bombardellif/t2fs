/*
 * File:   IndirectionBlockTest.c
 * Author: william
 *
 * Created on Jun 19, 2014, 9:54:28 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "t2fs.h"
#include "DirectoryBlock.h"
#include "FileSystem.h"
#include "DiscAccessManager.h"
#include "IndirectionBlock.h"
#include "FreeSpaceManager.h"

extern FileSystem fileSystem;

/*
 * Simple C Test Suite
 */

void testFind1() {
    printf("IndirectionBlockTest test 1\n");
    char name1[10] = "William\0";
    
    Record arq1;
    memset(&arq1, TYPEVAL_INVALIDO, sizeof(Record));
    arq1.TypeVal = TYPEVAL_REGULAR;
    strcpy(arq1.name, name1);
    
    DirectoryBlock dirBlock;
    BYTE block[fileSystem.superBlock.BlockSize];
    memset(block, TYPEVAL_INVALIDO, fileSystem.superBlock.BlockSize);
    
    DB_DirectoryBlock(&dirBlock, block);
    dirBlock.entries[0] = arq1;
    
    DWORD blockAddress;
    FSM_getFreeAddress(&blockAddress);
    printf("%u\n-\n", blockAddress);
    if (!DAM_write(blockAddress, block)){
        IndirectionBlock ib;
        BYTE iblock[fileSystem.superBlock.BlockSize];
        memset(iblock, FS_NULL_BLOCK_POINTER, fileSystem.superBlock.BlockSize);
        IB_IndirectionBlock(&ib, iblock);
        ib.dataPtr[0] = blockAddress;
        
        BYTE b[fileSystem.superBlock.BlockSize];
        DWORD ba;
        if (IB_find(&ib, name1, 1, b, &ba, DB_findByName) == IB_SUCCESS){
            if (strcmp((char*)block, (char*)b) == 0){
                if (blockAddress != ba)
                    printf("%%TEST_FAILED%% time=0 testname=testFind1 (IndirectionBlockTest) message=address are different\n");
            }else{
              printf("===%d\n",blockAddress);
              printf("===%d\n",ba);
              printf("===%d\n",((Record*)block)[0].TypeVal);
              printf("===%d\n",((Record*)b)[0].TypeVal);
              printf("%%TEST_FAILED%% time=0 testname=testFind1 (IndirectionBlockTest) message=blocks are different\n"); 
              
            }
        }else
            printf("%%TEST_FAILED%% time=0 testname=testFind1 (IndirectionBlockTest) message=find fail\n");
    }else
        printf("%%TEST_FAILED%% time=0 testname=testFind1 (IndirectionBlockTest) message=1st write fail\n");
}

int main(int argc, char** argv) {
    printf("%%SUITE_STARTING%% IndirectionBlockTest\n");
    
    if (FS_initilize() == FS_SUCCESS){    
        printf("%%SUITE_STARTED%%\n");

        printf("%%TEST_STARTED%% testFind1 (IndirectionBlockTest)\n");
        testFind1();
        printf("%%TEST_FINISHED%% time=0 testFind1 (IndirectionBlockTest) \n");

        printf("%%SUITE_FINISHED%% time=0\n");
    }else
        printf("Could not initialize File System\n");

    return (EXIT_SUCCESS);
}

/*
 * File:   DirectoryBlockTest.c
 * Author: william
 *
 * Created on Jun 19, 2014, 6:31:17 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DirectoryBlock.h"
#include "FileSystem.h"

extern FileSystem fileSystem;

/*
 * Simple C Test Suite
 */

void testFindByName1() {
    printf("DirectoryBlockTest test 1\n");
    char name1[10] = "William\0";
    char name2[10] = "Joao\0";
    char name3[10] = "";
    
    DirectoryBlock dirBlock;
    BYTE block[fileSystem.superBlock.BlockSize];
    memset(block, FS_NULL_BLOCK_POINTER, fileSystem.superBlock.BlockSize);
    
    DB_DirectoryBlock(&dirBlock, block);
    
    dirBlock.entries[0].TypeVal = TYPEVAL_REGULAR;
    dirBlock.entries[0].blocksFileSize = 0;
    dirBlock.entries[0].bytesFileSize = 0;
    strcpy(dirBlock.entries[0].name, name1);
    dirBlock.entries[0].dataPtr[0] = FS_NULL_BLOCK_POINTER;
    dirBlock.entries[0].dataPtr[1] = FS_NULL_BLOCK_POINTER;
    dirBlock.entries[0].singleIndPtr = FS_NULL_BLOCK_POINTER;
    dirBlock.entries[0].doubleIndPtr = FS_NULL_BLOCK_POINTER;
    
    char* name = name1;
    
    Record* rec = DB_findByName(&dirBlock, name);
    if(rec != &(dirBlock.entries[0]))
        printf("%%TEST_FAILED%% time=0 testname=testFindByName1 (DirectoryBlockTest) message=error message sample\n");
    
    name = name2;
    rec = DB_findByName(&dirBlock, name);
    if(rec != NULL)
        printf("%%TEST_FAILED%% time=0 testname=testFindByName1 (DirectoryBlockTest) message=error message sample\n");
    
    name = name3;
    rec = DB_findByName(&dirBlock, name);
    if(rec != NULL)
        printf("%%TEST_FAILED%% time=0 testname=testFindByName1 (DirectoryBlockTest) message=error message sample\n");
}

void testFindEmpty1() {
    printf("DirectoryBlockTest test 2\n");
    char name1[10] = "William\0";
    
    DirectoryBlock dirBlock;
    BYTE block[fileSystem.superBlock.BlockSize];
    memset(block, FS_NULL_BLOCK_POINTER, fileSystem.superBlock.BlockSize);
    
    DB_DirectoryBlock(&dirBlock, block);
    
    dirBlock.entries[0].TypeVal = TYPEVAL_REGULAR;
    dirBlock.entries[0].blocksFileSize = 0;
    dirBlock.entries[0].bytesFileSize = 0;
    strcpy(dirBlock.entries[0].name, name1);
    dirBlock.entries[0].dataPtr[0] = FS_NULL_BLOCK_POINTER;
    dirBlock.entries[0].dataPtr[1] = FS_NULL_BLOCK_POINTER;
    dirBlock.entries[0].singleIndPtr = FS_NULL_BLOCK_POINTER;
    dirBlock.entries[0].doubleIndPtr = FS_NULL_BLOCK_POINTER;
    
    
    Record* rec = DB_findEmpty(&dirBlock, NULL);
    if(rec != &(dirBlock.entries[1]))
        printf("%%TEST_FAILED%% time=0 testname=testFindEmpty1 (DirectoryBlockTest) message=error message sample\n");
    
    char* name = name1;
    rec = DB_findEmpty(&dirBlock, name);
    if(rec != &(dirBlock.entries[1]))
        printf("%%TEST_FAILED%% testname=testFindEmpty1 (DirectoryBlockTest) message=error message sample\n");
    
    //Full it
    memset(block, TYPEVAL_REGULAR, fileSystem.superBlock.BlockSize);
    DB_DirectoryBlock(&dirBlock, block);
    rec = DB_findEmpty(&dirBlock, name);
    if(rec != NULL)
        printf("%%TEST_FAILED%% testname=testFindEmpty1 (DirectoryBlockTest) message=error message sample\n");
}

int main(int argc, char** argv) {
    printf("%%SUITE_STARTING%% DirectoryBlockTest\n");
    
    if (FS_initilize() == FS_CREATE_FAIL){
        printf("Could not initialize File System\n");
    }else{
        printf("%%SUITE_STARTED%%\n");

        printf("%%TEST_STARTED%% testFindByName1 (DirectoryBlockTest)\n");
        testFindByName1();
        printf("%%TEST_FINISHED%% time=0 testFindByName1 (DirectoryBlockTest) \n");

        printf("%%TEST_STARTED%% testFindEmpty1 (DirectoryBlockTest)\n");
        testFindEmpty1();
        printf("%%TEST_FINISHED%% time=0 testFindEmpty1 (DirectoryBlockTest) \n");

        printf("%%SUITE_FINISHED%% time=0\n");
    }
    return (EXIT_SUCCESS);
}

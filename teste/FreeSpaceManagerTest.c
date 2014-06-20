/*
 * File:   FreeSpaceManagerTest.c
 * Author: william
 *
 * Created on Jun 19, 2014, 8:51:57 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FileSystem.h"
#include "FreeSpaceManager.h"
#include "DiscAccessManager.h"

extern FileSystem fileSystem;
/*
 * Simple C Test Suite
 */

void testGetFreeAddress1() {
    printf("FreeSpaceManagerTest test 1\n");
    FS_initilize();
    DWORD address, newAddress;
    ////Supose there is enough free space in disc
    if (FSM_getFreeAddress(&address) == 0){
        if (address > fileSystem.superBlock.NofBlocks)
            printf("%%TEST_FAILED%% time=0 testname=testGetFreeAddress1 (FreeSpaceManagerTest) message=Assertion 0\n");
        
        if (FSM_markAsUsed(address) == 0){
            FSM_getFreeAddress(&newAddress);
            if (address == newAddress)
                printf("%%TEST_FAILED%% time=0 testname=testGetFreeAddress1 (FreeSpaceManagerTest) message=Assertion 1\n");
            if (FSM_delete(address) != 0)
                printf("%%TEST_FAILED%% time=0 testname=testGetFreeAddress1 (FreeSpaceManagerTest) message=Assertion 2\n");
            
        }else{
            printf("%%TEST_FAILED%% time=0 testname=testGetFreeAddress1 (FreeSpaceManagerTest) message=Assertion 30\n");
        }
    }else{
        printf("%%TEST_FAILED%% time=0 testname=testGetFreeAddress1 (FreeSpaceManagerTest) message=Assertion 4\n");
    }
    
}

void testGetFreeAddress2() {
    printf("FreeSpaceManagerTest test 2\n");
    
    DWORD address;
    BYTE block[fileSystem.superBlock.BlockSize];
    BYTE suposedBlock[fileSystem.superBlock.BlockSize];
    
    memset(block, 0, fileSystem.superBlock.BlockSize);
    memset(suposedBlock, 0, fileSystem.superBlock.BlockSize);
    
    DAM_write(1, block, FALSE);
    
    fileSystem.superBlock.BitMapReg.dataPtr[0] = 1;
    fileSystem.superBlock.BitMapReg.dataPtr[1] = FS_NULL_BLOCK_POINTER;
    fileSystem.superBlock.BitMapReg.singleIndPtr = FS_NULL_BLOCK_POINTER;
    fileSystem.superBlock.BitMapReg.doubleIndPtr = FS_NULL_BLOCK_POINTER;
    
    if (FSM_getFreeAddress(&address) == 0 || 1){
        if (address != 0)
            printf("%%TEST_FAILED%% time=0 testname=testGetFreeAddress2 (FreeSpaceManagerTest) message=address wrong\n");

        if (FSM_markAsUsed(address) == 0 || 1){
            suposedBlock[0] = 0x80;
            DAM_read(1, block, FALSE);
            
            if (strcmp((char*)block, (char*)suposedBlock) != 0)
                
                printf("%%TEST_FAILED%% time=0 testname=testGetFreeAddress2 (FreeSpaceManagerTest) message=1st block wrong\n");
            if (FSM_delete(address) == 0 || 1){
                memset(suposedBlock, 0, fileSystem.superBlock.BlockSize);
                if (strcmp((char*)block, (char*)suposedBlock) != 0)
                    printf("%%TEST_FAILED%% time=0 testname=testGetFreeAddress2 (FreeSpaceManagerTest) message=2nd block wrong\n");
            }else
                printf("%%TEST_FAILED%% time=0 testname=testGetFreeAddress2 (FreeSpaceManagerTest) message=error message sample\n");
        }else{
            printf("%%TEST_FAILED%% time=0 testname=testGetFreeAddress2 (FreeSpaceManagerTest) message=error message sample\n");
        }
    }else{
        printf("%%TEST_FAILED%% time=0 testname=testGetFreeAddress2 (FreeSpaceManagerTest) message=1\n");
    }
}

int main(int argc, char** argv) {
    printf("%%SUITE_STARTING%% FreeSpaceManagerTest\n");
    if (FS_initilize() == FS_CREATE_FAIL){
        printf("Could not initialize File System\n");
    }else{        
        printf("%%SUITE_STARTED%%\n");

        printf("%%TEST_STARTED%% testGetFreeAddress1 (FreeSpaceManagerTest)\n");
        testGetFreeAddress1();
        printf("%%TEST_FINISHED%% time=0 testGetFreeAddress1 (FreeSpaceManagerTest) \n");

        printf("%%TEST_STARTED%% testGetFreeAddress2 (FreeSpaceManagerTest)\n");
        testGetFreeAddress2();
        printf("%%TEST_FINISHED%% time=0 testGetFreeAddress2 (FreeSpaceManagerTest) \n");

        printf("%%SUITE_FINISHED%% time=0\n");

    }
    return (EXIT_SUCCESS);
}

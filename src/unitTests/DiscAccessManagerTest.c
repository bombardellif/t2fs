/*
 * File:   DiscAccessManagerTest.c
 * Author: fernando
 *
 * Created on 19.06.2014, 16:49:33
 */

#include <stdio.h>
#include <stdlib.h>

#include "FileSystem.h"
#include "apidisk.h"

/*
 * Simple C Test Suite
 */

int DAM_read(DWORD blockAddress, BYTE* data);

void testDAM_read() {
    DWORD blockAddress;
    SuperBlock sb;
    FS_initilize();
    
    blockAddress = FS_SUPERBLOCK_ADDRESS;
    int result = DAM_read(blockAddress, (BYTE*)(&sb));
    
    if (!result) {
        printf("Id: \t%c%c%c%c\n", sb.Id[0],sb.Id[1],sb.Id[2],sb.Id[3]);
        printf("Version: \t%u\n", sb.Version);
        printf("SB Size: \t%u\n", sb.SuperBlockSize);
        printf("Disc Size: \t%u\n", sb.DiskSize);
        printf("No Blocks: \t%u\n", sb.NofBlocks);
        printf("Block Size: \t%u\n", sb.BlockSize);
    } else {
        printf("%%TEST_FAILED%% time=0 testname=testDAM_read (DiscAccessManagerTest) message=error message sample\n");
    }
}

int DAM_write(DWORD blockAddress, BYTE* data);

void testDAM_write() {
    DWORD blockAddress;
    BYTE* data;
    //int result = DAM_write(blockAddress, data);
    if (1 /*check result*/) {
        printf("%%TEST_FAILED%% time=0 testname=testDAM_write (DiscAccessManagerTest) message=error message sample\n");
    }
}

int main(int argc, char** argv) {
    printf("%%SUITE_STARTING%% DiscAccessManagerTest\n");
    printf("%%SUITE_STARTED%%\n");

    printf("%%TEST_STARTED%%  testDAM_read (DiscAccessManagerTest)\n");
    testDAM_read();
    printf("%%TEST_FINISHED%% time=0 testDAM_read (DiscAccessManagerTest)\n");

    printf("%%TEST_STARTED%%  testDAM_write (DiscAccessManagerTest)\n");
    testDAM_write();
    printf("%%TEST_FINISHED%% time=0 testDAM_write (DiscAccessManagerTest)\n");

    printf("%%SUITE_FINISHED%% time=0\n");

    return (EXIT_SUCCESS);
}

/*
 * File:   DiscAccessManagerTest.c
 * Author: fernando
 *
 * Created on 19.06.2014, 22:10:59
 */

#include <stdio.h>
#include <stdlib.h>
#include "DiscAccessManager.h"
#include "FileSystem.h"
#include "apidisk.h"
#include "DirectoryBlock.h"
#include "IndirectionBlock.h"

/*
 * Simple C Test Suite
 */

void testDAM_read() {
    DWORD blockAddress;
    SuperBlock sb;
    FS_initilize();
    
    blockAddress = FS_SUPERBLOCK_ADDRESS;
    int result = DAM_read(blockAddress, (BYTE*)(&sb), TRUE);
    
    if (!result) {
        printf("Id: \t%c%c%c%c\n", sb.Id[0],sb.Id[1],sb.Id[2],sb.Id[3]);
        printf("Version: \t%u\n", sb.Version);
        printf("SB Size: \t%u\n", sb.SuperBlockSize);
        printf("Disc Size: \t%u\n", sb.DiskSize);
        printf("No Blocks: \t%u\n", sb.NofBlocks);
        printf("Block Size: \t%u\n", sb.BlockSize);
        printf("rootPtr0: \t%x\n", sb.RootDirReg.dataPtr[0]);
        printf("rootPtr1: \t%x\n", sb.RootDirReg.dataPtr[1]);
        printf("singleIndPtr: \t%x\n", sb.RootDirReg.singleIndPtr);
        printf("doubleIndPtr: \t%x\n", sb.RootDirReg.doubleIndPtr);
        printf("bitmapPtr0: \t%x\n", sb.BitMapReg.dataPtr[0]);
        printf("bitmapPtr1: \t%x\n", sb.BitMapReg.dataPtr[1]);
        printf("singleBitmapPtr: \t%x\n", sb.BitMapReg.singleIndPtr);
        printf("doubleBitmapPtr: \t%x\n", sb.BitMapReg.doubleIndPtr);
    
        blockAddress = sb.RootDirReg.dataPtr[0];
        BYTE block[sb.BlockSize];
        result = DAM_read(blockAddress, (BYTE*)(&block), FALSE);
        
        if (!result) {
            DirectoryBlock root;
            DB_DirectoryBlock(&root, block);
            for (int i=0; i<16 ; i++) {
                printf("------ %d -------\n", i);
                printf("TypeVal: \t%x\n", root.entries[i].TypeVal);
                printf("Name: \t%s\n", root.entries[i].name);
                printf("Block FS: \t%u\n", root.entries[i].blocksFileSize);
                printf("Bytes FS: \t%u\n", root.entries[i].bytesFileSize);
            }
            
            BYTE block2[sb.BlockSize];
            result = DAM_read(root.entries[0].dataPtr[0], block2, FALSE);
            printf("%s\n", block2);
            
            result = DAM_read(root.entries[1].dataPtr[0], block2, FALSE);
            printf("%s\n", block2);
            result = DAM_read(root.entries[1].dataPtr[1], block2, FALSE);
            printf("%s\n", block2);
            result = DAM_read(root.entries[1].singleIndPtr, block2, FALSE);
            DirectoryBlock dir;
            DB_DirectoryBlock(&dir, block2);
            DAM_read(dir.entries[0].dataPtr[0], block2, FALSE);
            printf("%d\n", block2);
            
            result = DAM_read(root.entries[2].dataPtr[0], block2, FALSE);
            printf("%s\n", block2);
            result = DAM_read(root.entries[2].dataPtr[1], block2, FALSE);
            printf("%s\n", block2);
            
            
            result = DAM_read(root.entries[3].dataPtr[0], block2, FALSE);
            printf("%s\n", block2);
            
        } else {
            printf("%%TEST_FAILED%% time=0 testname=testDAM_read (DiscAccessManagerTest) message=error reading\n");
        }
        
        blockAddress = sb.BitMapReg.dataPtr[0];
        result = DAM_read(blockAddress, (BYTE*)(block), FALSE);
        
        if (!result) {
            IndirectionBlock root;
            IB_IndirectionBlock(&root, block);
            for (int i=0; i<16 ; i++) {
                printf("------ %d -------\n", i);
                printf("Value: \t%x\n", root.dataPtr[i]);
            }
        } else {
            printf("%%TEST_FAILED%% time=0 testname=testDAM_read (DiscAccessManagerTest) message=error reading\n");
        }
        
    } else {
        printf("%%TEST_FAILED%% time=0 testname=testDAM_read (DiscAccessManagerTest) message=error reading\n");
    }
}

void testDAM_write() {
    DWORD blockAddress;
    SuperBlock sb;
    FS_initilize();
    
    blockAddress = FS_SUPERBLOCK_ADDRESS;
    int result = DAM_read(blockAddress, (BYTE*)(&sb), TRUE);
    
    if (!result) {
        
        blockAddress = sb.RootDirReg.dataPtr[0];
        BYTE block[sb.BlockSize];
        result = DAM_read(blockAddress, (BYTE*)(block), FALSE);
        
        if (!result) {
            
            DirectoryBlock root;
            DB_DirectoryBlock(&root, block);
            
            BYTE block2[sb.BlockSize];
            result = DAM_read(root.entries[0].dataPtr[0], block2, FALSE);
            printf("%s\n", block2);
            strcat(block2, " - TESTE DE ALTERAÇÃO DE ARQUIVO");
            
            result = DAM_write(root.entries[0].dataPtr[0], block2, FALSE);
            if (!result) {
                
                DAM_read(root.entries[0].dataPtr[0], block2, FALSE);
                printf("%s\n", block2);
            } else {
                printf("%%TEST_FAILED%% time=0 testname=testDAM_write (DiscAccessManagerTest) message=error writing\n");
            }
        }
    } else {
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

/*
 * File:   FileSystemTest.c
 * Author: fernando
 *
 * Created on 20.06.2014, 02:33:54
 */

#include <stdio.h>
#include <stdlib.h>
#include "include/FileSystem.h"

/*
 * Simple C Test Suite
 */

void testFS_create() {
    FS_initilize();
    
    FilePath filePath;
    char path[] = "/novoArquivo";
    FP_FilePath(&filePath, path);
    
    t2fs_file result = FS_create(&filePath);
    if (result != 0) {
        printf("%%TEST_FAILED%% time=0 testname=testFS_create (FileSystemTest) message=%d\n", result);
    }
}

void testFS_createHandle() {
    OpenRecord openRecord;
    t2fs_file result = FS_createHandle(openRecord);
    if (1 /*check result*/) {
        printf("%%TEST_FAILED%% time=0 testname=testFS_createHandle (FileSystemTest) message=error message sample\n");
    }
}

void testFS_delete() {
    FS_initilize();
    
    FilePath filePath;
    char path[] = "/dir1";
    FP_FilePath(&filePath, path);
    int result = FS_delete(&filePath);
    FP_destroy(&filePath);
    
    if (result != 0) {
        printf("%%TEST_FAILED%% time=0 testname=testFS_delete (FileSystemTest) message=/dir1\n");
    }
    
    char path2[] = "/teste1";
    FP_FilePath(&filePath, path2);
    result = FS_delete(&filePath);
    FP_destroy(&filePath);
    
    if (result != 0) {
        printf("%%TEST_FAILED%% time=0 testname=testFS_delete (FileSystemTest) message=/teste1\n");
    }
    
    char path3[] = "/teste2";
    FP_FilePath(&filePath, path3);
    result = FS_delete(&filePath);
    FP_destroy(&filePath);
    
    if (result != 0) {
        printf("%%TEST_FAILED%% time=0 testname=testFS_delete (FileSystemTest) message=/teste2\n");
    }
    
    char path4[] = "/teste3";
    FP_FilePath(&filePath, path4);
    result = FS_delete(&filePath);
    FP_destroy(&filePath);
    
    if (result != 0) {
        printf("%%TEST_FAILED%% time=0 testname=testFS_delete (FileSystemTest) message=/teste3\n");
    }
}

void testFS_findRecordInArray() {
    DWORD* dataPtr;
    BYTE* block;
    DWORD* blockAddress;
    //Record * (*)(const DirectoryBlock * const, const char*) find;
    char* name;
    int count;
    //Record* result = FS_findRecordInArray(dataPtr, block, blockAddress, find, name, count);
    if (1 /*check result*/) {
        printf("%%TEST_FAILED%% time=0 testname=testFS_findRecordInArray (FileSystemTest) message=error message sample\n");
    }
}

void testFS_initilize() {
    int result = FS_initilize();
    if (1 /*check result*/) {
        printf("%%TEST_FAILED%% time=0 testname=testFS_initilize (FileSystemTest) message=error message sample\n");
    }
}

void testFS_open() {
    FilePath * const filePath;
    t2fs_file result = FS_open(filePath);
    if (1 /*check result*/) {
        printf("%%TEST_FAILED%% time=0 testname=testFS_open (FileSystemTest) message=error message sample\n");
    }
}

int main(int argc, char** argv) {
    printf("%%SUITE_STARTING%% FileSystemTest\n");
    printf("%%SUITE_STARTED%%\n");
/*
    printf("%%TEST_STARTED%%  testFS_create (FileSystemTest)\n");
    testFS_create();
    printf("%%TEST_FINISHED%% time=0 testFS_create (FileSystemTest)\n");
    
    printf("%%TEST_STARTED%%  testFS_createHandle (FileSystemTest)\n");
    testFS_createHandle();
    printf("%%TEST_FINISHED%% time=0 testFS_createHandle (FileSystemTest)\n");
*/
    printf("%%TEST_STARTED%%  testFS_delete (FileSystemTest)\n");
    testFS_delete();
    printf("%%TEST_FINISHED%% time=0 testFS_delete (FileSystemTest)\n");
/*
    printf("%%TEST_STARTED%%  testFS_findRecordInArray (FileSystemTest)\n");
    testFS_findRecordInArray();
    printf("%%TEST_FINISHED%% time=0 testFS_findRecordInArray (FileSystemTest)\n");

    printf("%%TEST_STARTED%%  testFS_initilize (FileSystemTest)\n");
    testFS_initilize();
    printf("%%TEST_FINISHED%% time=0 testFS_initilize (FileSystemTest)\n");

    printf("%%TEST_STARTED%%  testFS_open (FileSystemTest)\n");
    testFS_open();
    printf("%%TEST_FINISHED%% time=0 testFS_open (FileSystemTest)\n");*/

    printf("%%SUITE_FINISHED%% time=0\n");

    return (EXIT_SUCCESS);
}

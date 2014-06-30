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
    
    t2fs_file result = FS_create(&filePathm TYPEVAL_REGULAR);
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
    FS_initilize();
    
    FilePath filePath;
    char path[] = "/teste1";
    FP_FilePath(&filePath, path);
    
    t2fs_file result = FS_open(&filePath);
    if (result < 0) {
        printf("%%TEST_FAILED%% time=0 testname=testFS_open (FileSystemTest) message=error opening file\n");
    }
}

void testFS_close() {
    FS_initilize();
    
    FilePath filePath;
    char path[] = "/teste1";
    FP_FilePath(&filePath, path);
    
    t2fs_file result = FS_open(&filePath);
    if (result < 0) {
        printf("%%TEST_FAILED%% time=0 testname=testFS_open (FileSystemTest) message=error opening file\n");
    }
    
    t2fs_file resultClose = FS_close(result);
    if (resultClose < 0) {
        printf("%%TEST_FAILED%% time=0 testname=testFS_open (FileSystemTest) message=error closing file\n");
    }
}

void testFS_read() {
    FS_initilize();
    
    FilePath filePath;
    char path[] = "/teste1";
    FP_FilePath(&filePath, path);
    
    t2fs_file handle = FS_open(&filePath);
    if (handle < 0) {
        printf("%%TEST_FAILED%% time=0 testname=testFS_read (FileSystemTest) message=error opening file\n");
    }
    
    char buffer[1055];
    int result = FS_read(handle, buffer, 1054);
    buffer[29] = '-';
    buffer[1054] = '\0';
    printf("%s\n", buffer);
    if (result != 1054) {
        printf("%%TEST_FAILED%% time=0 testname=testFS_read (FileSystemTest) message=read bytes = %d\n", result);
    }
    
    /*result = FS_read(handle, buffer+10, 10);
    buffer[20] = '\0';
    printf("%s\n", buffer);
    if (result != 10) {
        printf("%%TEST_FAILED%% time=0 testname=testFS_read (FileSystemTest) message=read bytes = %d\n", result);
    }
    
    result = FS_read(handle, buffer, 1);
    printf("%s\n", buffer);
    if (result != 0) {
        printf("%%TEST_FAILED%% time=0 testname=testFS_read (FileSystemTest) message=read bytes = %d\n", result);
    }*/
    
    t2fs_file resultClose = FS_close(handle);
    if (resultClose < 0) {
        printf("%%TEST_FAILED%% time=0 testname=testFS_read (FileSystemTest) message=error closing file\n");
    }
}

void testFS_write() {
    FS_initilize();
    
    FilePath filePath;
    char path[] = "/teste1";
    FP_FilePath(&filePath, path);
    
    t2fs_file handle = FS_open(&filePath);
    if (handle < 0) {
        printf("%%TEST_FAILED%% time=0 testname=testFS_write (FileSystemTest) message=error opening file\n");
    }
    
    char buffer[30] = "Fernando Bombardelli da Silva";
    int result = FS_write(handle, buffer, 30);
    if (result != 30) {
        printf("%%TEST_FAILED%% time=0 testname=testFS_write (FileSystemTest) message=read bytes = %d\n", result);
    }
    
    char buffer2[1026] = "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. \nDuis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at";
    result = FS_write(handle, buffer2, 1025);
    //printf("%s\n", buffer);
    if (result != 1025) {
        printf("%%TEST_FAILED%% time=0 testname=testFS_write (FileSystemTest) message=read bytes = %d\n", result);
    }
}

void testFS_seek() {
    FS_initilize();
    
    FilePath filePath;
    char path[] = "/teste1";
    FP_FilePath(&filePath, path);
    
    t2fs_file handle = FS_open(&filePath);
    if (handle < 0) {
        printf("%%TEST_FAILED%% time=0 testname=testFS_seek (FileSystemTest) message=error opening file\n");
    }
    
    char buffer[30] = "-------- Bombardelli da Silva";
    int result = FS_write(handle, buffer, 29);
    if (result != 29) {
        printf("%%TEST_FAILED%% time=1 testname=testFS_seek (FileSystemTest) message=written bytes = %d\n", result);
    }
    
    result = FS_seek(handle, 0);
    if (result) {
        printf("%%TEST_FAILED%% time=2 testname=testFS_seek (FileSystemTest) message=seek return = %d\n", result);
    }
    
    char buffer1[35];
    result = FS_read(handle, buffer1, 35);
    if (result != 35) {
        printf("%%TEST_FAILED%% time=3 testname=testFS_seek (FileSystemTest) message=read bytes = %d\n", result);
    }
    buffer1[34] ='\0';
    printf("%s\n", buffer1);
    
    char buffer3[30] = "||------ Bombardelli da Sil |";
    result = FS_write(handle, buffer3, 29);
    if (result != 29) {
        printf("%%TEST_FAILED%% time=4 testname=testFS_seek (FileSystemTest) message=written bytes = %d\n", result);
    }
    
    result = FS_seek(handle, 29);
    if (result) {
        printf("%%TEST_FAILED%% time=5 testname=testFS_seek (FileSystemTest) message=seek return = %d\n", result);
    }
    
    result = FS_read(handle, buffer1, 35);
    if (result != 35) {
        printf("%%TEST_FAILED%% time=6 testname=testFS_seek (FileSystemTest) message=read bytes = %d\n", result);
    }
    buffer1[34] ='\0';
    printf("%s\n", buffer1);
    
    result = FS_seek(handle, -1);
    if (result) {
        printf("%%TEST_FAILED%% time=7 testname=testFS_seek (FileSystemTest) message=seek return = %d\n", result);
    }
    result = FS_read(handle, buffer1, 1);
    if (result != 0) {
        printf("%%TEST_FAILED%% time=8 testname=testFS_seek (FileSystemTest) message=read bytes = %d\n", result);
    }
    
    result = FS_seek(handle, 0);
    if (result) {
        printf("%%TEST_FAILED%% time=9 testname=testFS_seek (FileSystemTest) message=seek return = %d\n", result);
    }
    char buffer2[1055];
    result = FS_read(handle, buffer2, 1054);
    printf("%s\n", buffer2);
    if (!result) {
        printf("%%TEST_FAILED%% time=10 testname=testFS_seek (FileSystemTest) message=read bytes = %d\n", result);
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

    printf("%%TEST_STARTED%%  testFS_delete (FileSystemTest)\n");
    testFS_delete();
    printf("%%TEST_FINISHED%% time=0 testFS_delete (FileSystemTest)\n");

    printf("%%TEST_STARTED%%  testFS_findRecordInArray (FileSystemTest)\n");
    testFS_findRecordInArray();
    printf("%%TEST_FINISHED%% time=0 testFS_findRecordInArray (FileSystemTest)\n");

    printf("%%TEST_STARTED%%  testFS_initilize (FileSystemTest)\n");
    testFS_initilize();
    printf("%%TEST_FINISHED%% time=0 testFS_initilize (FileSystemTest)\n");
*/
    printf("%%TEST_STARTED%%  testFS_open (FileSystemTest)\n");
    testFS_open();
    printf("%%TEST_FINISHED%% time=0 testFS_open (FileSystemTest)\n");
    
    printf("%%TEST_STARTED%%  testFS_close (FileSystemTest)\n");
    testFS_close();
    printf("%%TEST_FINISHED%% time=0 testFS_close (FileSystemTest)\n");
    
    printf("%%TEST_STARTED%%  testFS_write (FileSystemTest)\n");
    testFS_write();
    printf("%%TEST_FINISHED%% time=0 testFS_write (FileSystemTest)\n");
    
    printf("%%TEST_STARTED%%  testFS_read (FileSystemTest)\n");
    testFS_read();
    printf("%%TEST_FINISHED%% time=0 testFS_read (FileSystemTest)\n");
    
    printf("%%TEST_STARTED%%  testFS_seek (FileSystemTest)\n");
    testFS_seek();
    printf("%%TEST_FINISHED%% time=0 testFS_seek (FileSystemTest)\n");
    printf("%%SUITE_FINISHED%% time=0\n");

    return (EXIT_SUCCESS);
}

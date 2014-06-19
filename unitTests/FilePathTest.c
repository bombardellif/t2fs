/*
 * File:   FilePathTest.c
 * Author: fernando
 *
 * Created on 19.06.2014, 13:48:15
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/FilePath.h"

/*
 * Simple C Test Suite
 */

void testFP_FilePath() {
    FilePath this;
    char path[] = "/this/is/a/test/sample/1";
    char pathWoLastNode[] = "/this/is/a/test/sample";
    
    FP_FilePath(&this, path);
    
    if (strcmp(this.path, path) != 0) {
        printf("%%TEST_FAILED%% time=0 testname=testFP_FilePath (FilePathTest) message=Assertion 1\n");
    }
    
    if (strcmp(this.pathWithoutLastNode, pathWoLastNode) != 0) {
        printf("%%TEST_FAILED%% time=0 testname=testFP_FilePath (FilePathTest) message=Assertion 2\n");
    }
    
    FP_destroy(&this);
}

void testFP_getNextNode() {
    FilePath this;
    char path[] = "/this/is/a/test/sample/1";
    char pathWoLastNode[] = "/this/is/a/test/sample";
    char expected[][7] = {"this", "is", "a", "test", "sample", "1"};
    char* result;
    
    FP_FilePath(&this, path);
    
    for (int i=0; i<6; i++) {
        result = FP_getNextNode(&this);
        
        if (strcmp(result, expected[i]) != 0) {
            printf("%%TEST_FAILED%% time=0 testname=testFP_FilePath (FilePathTest) message=Assertion %d - Expected: %s Got: %s\n", i, expected[i], result);
        }
    }
    
    result = FP_getNextNode(&this);
    if (result != NULL) {
        printf("%%TEST_FAILED%% time=0 testname=testFP_FilePath (FilePathTest) message=Assertion %d\n", 6);
    }
}

void testFP_hasNextNode() {
    FilePath this;
    char path[] = "/this/is/a/test/sample/1";
    char pathWoLastNode[] = "/this/is/a/test/sample";
    char expected[][7] = {"this", "is", "a", "test", "sample", "1"};
    char* resultNextNode;
    
    FP_FilePath(&this, path);
    
    if (! FP_hasNextNode(&this)) {
        printf("%%TEST_FAILED%% time=0 testname=testFP_FilePath (FilePathTest) message=Assertion %d\n", -2);
    }
    if (! FP_hasNextNode(&this)) {
        printf("%%TEST_FAILED%% time=0 testname=testFP_FilePath (FilePathTest) message=Assertion %d\n", -1);
    }
    
    for (int i=0; i<6; i++) {
        
        if (i % 2) {
            if (! FP_hasNextNode(&this)) {
                printf("%%TEST_FAILED%% time=0 testname=testFP_FilePath (FilePathTest) message=Assertion %d.0\n", i);
            }
        }
        
        resultNextNode = FP_getNextNode(&this);
        
        if (strcmp(resultNextNode, expected[i]) != 0) {
            printf("%%TEST_FAILED%% time=0 testname=testFP_FilePath (FilePathTest) message=Assertion %d.1\n", i);
        }
    }
    
    if (FP_hasNextNode(&this)) {
        printf("%%TEST_FAILED%% time=0 testname=testFP_FilePath (FilePathTest) message=Assertion %d\n", 6);
    }
    
    resultNextNode = FP_getNextNode(&this);
    if (resultNextNode != NULL) {
        printf("%%TEST_FAILED%% time=0 testname=testFP_FilePath (FilePathTest) message=Assertion %d\n", 7);
    }
}

void testFP_withoutLastNode() {
    FilePath this, new;
    char path[] = "/this/is/a/test/sample/1/";
    char pathWoLastNode[] = "/this/is/a/test/sample";
    char pathWoLastNode2[] = "/this/is/a/test";
    char expected[][7] = {"this", "is", "a", "test", "sample"};
    char* result;
    
    FP_FilePath(&this, path);
    
    FP_withoutLastNode(&this, &new);
    
    if (strcmp(new.path, pathWoLastNode) != 0) {
        printf("%%TEST_FAILED%% time=0 testname=testFP_FilePath (FilePathTest) message=Assertion 1\n");
    }
    
    for (int i=0; i<5; i++) {
        result = FP_getNextNode(&new);
        
        if (strcmp(result, expected[i]) != 0) {
            printf("%%TEST_FAILED%% time=0 testname=testFP_FilePath (FilePathTest) message=Assertion %d\n", i+3);
        }
    }
    
    result = FP_getNextNode(&new);
    if (result != NULL) {
        printf("%%TEST_FAILED%% time=0 testname=testFP_FilePath (FilePathTest) message=Assertion %d\n", 8);
    }
    
    if (strcmp(new.pathWithoutLastNode, pathWoLastNode2) != 0) {
        printf("%%TEST_FAILED%% time=0 testname=testFP_FilePath (FilePathTest) message=Assertion 2\n");
    }
}

int main(int argc, char** argv) {
    printf("%%SUITE_STARTING%% FilePathTest\n");
    printf("%%SUITE_STARTED%%\n");

    printf("%%TEST_STARTED%%  testFP_FilePath (FilePathTest)\n");
    testFP_FilePath();
    printf("%%TEST_FINISHED%% time=0 testFP_FilePath (FilePathTest)\n");

    printf("%%TEST_STARTED%%  testFP_getNextNode (FilePathTest)\n");
    testFP_getNextNode();
    printf("%%TEST_FINISHED%% time=0 testFP_getNextNode (FilePathTest)\n");

    printf("%%TEST_STARTED%%  testFP_hasNextNode (FilePathTest)\n");
    testFP_hasNextNode();
    printf("%%TEST_FINISHED%% time=0 testFP_hasNextNode (FilePathTest)\n");

    printf("%%TEST_STARTED%%  testFP_withoutLastNode (FilePathTest)\n");
    testFP_withoutLastNode();
    printf("%%TEST_FINISHED%% time=0 testFP_withoutLastNode (FilePathTest)\n");

    printf("%%SUITE_FINISHED%% time=0\n");

    return (EXIT_SUCCESS);
}

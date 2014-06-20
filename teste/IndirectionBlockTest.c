/*
 * File:   IndirectionBlockTest.c
 * Author: william
 *
 * Created on Jun 19, 2014, 9:54:28 PM
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * Simple C Test Suite
 */

void testFind1() {
    printf("IndirectionBlockTest test 1\n");
}

void test2() {
    printf("IndirectionBlockTest test 2\n");
    printf("%%TEST_FAILED%% time=0 testname=test2 (IndirectionBlockTest) message=error message sample\n");
}

int main(int argc, char** argv) {
    printf("%%SUITE_STARTING%% IndirectionBlockTest\n");
    printf("%%SUITE_STARTED%%\n");

    printf("%%TEST_STARTED%% testFind1 (IndirectionBlockTest)\n");
    testFind1();
    printf("%%TEST_FINISHED%% time=0 testFind1 (IndirectionBlockTest) \n");

    printf("%%TEST_STARTED%% test2 (IndirectionBlockTest)\n");
    test2();
    printf("%%TEST_FINISHED%% time=0 test2 (IndirectionBlockTest) \n");

    printf("%%SUITE_FINISHED%% time=0\n");

    return (EXIT_SUCCESS);
}

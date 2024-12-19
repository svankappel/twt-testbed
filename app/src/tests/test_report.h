#ifndef TEST_RESULT_H
#define TEST_RESULT_H


struct test_report{
    char test_title[128];
    char test_setup[256];
    char results[256];
    char latency_histogram[1024];
};


void test_report_print(struct test_report *report);


#endif //TEST_RESULT_H


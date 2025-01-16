/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


#ifndef TEST_RESULT_H
#define TEST_RESULT_H

/**
 * @struct test_report
 * @brief Structure to hold test report details.
 * 
 * This structure contains a test report, including the test title,
 * test setup, results, and latency histogram.
 * 
 */
struct test_report{
    char test_title[128];
    char test_setup[256];
    char results[256];
    char latency_histogram[4096];
};

/**
 * @brief Prints the test report details.
 * 
 * This function prints the details of the given test report, including the test title,
 * test setup, results, and latency histogram.
 * 
 * @param report Pointer to the test_report structure to be printed.
 */
void test_report_print(struct test_report *report);


#endif //TEST_RESULT_H


/**
 * @file test_template.h
 * @brief Header file for test template settings and initialization.
 *
 * This file contains a template to create a new test.
 */

#ifndef TEST_TEMPLATE_H
#define TEST_TEMPLATE_H

#include <zephyr/kernel.h>


/**
* @brief Initialize the test template.
*
* This function initializes the test
*
* @param sem Pointer to the test semaphore.
* @param test_settings Pointer to the test settings structure.
*/
void init_test_template(struct k_sem *sem, void * test_settings);

/**
* @struct test_template_settings
* @brief Structure to hold test settings.
*
* This structure can be customized for each test
*/
struct test_template_settings {
    uint32_t twt_interval;
    uint32_t twt_wake_interval;
    uint8_t test_number;
};


#endif // TEST_TEMPLATE_H
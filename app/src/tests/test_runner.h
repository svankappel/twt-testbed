/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include "test_global.h"

#include "test_sensor_ps.h"
#include "test_sensor_twt.h"

#include "test_large_packet_ps.h"
#include "test_large_packet_twt.h"

#include "test_actuator_ps.h"
#include "test_actuator_twt.h"

#include "test_multi_packet_ps.h"
#include "test_multi_packet_twt.h"

/**
 * @brief Run all tests
 * 
 * This function runs all the tests.
 * 
 */
void run_tests();


#endif //TEST_RUNNER_H
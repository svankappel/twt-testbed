/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/**
 * @file profiler.c
 * @brief Profiler module for GPIO control.
 *
 * This module provides functions to initialize and control GPIO pins
 * for profiling purposes. It includes functions to set, clear, and
 * output binary values to the GPIO pins.
 *
 * The GPIO pins are configured based on the device tree specifications.
 * The module supports four GPIO channels.
 *
 * @note The GPIO pins are configured as output and set to inactive state
 *       during initialization.
 */

#ifdef CONFIG_PROFILER_ENABLE

#ifndef PROFILER_H
#define PROFILER_H

/**
 * @brief Initializes the profiler.
 *
 * This function sets up the necessary resources and configurations
 * required for the profiler to start functioning. It should be called
 * before any other profiler-related functions are used.
 *
 * @return int Returns 0 on success, or a negative error code on failure.
 */
int profiler_init();

/**
 * @brief Sets profiler channel 0.
 *
 * This function sets the profiler channel 0.
 */
void profiler_ch0_set();

/**
 * @brief Clears profiler channel 0.
 *
 * This function clears the profiler channel 0.
 */
void profiler_ch0_clear();

/**
 * @brief Sets profiler channel 1.
 *
 * This function sets the profiler channel 1.
 */
void profiler_ch1_set();

/**
 * @brief Clears profiler channel 1.
 *
 * This function clears the profiler channel 1.
 */
void profiler_ch1_clear();

/**
 * @brief Sets profiler channel 2.
 *
 * This function sets the profiler channel 2.
 */
void profiler_ch2_set();

/**
 * @brief Clears profiler channel 2.
 *
 * This function clears the profiler channel 2.
 */
void profiler_ch2_clear();

/**
 * @brief Sets profiler channel 3.
 *
 * This function sets the profiler channel 3.
 */
void profiler_ch3_set();

/**
 * @brief Clears profiler channel 3.
 *
 * This function clears the profiler channel 3.
 */
void profiler_ch3_clear();

/**
 * @brief Outputs a binary value to the profiler.
 *
 * This function outputs a binary value to the profiler, which can be used
 * for various profiling purposes.
 *
 * @param value The binary value to output.
 */
void profiler_output_binary(int value);

/**
 * @brief Clears all profiler channels.
 *
 * This function clears all profiler channels.
 */
void profiler_all_clear();

/**
 * @brief Sets all profiler channels.
 *
 * This function sets all profiler channels.
 */
void profiler_all_set();

/**
 * @brief Outputs the next value to the profiler.
 *
 * This function outputs the next value to the profiler.
 * Starts from 1 and increments by 1 each time it is called.
 */
void profiler_output_next();

#endif //PROFILER_H


#endif //CONFIG_PROFILER_ENABLE
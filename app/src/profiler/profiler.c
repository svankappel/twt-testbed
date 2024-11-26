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

#include "profiler.h"

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define ZEPHYR_USER_NODE DT_PATH(zephyr_user)

static const struct gpio_dt_spec gpio0 = GPIO_DT_SPEC_GET(ZEPHYR_USER_NODE, io0_gpios);
static const struct gpio_dt_spec gpio1 = GPIO_DT_SPEC_GET(ZEPHYR_USER_NODE, io1_gpios);
static const struct gpio_dt_spec gpio2 = GPIO_DT_SPEC_GET(ZEPHYR_USER_NODE, io2_gpios);
static const struct gpio_dt_spec gpio3 = GPIO_DT_SPEC_GET(ZEPHYR_USER_NODE, io3_gpios);

int profiler_init() {
    int ret;

    ret = gpio_pin_configure_dt(&gpio0, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        return ret;
    }

    ret = gpio_pin_configure_dt(&gpio1, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        return ret;
    }

    ret = gpio_pin_configure_dt(&gpio2, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        return ret;
    }

    ret = gpio_pin_configure_dt(&gpio3, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        return ret;
    }

    return 0;
}

void profiler_ch0_set() {
    gpio_pin_set_dt(&gpio0, 1);
}

void profiler_ch0_clear() {
    gpio_pin_set_dt(&gpio0, 0);
}

void profiler_ch1_set() {
    gpio_pin_set_dt(&gpio1, 1);
}

void profiler_ch1_clear() {
    gpio_pin_set_dt(&gpio1, 0);
}

void profiler_ch2_set() {
    gpio_pin_set_dt(&gpio2, 1);
}

void profiler_ch2_clear() {
    gpio_pin_set_dt(&gpio2, 0);
}

void profiler_ch3_set() {
    gpio_pin_set_dt(&gpio3, 1);
}

void profiler_ch3_clear() {
    gpio_pin_set_dt(&gpio3, 0);
}

void profiler_output_binary(int value) {
    gpio_pin_set_dt(&gpio0, value & 0x01);
    gpio_pin_set_dt(&gpio1, (value >> 1) & 0x01);
    gpio_pin_set_dt(&gpio2, (value >> 2) & 0x01);
    gpio_pin_set_dt(&gpio3, (value >> 3) & 0x01);
}

void profiler_all_clear() {
    gpio_pin_set_dt(&gpio0, 0);
    gpio_pin_set_dt(&gpio1, 0);
    gpio_pin_set_dt(&gpio2, 0);
    gpio_pin_set_dt(&gpio3, 0);
}

void profiler_all_set() {
    gpio_pin_set_dt(&gpio0, 1);
    gpio_pin_set_dt(&gpio1, 1);
    gpio_pin_set_dt(&gpio2, 1);
    gpio_pin_set_dt(&gpio3, 1);
}

void profiler_output_next() {
    static int i = 1;
    profiler_output_binary(i);
    i++;
}

#endif //CONFIG_PROFILER_ENABLE
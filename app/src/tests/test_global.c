#include "test_global.h"

#include <zephyr/kernel.h>

K_THREAD_STACK_DEFINE(thread_stack, TEST_THREAD_STACK_SIZE);

K_SEM_DEFINE(test_sem, 0, 1);



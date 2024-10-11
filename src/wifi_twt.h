#ifndef WIFI_TWT_H
#define WIFI_TWT_H
#include <zephyr/kernel.h>

int wifi_twt_init();
int wifi_toggle_twt(uint32_t twt_wake_interval_ms, uint32_t twt_interval_ms);

#endif // WIFI_TWT_H
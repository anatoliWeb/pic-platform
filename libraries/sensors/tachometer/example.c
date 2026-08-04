/*
 * File: libraries/sensors/tachometer/example.c
 */

#include "libraries/sensors/tachometer/tachometer.h"

static tachometer_t g_tachometer;
static const tachometer_config_t g_tachometer_config =
{
    2u,
    900u,
    250u,
    500u,
    800u
};

void tachometer_example_init(void)
{
    (void)tachometer_init(&g_tachometer, &g_tachometer_config);
}

void tachometer_example_start(uint32_t now_us)
{
    tachometer_set_expected_running(&g_tachometer, 1u, now_us);
}

void tachometer_example_pulse(uint32_t now_us)
{
    (void)tachometer_on_pulse(&g_tachometer, now_us);
}

void tachometer_example_poll(uint32_t now_us)
{
    tachometer_process(&g_tachometer, now_us);
}

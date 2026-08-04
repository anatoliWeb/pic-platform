/*
 * File: examples-projects/xc8/sensors/tachometer.X/main.c
 */

#include "project_config.h"

#include "libraries/sensors/tachometer/tachometer.h"

static tachometer_t g_tachometer;
static volatile tachometer_status_t g_last_status;
static volatile uint16_t g_last_rpm;
static volatile uint32_t g_last_pulse_count;

static const tachometer_config_t g_tachometer_config =
{
    2u,
    900u,
    250u,
    500u,
    800u
};

static void run_simulation(void)
{
    (void)tachometer_init(&g_tachometer, &g_tachometer_config);
    tachometer_set_expected_running(&g_tachometer, 1u, 0u);

    (void)tachometer_on_pulse(&g_tachometer, 0u);
    (void)tachometer_on_pulse(&g_tachometer, 10000u);
    tachometer_process(&g_tachometer, 120000u);

    g_last_status = tachometer_get_status(&g_tachometer);
    g_last_rpm = tachometer_get_rpm(&g_tachometer);
    g_last_pulse_count = tachometer_get_pulse_count(&g_tachometer);

    (void)tachometer_on_pulse(&g_tachometer, 110000u);
    tachometer_process(&g_tachometer, 700000u);
    g_last_status = tachometer_get_status(&g_tachometer);
    tachometer_reset(&g_tachometer);
}

void main(void)
{
    run_simulation();

    while (1)
    {
        tachometer_process(&g_tachometer, 1000000u);
    }
}

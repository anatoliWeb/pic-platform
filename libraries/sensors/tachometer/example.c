/*
 * File: libraries/sensors/tachometer/example.c
 *
 * Integration outline for the tachometer library. The application owns the
 * tachometer_t instance, feeds monotonic microsecond timestamps, and calls the
 * step functions from the right context (a pulse ISR and the main loop).
 */

#include "libraries/sensors/tachometer/tachometer.h"

static tachometer_t g_tachometer;

/*
 * 2 pulses per revolution; RPM below 900 is reported as TOO_SLOW. The startup
 * grace window (250 ms) and the pulse interval floor (800 us) shape RPM and
 * status while the rotor spins up.
 */
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

/* Call once the rotor is expected to spin. Moving to expected-running re-arms
 * the measurement so RPM and pulse_count start from a fresh session. */
void tachometer_example_start(uint32_t now_us)
{
    tachometer_set_expected_running(&g_tachometer, 1u, now_us);
}

/* Feed from the pulse interrupt with the captured microsecond timestamp. */
void tachometer_example_pulse(uint32_t now_us)
{
    (void)tachometer_on_pulse(&g_tachometer, now_us);
}

/* Advance timeout and status from a periodic context (main loop or timer). */
void tachometer_example_poll(uint32_t now_us)
{
    tachometer_process(&g_tachometer, now_us);
}
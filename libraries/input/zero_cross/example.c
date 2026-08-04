/*
 * File: libraries/input/zero_cross/example.c
 *
 * Minimal reusable example for the zero-cross detector. It does not bind to any
 * pin, timer or output; the application is responsible for feeding edges and
 * dispatching the produced event to its consumers.
 */

#include "libraries/input/zero_cross/zero_cross.h"

static zero_cross_t g_zero_cross;
static const zero_cross_config_t g_zero_cross_config =
{
    500u,   /* timeout_ms: fail-safe all-off when edges stop. */
    7500u,  /* min_half_cycle_us: accept 60 Hz (8333 us). */
    12000u, /* max_half_cycle_us: accept 50 Hz (10000 us). */
    500u,   /* glitch_reject_us. */
    2u      /* recovery_event_count. */
};

void zero_cross_example_edge(uint32_t now_us)
{
    zero_cross_event_t event;

    if (zero_cross_on_edge(&g_zero_cross, now_us, &event) != 0u)
    {
        (void)event;
        /* Dispatch the same event to ac_phase_control and future consumers. */
    }
}

void zero_cross_example_init(void)
{
    (void)zero_cross_init(&g_zero_cross, &g_zero_cross_config);
}
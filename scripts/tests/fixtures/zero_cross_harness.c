/*
 * File: scripts/tests/fixtures/zero_cross_harness.c
 *
 * XC8 compile/preprocessing fixture for the zero_cross library in isolation.
 * It proves the library builds without ac_phase_control, timer drivers or any
 * pin binding. Compiled with the XC8 toolchain for PIC18F452
 * (see test_zero_cross.py).
 */

#include <xc.h>
#include <stdint.h>

#include "core/compiler.h"
#include "core/types.h"

#include "libraries/input/zero_cross/zero_cross.c"

static zero_cross_t g_zero_cross;

static const zero_cross_config_t g_config =
{
    500u,   /* timeout_ms. */
    7500u,  /* min_half_cycle_us. */
    12000u, /* max_half_cycle_us. */
    500u,   /* glitch_reject_us. */
    2u      /* recovery_event_count. */
};

static uint8_t g_consumer_a_count;
static uint8_t g_consumer_b_count;

static void consumer_a(const zero_cross_event_t* event)
{
    if (event->sequence != 0u)
    {
        g_consumer_a_count++;
    }
}

static void consumer_b(const zero_cross_event_t* event)
{
    if (event->half_cycle_us != 0u)
    {
        g_consumer_b_count++;
    }
}

void main(void)
{
    zero_cross_event_t event;
    uint32_t now_us;

    if (zero_cross_init(&g_zero_cross, &g_config) != DRV_STATUS_OK)
    {
        while (1)
        {
        }
    }

    now_us = 1000u;

    /* Two consumers observe the same dispatched event. */
    if (zero_cross_on_edge(&g_zero_cross, now_us, &event) == 0u)
    {
        consumer_a(&event);
        consumer_b(&event);
    }

    now_us += 10000u;

    if (zero_cross_on_edge(&g_zero_cross, now_us, &event) != 0u)
    {
        consumer_a(&event);
        consumer_b(&event);
    }

    zero_cross_process(&g_zero_cross, now_us + 600000u);
    zero_cross_reset(&g_zero_cross);

    while (1)
    {
    }
}

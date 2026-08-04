/*
 * File: examples-projects/xc8/input/zero_cross.X/main.c
 */

#include "project_config.h"

#include "core/compiler.h"
#include "core/types.h"

#include "libraries/input/zero_cross/zero_cross.h"

#if defined(DRV_COMPILER_C18)
#include <p18f452.h>
#endif

#if defined(DRV_COMPILER_XC8)
#include <xc.h>
#endif

/*
 * Standalone zero-cross demo for PIC18F452.
 *
 * The example feeds explicit synthetic edges into the reusable detector so the
 * lifecycle is visible without any triac, relay or timer dependency:
 * WAITING -> ALIVE -> LOST -> recovery WAITING -> ALIVE.
 */

static zero_cross_t g_zero_cross;

static const zero_cross_config_t g_zero_cross_config =
{
    200u,   /* timeout_ms. */
    7500u,  /* min_half_cycle_us. */
    12000u, /* max_half_cycle_us. */
    500u,   /* glitch_reject_us. */
    2u      /* recovery_event_count. */
};

static void status_outputs(void)
{
    zero_cross_status_t status;
    zero_cross_frequency_t frequency;

    status = zero_cross_get_status(&g_zero_cross);
    frequency = zero_cross_get_frequency(&g_zero_cross);

    LATD &= (uint8_t)~0x0FU;

    if (status == ZERO_CROSS_STATUS_WAITING)
    {
        LATDbits.LATD0 = 1u;
    }
    else if (status == ZERO_CROSS_STATUS_ALIVE)
    {
        LATDbits.LATD1 = 1u;
    }
    else if (status == ZERO_CROSS_STATUS_LOST)
    {
        LATDbits.LATD2 = 1u;
    }

    if (frequency == ZERO_CROSS_FREQUENCY_50_HZ)
    {
        LATDbits.LATD3 = 1u;
    }
}

static void feed_edge(uint32_t now_us)
{
    zero_cross_event_t event;

    if (zero_cross_on_edge(&g_zero_cross, now_us, &event) != 0u)
    {
        if (event.frequency == ZERO_CROSS_FREQUENCY_60_HZ)
        {
            LATDbits.LATD3 = 0u;
        }
        else
        {
            LATDbits.LATD3 = 1u;
        }
    }

    status_outputs();
}

void main(void)
{
    ADCON1 = 0x07u;

#if defined(CMCON)
    CMCON = 0x07u;
#endif

    TRISD &= (uint8_t)~0x0FU;
    LATD &= (uint8_t)~0x0FU;

    (void)zero_cross_init(&g_zero_cross, &g_zero_cross_config);
    status_outputs();

    feed_edge(0u);
    feed_edge(10000u);

    zero_cross_process(&g_zero_cross, 260000u);
    status_outputs();

    feed_edge(270000u);
    feed_edge(280000u);
    feed_edge(290000u);

    zero_cross_process(&g_zero_cross, 500000u);
    status_outputs();

    zero_cross_reset(&g_zero_cross);
    status_outputs();

    feed_edge(1000u);
    feed_edge(9333u);

    while (1)
    {
        zero_cross_process(&g_zero_cross, 1000000u);
    }
}

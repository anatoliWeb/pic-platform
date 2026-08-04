/*
 * File: scripts/tests/fixtures/zero_cross_event_harness.c
 *
 * XC8 compile/preprocessing fixture proving the event path: the caller feeds
 * the shared zero-cross detector and dispatches the same event to the
 * ac_phase_control group (and would to any other consumer). Exercises the
 * event API and the legacy wrapper, plus the relay/timeout regressions.
 * Compiled with the XC8 toolchain for PIC18F452 (see test_zero_cross.py).
 */

#include <xc.h>
#include <stdint.h>

#include "core/compiler.h"
#include "core/types.h"

#include "libraries/output/ac_phase_control/ac_phase_control.c"
#include "libraries/input/zero_cross/zero_cross.c"
#include "drivers/timers/timer2/timer2.c"

static ac_phase_control_group_t g_group;
static ac_phase_control_channel_t g_channels[AC_PHASE_CONTROL_MAX_CHANNELS];
static volatile uint8_t g_lat;
static volatile uint8_t g_tris;

static const ac_phase_control_config_t g_config =
{
    10000u, /* half-cycle, 10 ms. */
    500u,   /* min delay. */
    9500u,  /* max delay. */
    120u,   /* gate pulse. */
    98u,    /* relay on. */
    96u,    /* relay off. */
    50u,    /* break-before-make. */
    200u,   /* min on. */
    200u,   /* min off. */
    500u    /* zero-cross timeout. */
};

static uint8_t g_other_consumer_count;
void main(void)
{
    zero_cross_event_t event;
    uint32_t now_us;

    if (ac_phase_control_init_group(
            &g_group,
            AC_PHASE_CONTROL_TIMER2,
            &g_config,
            g_channels,
            AC_PHASE_CONTROL_MAX_CHANNELS) != DRV_STATUS_OK)
    {
        while (1)
        {
        }
    }

    (void)ac_phase_control_attach_channel(
        &g_group,
        0u,
        &g_lat,
        &g_tris,
        (uint8_t)(1u << 0u));
    (void)ac_phase_control_attach_channel_relay(
        &g_group,
        0u,
        &g_lat,
        &g_tris,
        (uint8_t)(1u << 4u));
    (void)ac_phase_control_set_power_percent(&g_group, 0u, 50u);

    now_us = 1000u;

    /* First edge: arms the detector, no event yet. */
    (void)zero_cross_on_edge(&g_group.zero_cross, now_us, &event);

    /* Second edge: valid half-cycle -> ALIVE event dispatched to consumers. */
    now_us += 10000u;

    if (zero_cross_on_edge(&g_group.zero_cross, now_us, &event) != 0u)
    {
        ac_phase_control_on_zero_cross_event(&g_group, &event);
        g_other_consumer_count++;
    }

    /* Advance the timer tick and run the relay/timeout state machine. */
    ac_phase_control_update_us(&g_group, 102u);
    ac_phase_control_process(&g_group);

    /* Legacy wrapper still compiles and drives the same detector. */
    ac_phase_control_on_zero_cross(&g_group);

    /* Lost sync: force a timeout so the group enters ZERO_CROSS_LOST. */
    zero_cross_process(&g_group.zero_cross, now_us + 600000u);
    ac_phase_control_process(&g_group);

    ac_phase_control_all_off(&g_group);

    while (1)
    {
    }
}

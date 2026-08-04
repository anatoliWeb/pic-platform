/*
 * File: scripts/tests/fixtures/ac_phase_control_harness.c
 *
 * XC8 compile/preprocessing fixture for the ac_phase_control library.
 * It includes the real library source and the Timer2 driver source, so
 * Timer2's internal relative includes resolve against the repository.
 * Compiled with the XC8 toolchain for PIC18F452 (see test_ac_phase_control.py).
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

void main(void)
{
    drv_status_t status;

    status = ac_phase_control_init_group(
        &g_group,
        AC_PHASE_CONTROL_TIMER2,
        &g_config,
        g_channels,
        AC_PHASE_CONTROL_MAX_CHANNELS);

    if (status == DRV_STATUS_OK)
    {
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
    }

    ac_phase_control_on_zero_cross(&g_group);
    ac_phase_control_update_us(&g_group, 102u);
    ac_phase_control_process(&g_group);
    ac_phase_control_all_off(&g_group);

    while (1)
    {
    }
}

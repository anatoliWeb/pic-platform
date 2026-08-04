/*
 * File: libraries/output/ac_phase_control/example.c
 */

#include "libraries/output/ac_phase_control/ac_phase_control.h"

/*
 * Example usage for the multi-channel phase-control module with optional
 * relay bypass for full-power mode.
 *
 * Replace the null register pointers below with real LAT/TRIS registers
 * from the target project before using this file in a build.
 */

static ac_phase_control_group_t g_example_group;
static ac_phase_control_channel_t g_example_channels[AC_PHASE_CONTROL_MAX_CHANNELS];

/*
 * Relay threshold and hysteresis use the defaults (98 % / 96 %) when the
 * fields are left at zero. A non-zero zero_cross_timeout_ms enables the
 * fail-safe shutdown when zero-cross events stop arriving.
 */
static const ac_phase_control_config_t g_example_config =
{
    10000u, /* half-cycle for 50 Hz AC. */
    500u,   /* minimum firing delay. */
    9500u,  /* maximum firing delay. */
    120u,   /* gate pulse width. */
    0u,     /* relay on threshold (default 98 %). */
    0u,     /* relay off threshold (default 96 %). */
    0u,     /* break-before-make (default 50 ms). */
    0u,     /* minimum relay ON time (default 200 ms). */
    0u,     /* minimum relay OFF time (default 200 ms). */
    500u    /* zero-cross timeout (ms). */
};

void ac_phase_control_example_init(void)
{
    (void)ac_phase_control_init_group(&g_example_group,
                                      AC_PHASE_CONTROL_TIMER2,
                                      &g_example_config,
                                      g_example_channels,
                                      AC_PHASE_CONTROL_MAX_CHANNELS);

    (void)ac_phase_control_attach_channel(&g_example_group,
                                          0u,
                                          (volatile uint8_t*)0,
                                          (volatile uint8_t*)0,
                                          /* Bit mask example: RD0 -> 1U << 0U. */
                                          (uint8_t)(1u << 0u));
    (void)ac_phase_control_attach_channel_relay(&g_example_group,
                                                0u,
                                                (volatile uint8_t*)0,
                                                (volatile uint8_t*)0,
                                                /* Bit mask example: RD4 -> 1U << 4U. */
                                                (uint8_t)(1u << 4u));
    (void)ac_phase_control_set_power_percent(&g_example_group, 0u, 50u);
}

void ac_phase_control_example_zero_cross(void)
{
    ac_phase_control_on_zero_cross(&g_example_group);
}

void ac_phase_control_example_update_us(uint16_t elapsed_us)
{
    ac_phase_control_update_us(&g_example_group, elapsed_us);
}

/*
 * Call periodically from the main loop so the relay state machine and the
 * zero-cross timeout can advance. The gate-pulse timing stays in the timer
 * interrupt path via ac_phase_control_update_us().
 */
void ac_phase_control_example_process(void)
{
    ac_phase_control_process(&g_example_group);
}

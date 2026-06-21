#include "libraries/output/ac_phase_control/ac_phase_control.h"

/*
 * Example usage for the low-voltage multi-channel phase-control module.
 *
 * Replace the null register pointers below with real LAT/TRIS registers
 * from the target project before using this file in a build.
 */

static ac_phase_control_group_t g_example_group;
static ac_phase_control_channel_t g_example_channels[AC_PHASE_CONTROL_MAX_CHANNELS];

static const ac_phase_control_config_t g_example_config =
{
    10000u,
    500u,
    9500u,
    120u
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

/*
 * File: examples-projects/xc8/actuator/cooldown_output.X/main.c
 */

#include "project_config.h"

#include "libraries/actuator/cooldown_output/cooldown_output.h"

static cooldown_output_t g_output;
static volatile uint8_t g_active;
static volatile uint8_t g_cooling;
static volatile uint32_t g_remaining;

static const cooldown_output_config_t g_output_config =
{
    250u,
    (cooldown_output_callback_t)0,
    (void*)0
};

static void run_simulation(void)
{
    (void)cooldown_output_init(&g_output, &g_output_config);
    cooldown_output_set_requested(&g_output, 1u, 0u);
    cooldown_output_set_requested(&g_output, 0u, 100u);
    cooldown_output_process(&g_output, 200u);

    g_active = cooldown_output_is_active(&g_output);
    g_cooling = cooldown_output_is_cooling_down(&g_output);
    g_remaining = cooldown_output_get_remaining_ms(&g_output, 200u);

    cooldown_output_set_requested(&g_output, 1u, 220u);
    cooldown_output_force_off(&g_output);
}

void main(void)
{
    run_simulation();

    while (1)
    {
        cooldown_output_process(&g_output, 1000u);
    }
}

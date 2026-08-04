/*
 * File: examples-projects/xc8/input/digital_input.X/main.c
 */

#include "project_config.h"

#include "libraries/input/digital_input/digital_input.h"

static digital_input_t g_input;
static volatile uint8_t g_active;
static volatile uint8_t g_rose;
static volatile uint8_t g_fell;
static volatile uint8_t g_latched;

static const digital_input_config_t g_input_config =
{
    20u,
    0u,
    1u,
    1u
};

static void run_simulation(void)
{
    (void)digital_input_init(&g_input, &g_input_config);

    digital_input_update(&g_input, 1u, 0u);
    digital_input_update(&g_input, 0u, 5u);
    digital_input_update(&g_input, 0u, 25u);
    digital_input_update(&g_input, 1u, 40u);
    digital_input_update(&g_input, 1u, 60u);

    g_active = digital_input_is_active(&g_input);
    g_rose = digital_input_rose(&g_input);
    g_fell = digital_input_fell(&g_input);
    g_latched = digital_input_is_latched(&g_input);
    digital_input_clear_latch(&g_input);
}

void main(void)
{
    run_simulation();

    while (1)
    {
        digital_input_update(&g_input, g_active, 1000u);
    }
}

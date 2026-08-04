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

/* 20 ms debounce, active-low (raw 0 = active), latch enabled. The initial raw
 * level (1) is committed at init without emitting an edge event. */
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

    /* Raw samples entering the debounce state machine at millisecond times.
     * Holding raw 0 for the full debounce window commits the active state;
     * a short glitch back to 1 would cancel the pending transition instead. */
    digital_input_update(&g_input, 1u, 0u);
    digital_input_update(&g_input, 0u, 5u);
    digital_input_update(&g_input, 0u, 25u);
    digital_input_update(&g_input, 1u, 40u);
    digital_input_update(&g_input, 1u, 60u);

    /* rose()/fell() consume their flags: each returns the pending edge once.
     * The latch stays set until clear_latch() acknowledges the event. */
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

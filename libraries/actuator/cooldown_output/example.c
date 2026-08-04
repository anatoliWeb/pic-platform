/*
 * File: libraries/actuator/cooldown_output/example.c
 *
 * Integration outline for the cooldown output helper. The application owns the
 * cooldown_output_t instance, drives it with logical on/off requests, and
 * calls process() periodically so the cooldown delay can advance. The callback
 * is a hardware adapter: it only mirrors the physical active state, and the
 * library never touches a pin directly.
 */

#include "libraries/actuator/cooldown_output/cooldown_output.h"

static cooldown_output_t g_output;
static uint8_t g_physical_output;

/*
 * Callback invoked only when the physical active state changes. The active
 * parameter is the committed physical state, not the last request, so the
 * adapter can drive the real relay or fan output. Here it just stores it.
 */
static void cooldown_output_example_on_change(void* context, uint8_t active)
{
    (void)context;
    g_physical_output = active;
}

/* 250 ms cooldown; the callback mirrors active-state changes to the output. */
static const cooldown_output_config_t g_output_config =
{
    250u,
    cooldown_output_example_on_change,
    (void*)0
};

void cooldown_output_example_init(void)
{
    (void)cooldown_output_init(&g_output, &g_output_config);
}

/* Logical on/off command from the application, with the current ms time. */
void cooldown_output_example_request(uint8_t requested, uint32_t now_ms)
{
    cooldown_output_set_requested(&g_output, requested, now_ms);
}

/* Periodic step: advances the cooldown deadline. Must be called regularly. */
void cooldown_output_example_process(uint32_t now_ms)
{
    cooldown_output_process(&g_output, now_ms);
}
/*
 * File: libraries/actuator/cooldown_output/example.c
 */

#include "libraries/actuator/cooldown_output/cooldown_output.h"

static cooldown_output_t g_output;
static const cooldown_output_config_t g_output_config =
{
    250u,
    (cooldown_output_callback_t)0,
    (void*)0
};

void cooldown_output_example_init(void)
{
    (void)cooldown_output_init(&g_output, &g_output_config);
}

void cooldown_output_example_request(uint8_t requested, uint32_t now_ms)
{
    cooldown_output_set_requested(&g_output, requested, now_ms);
}

void cooldown_output_example_process(uint32_t now_ms)
{
    cooldown_output_process(&g_output, now_ms);
}

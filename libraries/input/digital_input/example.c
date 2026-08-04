/*
 * File: libraries/input/digital_input/example.c
 */

#include "libraries/input/digital_input/digital_input.h"

static digital_input_t g_input;
static const digital_input_config_t g_input_config =
{
    20u,
    0u,
    1u,
    1u
};

void digital_input_example_init(void)
{
    (void)digital_input_init(&g_input, &g_input_config);
}

void digital_input_example_update(uint8_t raw_level, uint32_t now_ms)
{
    digital_input_update(&g_input, raw_level, now_ms);
}

uint8_t digital_input_example_active(void)
{
    return digital_input_is_active(&g_input);
}

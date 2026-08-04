/*
 * File: libraries/input/digital_input/digital_input.c
 */

#include "libraries/input/digital_input/digital_input.h"

static uint8_t digital_input_normalize_level(uint8_t level)
{
    return (uint8_t)(level != 0u ? 1u : 0u);
}

static uint8_t digital_input_to_active(const digital_input_t* input, uint8_t raw_level)
{
    return (uint8_t)(raw_level == input->config.active_level ? 1u : 0u);
}

static void digital_input_commit_level(digital_input_t* input,
                                       uint8_t raw_level)
{
    uint8_t active_level;

    input->stable_raw_level = raw_level;
    active_level = digital_input_to_active(input, raw_level);

    if (active_level != input->stable_active)
    {
        input->stable_active = active_level;
        if (active_level != 0u)
        {
            input->rose_flag = 1u;
            if (input->config.latch_active != 0u)
            {
                input->latched = 1u;
            }
        }
        else
        {
            input->fell_flag = 1u;
        }
    }
}

drv_status_t digital_input_init(digital_input_t* input,
                                const digital_input_config_t* config)
{
    if ((input == (digital_input_t*)0) ||
        (config == (const digital_input_config_t*)0) ||
        (config->active_level > 1u) ||
        (config->initial_raw_level > 1u) ||
        (config->latch_active > 1u))
    {
        return DRV_STATUS_ERROR;
    }

    input->config = *config;
    input->initialized = 1u;
    input->raw_level = digital_input_normalize_level(config->initial_raw_level);
    input->stable_raw_level = input->raw_level;
    input->stable_active = digital_input_to_active(input, input->stable_raw_level);
    input->latched = 0u;
    input->rose_flag = 0u;
    input->fell_flag = 0u;
    input->pending_since_ms = 0UL;

    return DRV_STATUS_OK;
}

void digital_input_update(digital_input_t* input,
                          uint8_t raw_level,
                          uint32_t now_ms)
{
    uint8_t normalized_raw;

    if ((input == (digital_input_t*)0) || (input->initialized == 0u))
    {
        return;
    }

    normalized_raw = digital_input_normalize_level(raw_level);
    if (normalized_raw == input->stable_raw_level)
    {
        input->raw_level = normalized_raw;
        input->pending_since_ms = now_ms;
        return;
    }

    if (normalized_raw != input->raw_level)
    {
        input->raw_level = normalized_raw;
        input->pending_since_ms = now_ms;
    }

    if ((uint32_t)(now_ms - input->pending_since_ms) >= (uint32_t)input->config.debounce_ms)
    {
        digital_input_commit_level(input, normalized_raw);
        input->pending_since_ms = now_ms;
    }
}

uint8_t digital_input_is_active(const digital_input_t* input)
{
    if ((input == (const digital_input_t*)0) || (input->initialized == 0u))
    {
        return 0u;
    }

    return input->stable_active;
}

uint8_t digital_input_rose(digital_input_t* input)
{
    uint8_t flag;

    if ((input == (digital_input_t*)0) || (input->initialized == 0u))
    {
        return 0u;
    }

    flag = input->rose_flag;
    input->rose_flag = 0u;
    return flag;
}

uint8_t digital_input_fell(digital_input_t* input)
{
    uint8_t flag;

    if ((input == (digital_input_t*)0) || (input->initialized == 0u))
    {
        return 0u;
    }

    flag = input->fell_flag;
    input->fell_flag = 0u;
    return flag;
}

uint8_t digital_input_is_latched(const digital_input_t* input)
{
    if ((input == (const digital_input_t*)0) || (input->initialized == 0u))
    {
        return 0u;
    }

    return input->latched;
}

void digital_input_clear_latch(digital_input_t* input)
{
    if ((input == (digital_input_t*)0) || (input->initialized == 0u))
    {
        return;
    }

    input->latched = 0u;
}

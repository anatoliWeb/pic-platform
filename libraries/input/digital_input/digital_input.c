/*
 * File: libraries/input/digital_input/digital_input.c
 */

#include "libraries/input/digital_input/digital_input.h"

static uint8_t digital_input_normalize_level(uint8_t level)
{
    return (uint8_t)(level != 0u ? 1u : 0u);
}

/* Map a raw electrical level to the logical active state after active-level
 * polarity is applied. This is the only place polarity is interpreted. */
static uint8_t digital_input_to_active(const digital_input_t* input, uint8_t raw_level)
{
    return (uint8_t)(raw_level == input->config.active_level ? 1u : 0u);
}

/*
 * Commit a debounced raw level as the new stable state. Only a committed
 * transition produces a rose/fell edge flag; a brief glitch that returns to the
 * previous stable level never reaches here and therefore never creates an edge.
 */
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
        /* Back to the committed stable level: any pending transition is
         * cancelled, and the debounce timer restarts from now. */
        input->raw_level = normalized_raw;
        input->pending_since_ms = now_ms;
        return;
    }

    if (normalized_raw != input->raw_level)
    {
        /* A new candidate level: restart the debounce window. */
        input->raw_level = normalized_raw;
        input->pending_since_ms = now_ms;
    }

    if ((uint32_t)(now_ms - input->pending_since_ms) >= (uint32_t)input->config.debounce_ms)
    {
        /* The candidate held for the full debounce window: commit it as the
         * new stable state, then begin a fresh debounce period. */
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

    /* Consuming accessor: the caller gets the flag and clears it in one read,
     * so a missed edge is not reported twice to another consumer. */
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

    /* Clearing the sticky latch does not change the current stable state or
     * consume any pending edge flag; it only acknowledges the active event. */
    input->latched = 0u;
}

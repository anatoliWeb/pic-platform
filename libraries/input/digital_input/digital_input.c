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
 * Resolve the effective debounce threshold for the given transition direction.
 *
 * Priority order:
 *   1. If immediate_active is set and the new level is active, return 0
 *      (activation is instant regardless of directional fields).
 *   2. If both activate_debounce_ms and release_debounce_ms are 0, fall back
 *      to the symmetric debounce_ms (backward-compatible legacy mode).
 *   3. Otherwise use the direction-specific threshold.
 *
 * This means immediate_active works correctly even when both directional
 * fields are 0, which is the expected legacy config pattern for safety inputs.
 */
static uint16_t digital_input_resolve_debounce(const digital_input_t* input,
                                               uint8_t new_active)
{
    uint16_t act_ms;
    uint16_t rel_ms;

    /* Priority 1: immediate_active overrides everything for activation. */
    if ((new_active != 0u) && (input->config.immediate_active != 0u))
    {
        return 0u;
    }

    act_ms = input->config.activate_debounce_ms;
    rel_ms = input->config.release_debounce_ms;

    if ((act_ms == 0u) && (rel_ms == 0u))
    {
        /* Priority 2: both direction-specific thresholds are 0: fall back to
         * the symmetric debounce_ms for backward compatibility. */
        return input->config.debounce_ms;
    }

    /* Priority 3: use direction-specific threshold. */
    if (new_active != 0u)
    {
        return act_ms;
    }

    return rel_ms;
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
        (config->latch_active > 1u) ||
        (config->immediate_active > 1u))
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
    uint8_t new_active;
    uint16_t debounce_ms;

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

    new_active = digital_input_to_active(input, normalized_raw);
    debounce_ms = digital_input_resolve_debounce(input, new_active);

    if (debounce_ms == 0u)
    {
        /* Immediate commit: no debounce required for this direction. */
        if (normalized_raw != input->stable_raw_level)
        {
            digital_input_commit_level(input, normalized_raw);
        }
        return;
    }

    if (normalized_raw != input->raw_level)
    {
        /* A new candidate level: restart the debounce window. */
        input->raw_level = normalized_raw;
        input->pending_since_ms = now_ms;
    }

    if ((uint32_t)(now_ms - input->pending_since_ms) >= (uint32_t)debounce_ms)
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

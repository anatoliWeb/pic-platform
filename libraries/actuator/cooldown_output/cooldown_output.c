/*
 * File: libraries/actuator/cooldown_output/cooldown_output.c
 */

#include "libraries/actuator/cooldown_output/cooldown_output.h"

static uint8_t cooldown_output_normalize(uint8_t value)
{
    return (uint8_t)(value != 0u ? 1u : 0u);
}

static void cooldown_output_notify(cooldown_output_t* output)
{
    cooldown_output_callback_t callback;

    callback = output->config.callback;
    if ((callback != (cooldown_output_callback_t)0))
    {
        callback(output->config.callback_context, output->active);
    }
}

drv_status_t cooldown_output_init(cooldown_output_t* output,
                                  const cooldown_output_config_t* config)
{
    if ((output == (cooldown_output_t*)0) ||
        (config == (const cooldown_output_config_t*)0))
    {
        return DRV_STATUS_ERROR;
    }

    output->config = *config;
    output->initialized = 1u;
    output->requested = 0u;
    output->active = 0u;
    output->cooling_down = 0u;
    output->cooldown_end_ms = 0UL;

    return DRV_STATUS_OK;
}

void cooldown_output_set_requested(cooldown_output_t* output,
                                   uint8_t requested,
                                   uint32_t now_ms)
{
    if ((output == (cooldown_output_t*)0) || (output->initialized == 0u))
    {
        return;
    }

    requested = cooldown_output_normalize(requested);
    if (output->requested == requested)
    {
        if ((requested != 0u) && (output->cooling_down != 0u))
        {
            output->cooling_down = 0u;
        }
        return;
    }

    output->requested = requested;

    if (requested != 0u)
    {
        output->cooling_down = 0u;
        if (output->active == 0u)
        {
            output->active = 1u;
            cooldown_output_notify(output);
        }
        return;
    }

    if (output->active == 0u)
    {
        output->cooling_down = 0u;
        return;
    }

    if (output->config.cooldown_ms == 0UL)
    {
        output->active = 0u;
        output->cooling_down = 0u;
        cooldown_output_notify(output);
        return;
    }

    output->cooling_down = 1u;
    output->cooldown_end_ms = now_ms + output->config.cooldown_ms;
}

void cooldown_output_process(cooldown_output_t* output, uint32_t now_ms)
{
    if ((output == (cooldown_output_t*)0) || (output->initialized == 0u))
    {
        return;
    }

    if ((output->cooling_down != 0u) &&
        ((int32_t)(now_ms - output->cooldown_end_ms) >= 0))
    {
        output->cooling_down = 0u;
        if (output->active != 0u)
        {
            output->active = 0u;
            cooldown_output_notify(output);
        }
    }
}

uint8_t cooldown_output_is_active(const cooldown_output_t* output)
{
    if ((output == (const cooldown_output_t*)0) || (output->initialized == 0u))
    {
        return 0u;
    }

    return output->active;
}

uint8_t cooldown_output_is_cooling_down(const cooldown_output_t* output)
{
    if ((output == (const cooldown_output_t*)0) || (output->initialized == 0u))
    {
        return 0u;
    }

    return output->cooling_down;
}

uint32_t cooldown_output_get_remaining_ms(const cooldown_output_t* output,
                                          uint32_t now_ms)
{
    if ((output == (const cooldown_output_t*)0) ||
        (output->initialized == 0u) ||
        (output->cooling_down == 0u))
    {
        return 0UL;
    }

    if ((int32_t)(now_ms - output->cooldown_end_ms) >= 0)
    {
        return 0UL;
    }

    return (uint32_t)(output->cooldown_end_ms - now_ms);
}

void cooldown_output_force_off(cooldown_output_t* output)
{
    if ((output == (cooldown_output_t*)0) || (output->initialized == 0u))
    {
        return;
    }

    output->requested = 0u;
    output->cooling_down = 0u;

    if (output->active != 0u)
    {
        output->active = 0u;
        cooldown_output_notify(output);
    }
}

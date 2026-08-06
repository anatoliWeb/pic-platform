/*
 * File: libraries/actuator/cooldown_output/cooldown_output.c
 */

#include "libraries/actuator/cooldown_output/cooldown_output.h"

static uint8_t cooldown_output_normalize(uint8_t value)
{
    return (uint8_t)(value != 0u ? 1u : 0u);
}

/* Fire the optional callback. It reports the physical active state, not the
 * caller's requested state, and it is invoked only when active actually
 * changes, so a re-request or a no-op call cannot cause a duplicate notify. */
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
    output->cooldown_ms = config->cooldown_ms;
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
        /* Repeating the current request is a no-op for the output state. As a
         * special case, a repeated ON while cooling down cancels the pending
         * shutdown without emitting a duplicate callback. */
        if ((requested != 0u) && (output->cooling_down != 0u))
        {
            output->cooling_down = 0u;
        }
        return;
    }

    output->requested = requested;

    if (requested != 0u)
    {
        /* Request ON: cancel any pending cooldown and activate the output. */
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
        /* Already physically off; nothing to cool down. */
        output->cooling_down = 0u;
        return;
    }

    if (output->cooldown_ms == 0UL)
    {
        /* Zero cooldown means immediate off, with no delay phase. */
        output->active = 0u;
        output->cooling_down = 0u;
        cooldown_output_notify(output);
        return;
    }

    /* Request OFF with a delay: mark the cooling-down phase and compute the
     * monotonic deadline. The deadline comparison later is wrap-safe as long
     * as cooldown_ms stays below 2^31 ms (see the header). */
    output->cooling_down = 1u;
    output->cooldown_end_ms = now_ms + output->cooldown_ms;
}

void cooldown_output_set_duration_ms(cooldown_output_t* output,
                                     uint32_t duration_ms,
                                     uint32_t now_ms)
{
    if ((output == (cooldown_output_t*)0) || (output->initialized == 0u))
    {
        return;
    }

    output->cooldown_ms = duration_ms;

    /* If currently cooling down, recalculate the deadline from now with the
     * new duration. This makes the change predictable in all phases:
     *   - idle: stored for the next OFF request
     *   - cooling down: deadline updated immediately
     *   - after completion: stored for the next cycle
     */
    if (output->cooling_down != 0u)
    {
        if (duration_ms == 0UL)
        {
            /* Zero cooldown now: turn off immediately. */
            output->cooling_down = 0u;
            if (output->active != 0u)
            {
                output->active = 0u;
                cooldown_output_notify(output);
            }
        }
        else
        {
            output->cooldown_end_ms = now_ms + duration_ms;
        }
    }
}

void cooldown_output_cancel(cooldown_output_t* output)
{
    if ((output == (cooldown_output_t*)0) || (output->initialized == 0u))
    {
        return;
    }

    /* Cancel any pending cooldown while preserving the current active state
     * and the request. The output stays in its current physical state. */
    output->cooling_down = 0u;
}

void cooldown_output_process(cooldown_output_t* output, uint32_t now_ms)
{
    if ((output == (cooldown_output_t*)0) || (output->initialized == 0u))
    {
        return;
    }

    /* Wrap-safe deadline check: the signed difference is >= 0 exactly when
     * now_ms has reached or passed cooldown_end_ms, even across a 32-bit
     * timestamp wrap. When the delay elapses the output turns off. */
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
        /* Deadline already reached or passed (wrap-safe): no time left. */
        return 0UL;
    }

    /* Before the deadline the unsigned difference is the remaining cooldown
     * time in milliseconds. */
    return (uint32_t)(output->cooldown_end_ms - now_ms);
}

void cooldown_output_force_off(cooldown_output_t* output)
{
    if ((output == (cooldown_output_t*)0) || (output->initialized == 0u))
    {
        return;
    }

    /* Force-off bypasses the cooldown entirely: the request is cleared, any
     * pending cooling-down cancelled, and the output turns off immediately. */
    output->requested = 0u;
    output->cooling_down = 0u;

    if (output->active != 0u)
    {
        output->active = 0u;
        cooldown_output_notify(output);
    }
}

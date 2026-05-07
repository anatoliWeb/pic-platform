#include "libraries/system/relay/relay.h"

#include "drivers/gpio/gpio.h"

static void relay_apply_hw(relay_t* relay)
{
    uint8_t hw_on;

    if (relay == (relay_t*)0)
    {
        return;
    }

    /* Map logical ON/OFF to physical level depending on module polarity. */
    hw_on = relay->state;
    if (relay->inverted != 0u)
    {
        hw_on = (uint8_t)(hw_on == 0u ? 1u : 0u);
    }

    if (hw_on != 0u)
    {
        gpio_write_high(relay->port, relay->pin);
    }
    else
    {
        gpio_write_low(relay->port, relay->pin);
    }
}

uint8_t relay_init(relay_t* relay,
                   volatile uint8_t* port,
                   volatile uint8_t* tris,
                   uint8_t pin,
                   uint8_t inverted,
                   uint8_t safe_start_off)
{
    if ((relay == (relay_t*)0) || (port == (volatile uint8_t*)0) || (tris == (volatile uint8_t*)0))
    {
        return 0u;
    }

    relay->port = port;
    relay->tris = tris;
    relay->pin = pin;
    relay->inverted = (uint8_t)(inverted != 0u ? 1u : 0u);

    relay->state = (uint8_t)(safe_start_off != 0u ? 0u : 1u);
    relay->pulse_active = 0u;
    relay->pulse_end_ms = 0u;
    relay->delayed_off_active = 0u;
    relay->delayed_off_end_ms = 0u;

    gpio_set_output(relay->tris, relay->pin);
    relay_apply_hw(relay);

    return 1u;
}

void relay_on(relay_t* relay)
{
    if (relay == (relay_t*)0)
    {
        return;
    }

    relay->state = 1u;
    relay_apply_hw(relay);
}

void relay_off(relay_t* relay)
{
    if (relay == (relay_t*)0)
    {
        return;
    }

    relay->state = 0u;
    relay->pulse_active = 0u;
    relay->delayed_off_active = 0u;
    relay_apply_hw(relay);
}

void relay_toggle(relay_t* relay)
{
    if (relay == (relay_t*)0)
    {
        return;
    }

    relay->state = (uint8_t)(relay->state == 0u ? 1u : 0u);
    relay_apply_hw(relay);
}

void relay_set_state(relay_t* relay, uint8_t on)
{
    if (relay == (relay_t*)0)
    {
        return;
    }

    relay->state = (uint8_t)(on != 0u ? 1u : 0u);
    relay_apply_hw(relay);
}

uint8_t relay_get_state(const relay_t* relay)
{
    if (relay == (const relay_t*)0)
    {
        return 0u;
    }

    return relay->state;
}

void relay_set_inverted(relay_t* relay, uint8_t inverted)
{
    if (relay == (relay_t*)0)
    {
        return;
    }

    relay->inverted = (uint8_t)(inverted != 0u ? 1u : 0u);
    relay_apply_hw(relay);
}

void relay_pulse(relay_t* relay, uint32_t duration_ms, uint32_t now_ms)
{
    if (relay == (relay_t*)0)
    {
        return;
    }

    relay_on(relay);
    relay->pulse_active = 1u;
    relay->pulse_end_ms = (uint32_t)(now_ms + duration_ms);
}

void relay_delayed_off(relay_t* relay, uint32_t delay_ms, uint32_t now_ms)
{
    if (relay == (relay_t*)0)
    {
        return;
    }

    relay->delayed_off_active = 1u;
    relay->delayed_off_end_ms = (uint32_t)(now_ms + delay_ms);
}

void relay_update(relay_t* relay, uint32_t now_ms)
{
    if (relay == (relay_t*)0)
    {
        return;
    }

    if ((relay->pulse_active != 0u) && ((int32_t)(now_ms - relay->pulse_end_ms) >= 0))
    {
        relay->pulse_active = 0u;
        relay_off(relay);
    }

    if ((relay->delayed_off_active != 0u) && ((int32_t)(now_ms - relay->delayed_off_end_ms) >= 0))
    {
        relay->delayed_off_active = 0u;
        relay_off(relay);
    }
}

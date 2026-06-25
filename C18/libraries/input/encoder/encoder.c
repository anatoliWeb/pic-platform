/*
 * File: C18/libraries/input/encoder/encoder.c
 */

#include "libraries/input/encoder/encoder.h"
#include "core/bit_utils.h"

static uint8_t encoder_read_state(const encoder_t* enc)
{
    uint8_t a;
    uint8_t b;

    a = (uint8_t)READ_BIT((*enc->port), enc->pin_a);
    b = (uint8_t)READ_BIT((*enc->port), enc->pin_b);

    return (uint8_t)((a << 1u) | b);
}

static void encoder_apply_step(encoder_t* enc, int8_t step)
{
    if (step > 0)
    {
        if (enc->delta < 127)
        {
            enc->delta++;
        }
        if (enc->position < 32767)
        {
            enc->position++;
        }
    }
    else if (step < 0)
    {
        if (enc->delta > -127)
        {
            enc->delta--;
        }
        if (enc->position > -32768)
        {
            enc->position--;
        }
    }
}

void encoder_init(encoder_t* enc,
                  volatile uint8_t* port,
                  volatile uint8_t* tris,
                  uint8_t pin_a,
                  uint8_t pin_b,
                  volatile uint8_t* btn_port,
                  volatile uint8_t* btn_tris,
                  uint8_t btn_pin)
{
    if (enc == (encoder_t*)0)
    {
        return;
    }

    enc->port = port;
    enc->tris = tris;
    enc->pin_a = pin_a;
    enc->pin_b = pin_b;

    SET_BIT((*enc->tris), enc->pin_a);
    SET_BIT((*enc->tris), enc->pin_b);

    enc->last_state = encoder_read_state(enc);
    enc->delta = 0;
    enc->position = 0;

    button_init(&enc->button, btn_port, btn_tris, btn_pin);
}

void encoder_tick(encoder_t* enc)
{
    uint8_t current;
    uint8_t transition;

    if (enc == (encoder_t*)0)
    {
        return;
    }

    current = encoder_read_state(enc);

    if (current != enc->last_state)
    {
        transition = (uint8_t)((enc->last_state << 2u) | current);

        /* Valid quadrature transitions:
         * CW : 00->01->11->10->00
         * CCW: 00->10->11->01->00
         */
        switch (transition)
        {
            case 0x01u:
            case 0x07u:
            case 0x0Eu:
            case 0x08u:
                encoder_apply_step(enc, 1);
                break;

            case 0x02u:
            case 0x0Bu:
            case 0x0Du:
            case 0x04u:
                encoder_apply_step(enc, -1);
                break;

            default:
                /* Ignore bounce/invalid transitions. */
                break;
        }

        enc->last_state = current;
    }

    button_update(&enc->button);
}

void encoder_update(encoder_t* enc)
{
    encoder_tick(enc);
}

int8_t encoder_get_delta(encoder_t* enc)
{
    int8_t d;

    if (enc == (encoder_t*)0)
    {
        return 0;
    }

    d = enc->delta;

    if (d > 0)
    {
        enc->delta--;
        return 1;
    }

    if (d < 0)
    {
        enc->delta++;
        return -1;
    }

    return 0;
}

int16_t encoder_get_position(encoder_t* enc)
{
    if (enc == (encoder_t*)0)
    {
        return 0;
    }

    return enc->position;
}

void encoder_set_position(encoder_t* enc, int16_t value)
{
    if (enc == (encoder_t*)0)
    {
        return;
    }

    enc->position = value;
    enc->delta = 0;
}

uint8_t encoder_clicked(encoder_t* enc)
{
    if (enc == (encoder_t*)0)
    {
        return 0u;
    }

    return button_clicked(&enc->button);
}

uint8_t encoder_double_clicked(encoder_t* enc)
{
    if (enc == (encoder_t*)0)
    {
        return 0u;
    }

    return button_double_clicked(&enc->button);
}

uint8_t encoder_held(encoder_t* enc)
{
    if (enc == (encoder_t*)0)
    {
        return 0u;
    }

    return button_held(&enc->button);
}

uint8_t encoder_get_click_count(encoder_t* enc)
{
    if (enc == (encoder_t*)0)
    {
        return 0u;
    }

    return button_get_click_count(&enc->button);
}

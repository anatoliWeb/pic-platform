#include "drivers/encoder/encoder.h"

#if defined(DRV_COMPILER_C18)
    #include "../../C18/drivers/encoder/encoder.c"
#elif defined(DRV_COMPILER_XC8)
    #include "../../XC8/drivers/encoder/encoder.c"
#else

#include "core/bit_utils.h"

static uint8_t encoder_read_state(const encoder_t* enc)
{
    uint8_t a = (uint8_t)READ_BIT((*enc->port), enc->pin_a);
    uint8_t b = (uint8_t)READ_BIT((*enc->port), enc->pin_b);

    return (uint8_t)((a << 1u) | b);
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

    button_init(&enc->button, btn_port, btn_tris, btn_pin);
}

void encoder_update(encoder_t* enc)
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
                if (enc->delta < 127)
                {
                    enc->delta++;
                }
                break;

            case 0x02u:
            case 0x0Bu:
            case 0x0Du:
            case 0x04u:
                if (enc->delta > -127)
                {
                    enc->delta--;
                }
                break;

            default:
                /* Ignore bounce/invalid transitions. */
                break;
        }

        enc->last_state = current;
    }

    button_update(&enc->button);
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

#endif

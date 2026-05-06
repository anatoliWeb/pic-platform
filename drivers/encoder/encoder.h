#ifndef DRIVERS_ENCODER_ENCODER_H
#define DRIVERS_ENCODER_ENCODER_H

#include "core/compiler.h"
#include "core/types.h"
#include "drivers/button/button.h"

typedef struct
{
    volatile uint8_t* port;
    volatile uint8_t* tris;

    uint8_t pin_a;
    uint8_t pin_b;

    uint8_t last_state;
    int8_t  delta;

    /* button (reuse existing driver) */
    button_t button;

} encoder_t;

void encoder_init(encoder_t* enc,
                  volatile uint8_t* port,
                  volatile uint8_t* tris,
                  uint8_t pin_a,
                  uint8_t pin_b,
                  volatile uint8_t* btn_port,
                  volatile uint8_t* btn_tris,
                  uint8_t btn_pin);

void encoder_update(encoder_t* enc);

int8_t encoder_get_delta(encoder_t* enc);

#endif /* DRIVERS_ENCODER_ENCODER_H */

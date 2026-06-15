#ifndef DRIVERS_ENCODER_ENCODER_H
#define DRIVERS_ENCODER_ENCODER_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/input/button/button.h"

typedef struct
{
    volatile uint8_t* port;
    volatile uint8_t* tris;

    uint8_t pin_a;
    uint8_t pin_b;

    uint8_t last_state;
    int8_t  delta;
    int16_t position;

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

void encoder_tick(encoder_t* enc);
void encoder_update(encoder_t* enc);

int8_t encoder_get_delta(encoder_t* enc);
int16_t encoder_get_position(encoder_t* enc);
void encoder_set_position(encoder_t* enc, int16_t value);

uint8_t encoder_clicked(encoder_t* enc);
uint8_t encoder_double_clicked(encoder_t* enc);
uint8_t encoder_held(encoder_t* enc);
uint8_t encoder_get_click_count(encoder_t* enc);

#endif /* DRIVERS_ENCODER_ENCODER_H */

#ifndef DRIVERS_BUTTON_BUTTON_H
#define DRIVERS_BUTTON_BUTTON_H

#include "core/compiler.h"
#include "core/types.h"

typedef struct
{
    volatile uint8_t* port;
    volatile uint8_t* tris;
    uint8_t pin;

    uint8_t state;
    uint8_t last_state;

    uint32_t last_change_time;
    uint32_t press_time;

    uint8_t click_flag;
    uint8_t hold_flag;

} button_t;

void button_init(button_t* btn,
                 volatile uint8_t* port,
                 volatile uint8_t* tris,
                 uint8_t pin);

void button_update(button_t* btn);

uint8_t button_is_clicked(button_t* btn);
uint8_t button_is_held(button_t* btn);

#endif /* DRIVERS_BUTTON_BUTTON_H */

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
    uint32_t last_click_time;
    uint32_t last_hold_repeat_time;

    uint8_t pressed_flag;
    uint8_t released_flag;
    uint8_t click_flag;
    uint8_t double_click_flag;
    uint8_t hold_flag;
    uint8_t hold_repeat_flag;
    uint8_t click_count;

} button_t;

void button_init(button_t* btn,
                 volatile uint8_t* port,
                 volatile uint8_t* tris,
                 uint8_t pin);

void button_update(button_t* btn);

uint8_t button_pressed(button_t* btn);
uint8_t button_released(button_t* btn);
uint8_t button_clicked(button_t* btn);
uint8_t button_double_clicked(button_t* btn);
uint8_t button_held(button_t* btn);
uint8_t button_hold_repeated(button_t* btn);
uint8_t button_get_click_count(button_t* btn);

uint8_t button_is_clicked(button_t* btn);
uint8_t button_is_held(button_t* btn);

#endif /* DRIVERS_BUTTON_BUTTON_H */

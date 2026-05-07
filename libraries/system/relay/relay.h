#ifndef LIBRARIES_SYSTEM_RELAY_H
#define LIBRARIES_SYSTEM_RELAY_H

#include "core/compiler.h"
#include "core/types.h"

typedef struct
{
    volatile uint8_t* port;
    volatile uint8_t* tris;
    uint8_t pin;

    uint8_t inverted;
    uint8_t state;

    uint8_t pulse_active;
    uint32_t pulse_end_ms;

    uint8_t delayed_off_active;
    uint32_t delayed_off_end_ms;
} relay_t;

uint8_t relay_init(relay_t* relay,
                   volatile uint8_t* port,
                   volatile uint8_t* tris,
                   uint8_t pin,
                   uint8_t inverted,
                   uint8_t safe_start_off);

void relay_on(relay_t* relay);
void relay_off(relay_t* relay);
void relay_toggle(relay_t* relay);
void relay_set_state(relay_t* relay, uint8_t on);
uint8_t relay_get_state(const relay_t* relay);

void relay_set_inverted(relay_t* relay, uint8_t inverted);

void relay_pulse(relay_t* relay, uint32_t duration_ms, uint32_t now_ms);
void relay_delayed_off(relay_t* relay, uint32_t delay_ms, uint32_t now_ms);

void relay_update(relay_t* relay, uint32_t now_ms);

#endif /* LIBRARIES_SYSTEM_RELAY_H */

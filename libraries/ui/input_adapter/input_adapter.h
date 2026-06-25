/*
 * File: libraries/ui/input_adapter/input_adapter.h
 */

#ifndef LIBRARIES_UI_INPUT_ADAPTER_H
#define LIBRARIES_UI_INPUT_ADAPTER_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/input/button/button.h"
#include "libraries/input/encoder/encoder.h"
#include "libraries/input/keypad_matrix/keypad_matrix.h"

#define INPUT_ADAPTER_QUEUE_SIZE 8u

typedef enum
{
    INPUT_EVENT_NONE = 0,
    INPUT_EVENT_UP,
    INPUT_EVENT_DOWN,
    INPUT_EVENT_LEFT,
    INPUT_EVENT_RIGHT,
    INPUT_EVENT_SELECT,
    INPUT_EVENT_BACK,
    INPUT_EVENT_HOLD
} input_event_t;

typedef struct
{
    button_t* button;
    encoder_t* encoder;
    keypad_matrix_t* keypad;

    char key_up;
    char key_down;
    char key_left;
    char key_right;
    char key_select;
    char key_back;

    char last_key;

    input_event_t queue[INPUT_ADAPTER_QUEUE_SIZE];
    uint8_t head;
    uint8_t tail;
} input_adapter_t;

uint8_t input_adapter_init(input_adapter_t* adapter,
                           button_t* button,
                           encoder_t* encoder,
                           keypad_matrix_t* keypad);

void input_adapter_update(input_adapter_t* adapter);
input_event_t input_adapter_get_event(input_adapter_t* adapter);

void input_adapter_clear(input_adapter_t* adapter);
input_event_t input_adapter_peek(const input_adapter_t* adapter);

#endif /* LIBRARIES_UI_INPUT_ADAPTER_H */

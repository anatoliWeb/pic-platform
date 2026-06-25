/*
 * File: libraries/ui/input_adapter/input_adapter.c
 */

#include "libraries/ui/input_adapter/input_adapter.h"

static void input_adapter_push(input_adapter_t* adapter, input_event_t ev)
{
    uint8_t next;

    if ((adapter == (input_adapter_t*)0) || (ev == INPUT_EVENT_NONE))
    {
        return;
    }

    next = (uint8_t)((adapter->head + 1u) % INPUT_ADAPTER_QUEUE_SIZE);

    if (next == adapter->tail)
    {
        /* Queue full, drop oldest event. */
        adapter->tail = (uint8_t)((adapter->tail + 1u) % INPUT_ADAPTER_QUEUE_SIZE);
    }

    adapter->queue[adapter->head] = ev;
    adapter->head = next;
}

static input_event_t input_adapter_translate_key(input_adapter_t* adapter, char key)
{
    if (adapter == (input_adapter_t*)0)
    {
        return INPUT_EVENT_NONE;
    }

    if (key == adapter->key_up) return INPUT_EVENT_UP;
    if (key == adapter->key_down) return INPUT_EVENT_DOWN;
    if (key == adapter->key_left) return INPUT_EVENT_LEFT;
    if (key == adapter->key_right) return INPUT_EVENT_RIGHT;
    if (key == adapter->key_select) return INPUT_EVENT_SELECT;
    if (key == adapter->key_back) return INPUT_EVENT_BACK;

    return INPUT_EVENT_NONE;
}

uint8_t input_adapter_init(input_adapter_t* adapter,
                           button_t* button,
                           encoder_t* encoder,
                           keypad_matrix_t* keypad)
{
    uint8_t i;

    if (adapter == (input_adapter_t*)0)
    {
        return 0u;
    }

    adapter->button = button;
    adapter->encoder = encoder;
    adapter->keypad = keypad;

    /* Default keypad mapping. */
    adapter->key_up = '2';
    adapter->key_down = '8';
    adapter->key_left = '4';
    adapter->key_right = '6';
    adapter->key_select = '#';
    adapter->key_back = '*';

    adapter->last_key = '\0';

    adapter->head = 0u;
    adapter->tail = 0u;

    for (i = 0u; i < INPUT_ADAPTER_QUEUE_SIZE; i++)
    {
        adapter->queue[i] = INPUT_EVENT_NONE;
    }

    return 1u;
}

void input_adapter_update(input_adapter_t* adapter)
{
    int8_t delta;
    char key;
    input_event_t ev;

    if (adapter == (input_adapter_t*)0)
    {
        return;
    }

    /* Button source. */
    if (adapter->button != (button_t*)0)
    {
        button_update(adapter->button);

        if (button_is_clicked(adapter->button) != 0u)
        {
            input_adapter_push(adapter, INPUT_EVENT_SELECT);
        }

        if (button_is_held(adapter->button) != 0u)
        {
            input_adapter_push(adapter, INPUT_EVENT_HOLD);
        }
    }

    /* Encoder source. */
    if (adapter->encoder != (encoder_t*)0)
    {
        encoder_update(adapter->encoder);
        delta = encoder_get_delta(adapter->encoder);

        if (delta > 0)
        {
            input_adapter_push(adapter, INPUT_EVENT_DOWN);
        }
        else if (delta < 0)
        {
            input_adapter_push(adapter, INPUT_EVENT_UP);
        }

        if (button_is_clicked(&adapter->encoder->button) != 0u)
        {
            input_adapter_push(adapter, INPUT_EVENT_SELECT);
        }
        if (button_is_held(&adapter->encoder->button) != 0u)
        {
            input_adapter_push(adapter, INPUT_EVENT_HOLD);
        }
    }

    /* Keypad source. */
    if (adapter->keypad != (keypad_matrix_t*)0)
    {
        keypad_matrix_scan(adapter->keypad);
        key = keypad_matrix_get_key(adapter->keypad);

        if ((key != '\0') && (key != adapter->last_key))
        {
            ev = input_adapter_translate_key(adapter, key);
            input_adapter_push(adapter, ev);
        }

        adapter->last_key = key;
    }
}

input_event_t input_adapter_get_event(input_adapter_t* adapter)
{
    input_event_t ev;

    if ((adapter == (input_adapter_t*)0) || (adapter->head == adapter->tail))
    {
        return INPUT_EVENT_NONE;
    }

    ev = adapter->queue[adapter->tail];
    adapter->tail = (uint8_t)((adapter->tail + 1u) % INPUT_ADAPTER_QUEUE_SIZE);

    return ev;
}

void input_adapter_clear(input_adapter_t* adapter)
{
    uint8_t i;

    if (adapter == (input_adapter_t*)0)
    {
        return;
    }

    adapter->head = 0u;
    adapter->tail = 0u;

    for (i = 0u; i < INPUT_ADAPTER_QUEUE_SIZE; i++)
    {
        adapter->queue[i] = INPUT_EVENT_NONE;
    }
}

input_event_t input_adapter_peek(const input_adapter_t* adapter)
{
    if ((adapter == (const input_adapter_t*)0) || (adapter->head == adapter->tail))
    {
        return INPUT_EVENT_NONE;
    }

    return adapter->queue[adapter->tail];
}

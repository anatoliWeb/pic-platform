/*
 * File: C18/libraries/input/button/button.c
 */

#include "libraries/input/button/button.h"
#include "core/bit_utils.h"
#include "drivers/timers/tick/tick.h"

#define BUTTON_DEBOUNCE_MS       30u
#define BUTTON_CLICK_TIMEOUT_MS  400u
#define BUTTON_HOLD_MS           700u
#define BUTTON_HOLD_REPEAT_MS    300u

static uint8_t button_read_raw(const button_t* btn)
{
    return (uint8_t)READ_BIT((*btn->port), btn->pin);
}

static void button_clear_transition_flags(button_t* btn)
{
    btn->pressed_flag = 0u;
    btn->released_flag = 0u;
    btn->click_flag = 0u;
    btn->double_click_flag = 0u;
    btn->hold_flag = 0u;
    btn->hold_repeat_flag = 0u;
}

void button_init(button_t* btn,
                 volatile uint8_t* port,
                 volatile uint8_t* tris,
                 uint8_t pin)
{
    uint8_t raw;

    if (btn == (button_t*)0)
    {
        return;
    }

    btn->port = port;
    btn->tris = tris;
    btn->pin = pin;

    SET_BIT((*btn->tris), btn->pin);

    raw = button_read_raw(btn);
    btn->state = raw;
    btn->last_state = raw;

    btn->last_change_time = tick_get();
    btn->press_time = 0u;
    btn->last_click_time = 0u;
    btn->last_hold_repeat_time = 0u;

    btn->click_count = 0u;
    button_clear_transition_flags(btn);
}

void button_update(button_t* btn)
{
    uint8_t raw;
    uint32_t now;

    if (btn == (button_t*)0)
    {
        return;
    }

    now = tick_get();
    raw = button_read_raw(btn);

    if (raw != btn->last_state)
    {
        btn->last_state = raw;
        btn->last_change_time = now;
    }

    if (((now - btn->last_change_time) >= BUTTON_DEBOUNCE_MS) && (btn->state != raw))
    {
        btn->state = raw;

        if (btn->state == 0u)
        {
            /* Active-low button logic: 0 = pressed, 1 = released. */
            btn->pressed_flag = 1u;
            btn->press_time = now;
            btn->hold_flag = 0u;
            btn->hold_repeat_flag = 0u;
            btn->last_hold_repeat_time = now;
        }
        else
        {
            btn->released_flag = 1u;

            if ((now - btn->press_time) < BUTTON_HOLD_MS)
            {
                btn->click_flag = 1u;

                if (btn->click_count < 255u)
                {
                    btn->click_count++;
                }

                if ((btn->last_click_time != 0u) &&
                    ((now - btn->last_click_time) <= BUTTON_CLICK_TIMEOUT_MS))
                {
                    btn->double_click_flag = 1u;
                }

                btn->last_click_time = now;
            }
            else
            {
                btn->last_click_time = 0u;
            }
        }
    }

    if (btn->state == 0u)
    {
        if ((btn->hold_flag == 0u) && ((now - btn->press_time) >= BUTTON_HOLD_MS))
        {
            btn->hold_flag = 1u;
            btn->last_hold_repeat_time = now;
        }
        else if ((btn->hold_flag != 0u) &&
                 ((now - btn->last_hold_repeat_time) >= BUTTON_HOLD_REPEAT_MS))
        {
            btn->hold_repeat_flag = 1u;
            btn->last_hold_repeat_time = now;
        }
    }
}

uint8_t button_pressed(button_t* btn)
{
    uint8_t flag;

    if (btn == (button_t*)0)
    {
        return 0u;
    }

    flag = btn->pressed_flag;
    btn->pressed_flag = 0u;
    return flag;
}

uint8_t button_released(button_t* btn)
{
    uint8_t flag;

    if (btn == (button_t*)0)
    {
        return 0u;
    }

    flag = btn->released_flag;
    btn->released_flag = 0u;
    return flag;
}

uint8_t button_clicked(button_t* btn)
{
    uint8_t flag;

    if (btn == (button_t*)0)
    {
        return 0u;
    }

    flag = btn->click_flag;
    btn->click_flag = 0u;
    return flag;
}

uint8_t button_double_clicked(button_t* btn)
{
    uint8_t flag;

    if (btn == (button_t*)0)
    {
        return 0u;
    }

    flag = btn->double_click_flag;
    btn->double_click_flag = 0u;
    return flag;
}

uint8_t button_held(button_t* btn)
{
    uint8_t flag;

    if (btn == (button_t*)0)
    {
        return 0u;
    }

    flag = btn->hold_flag;
    btn->hold_flag = 0u;
    return flag;
}

uint8_t button_hold_repeated(button_t* btn)
{
    uint8_t flag;

    if (btn == (button_t*)0)
    {
        return 0u;
    }

    flag = btn->hold_repeat_flag;
    btn->hold_repeat_flag = 0u;
    return flag;
}

uint8_t button_get_click_count(button_t* btn)
{
    uint8_t count;

    if (btn == (button_t*)0)
    {
        return 0u;
    }

    count = btn->click_count;
    btn->click_count = 0u;
    return count;
}

uint8_t button_is_clicked(button_t* btn)
{
    return button_clicked(btn);
}

uint8_t button_is_held(button_t* btn)
{
    return button_held(btn);
}

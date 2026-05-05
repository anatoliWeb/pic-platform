#include "drivers/button/button.h"

#if defined(DRV_COMPILER_C18)
    #include "../../C18/drivers/button/button.c"
#elif defined(DRV_COMPILER_XC8)
    #include "../../XC8/drivers/button/button.c"
#else

#include "core/bit_utils.h"
#include "drivers/tick/tick.h"

#define BUTTON_DEBOUNCE_MS 30u
#define BUTTON_HOLD_MS     500u

static uint8_t button_read_raw(const button_t* btn)
{
    return (uint8_t)READ_BIT((*btn->port), btn->pin);
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

    btn->click_flag = 0u;
    btn->hold_flag = 0u;
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

        /* Active-low button logic: 0=pressed, 1=released. */
        if (btn->state == 0u)
        {
            btn->press_time = now;
            btn->hold_flag = 0u;
        }
        else
        {
            if ((now - btn->press_time) < BUTTON_HOLD_MS)
            {
                btn->click_flag = 1u;
            }
        }
    }

    if ((btn->state == 0u) && (btn->hold_flag == 0u) && ((now - btn->press_time) >= BUTTON_HOLD_MS))
    {
        btn->hold_flag = 1u;
        /* TODO: double-click detection can be added later. */
    }
}

uint8_t button_is_clicked(button_t* btn)
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

uint8_t button_is_held(button_t* btn)
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

#endif

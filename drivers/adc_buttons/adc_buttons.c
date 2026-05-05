#include "drivers/adc_buttons/adc_buttons.h"

#if defined(DRV_COMPILER_C18)
    #include "../../C18/drivers/adc_buttons/adc_buttons.c"
#elif defined(DRV_COMPILER_XC8)
    #include "../../XC8/drivers/adc_buttons/adc_buttons.c"
#else

#include "drivers/adc/adc.h"
#include "drivers/tick/tick.h"

#define ADC_BUTTONS_DEBOUNCE_MS 30u
#define ADC_BUTTONS_HOLD_MS     500u

static uint8_t adc_buttons_decode(const adc_buttons_t* btns, uint16_t value)
{
    uint8_t i;

    for (i = 0u; i < btns->table_size; i++)
    {
        const adc_button_range_t* r = &btns->table[i];
        if ((value >= r->min) && (value <= r->max))
        {
            return r->id;
        }
    }

    return ADC_BUTTONS_NO_BUTTON;
}

void adc_buttons_init(adc_buttons_t* btns,
                      uint8_t channel,
                      const adc_button_range_t* table,
                      uint8_t table_size)
{
    uint16_t value;

    if (btns == (adc_buttons_t*)0)
    {
        return;
    }

    btns->channel = channel;
    btns->table = table;
    btns->table_size = (table_size > ADC_BUTTONS_MAX) ? ADC_BUTTONS_MAX : table_size;

    value = adc_read(channel);
    btns->current_button = (table != (const adc_button_range_t*)0) ? adc_buttons_decode(btns, value) : ADC_BUTTONS_NO_BUTTON;
    btns->last_button = btns->current_button;

    btns->last_change_time = tick_get();
    btns->press_time = 0u;

    btns->click_flag = 0u;
    btns->hold_flag = 0u;
}

void adc_buttons_update(adc_buttons_t* btns)
{
    uint16_t value;
    uint8_t decoded;
    uint32_t now;

    if ((btns == (adc_buttons_t*)0) || (btns->table == (const adc_button_range_t*)0) || (btns->table_size == 0u))
    {
        return;
    }

    now = tick_get();
    value = adc_read(btns->channel);
    decoded = adc_buttons_decode(btns, value);

    if (decoded != btns->last_button)
    {
        btns->last_button = decoded;
        btns->last_change_time = now;
    }

    if (((now - btns->last_change_time) >= ADC_BUTTONS_DEBOUNCE_MS) && (btns->current_button != decoded))
    {
        btns->current_button = decoded;

        if (btns->current_button != ADC_BUTTONS_NO_BUTTON)
        {
            btns->press_time = now;
            btns->hold_flag = 0u;
        }
        else
        {
            if ((now - btns->press_time) < ADC_BUTTONS_HOLD_MS)
            {
                btns->click_flag = 1u;
            }
        }
    }

    if ((btns->current_button != ADC_BUTTONS_NO_BUTTON) &&
        (btns->hold_flag == 0u) &&
        ((now - btns->press_time) >= ADC_BUTTONS_HOLD_MS))
    {
        btns->hold_flag = 1u;
    }
}

uint8_t adc_buttons_get(adc_buttons_t* btns)
{
    if (btns == (adc_buttons_t*)0)
    {
        return ADC_BUTTONS_NO_BUTTON;
    }

    return btns->current_button;
}

uint8_t adc_buttons_is_clicked(adc_buttons_t* btns)
{
    uint8_t flag;

    if (btns == (adc_buttons_t*)0)
    {
        return 0u;
    }

    flag = btns->click_flag;
    btns->click_flag = 0u;
    return flag;
}

uint8_t adc_buttons_is_held(adc_buttons_t* btns)
{
    uint8_t flag;

    if (btns == (adc_buttons_t*)0)
    {
        return 0u;
    }

    flag = btns->hold_flag;
    btns->hold_flag = 0u;
    return flag;
}

#endif

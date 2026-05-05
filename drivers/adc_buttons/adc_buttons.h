#ifndef DRIVERS_ADC_BUTTONS_ADC_BUTTONS_H
#define DRIVERS_ADC_BUTTONS_ADC_BUTTONS_H

#include "core/compiler.h"
#include "core/types.h"

#define ADC_BUTTONS_MAX 8u
#define ADC_BUTTONS_NO_BUTTON 0u

typedef struct
{
    uint16_t min;
    uint16_t max;
    uint8_t  id;
} adc_button_range_t;

typedef struct
{
    uint8_t channel;

    const adc_button_range_t* table;
    uint8_t table_size;

    uint8_t current_button;
    uint8_t last_button;

    uint32_t last_change_time;
    uint32_t press_time;

    uint8_t click_flag;
    uint8_t hold_flag;

} adc_buttons_t;

void adc_buttons_init(adc_buttons_t* btns,
                      uint8_t channel,
                      const adc_button_range_t* table,
                      uint8_t table_size);

void adc_buttons_update(adc_buttons_t* btns);

uint8_t adc_buttons_get(adc_buttons_t* btns);

uint8_t adc_buttons_is_clicked(adc_buttons_t* btns);
uint8_t adc_buttons_is_held(adc_buttons_t* btns);

#endif /* DRIVERS_ADC_BUTTONS_ADC_BUTTONS_H */

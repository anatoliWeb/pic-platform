/*
 * File: XC8/drivers/analog/adc/adc.c
 */

#include "drivers/analog/adc/adc.h"
#include "core/delay.h"

#define ADC_DEFAULT_VREF_MV 5000u
#define ADC_BUTTON_NONE     0u

void adc_init(void)
{
    ADCON0 = 0x01u;
    ADCON1 = 0x0Eu;
#ifdef ADCON2
    ADCON2 = 0xA9u;
#endif
}

uint16_t adc_read(uint8_t channel)
{
    ADCON0 &= 0xC3u;
    ADCON0 |= (uint8_t)((channel & 0x0Fu) << 2u);

    DRV_DELAY_US(10);
    ADCON0bits.GO = 1u;
    while (ADCON0bits.GO != 0u)
    {
    }

    return (uint16_t)(((uint16_t)ADRESH << 8u) | ADRESL);
}

uint16_t adc_read_avg(uint8_t channel, uint8_t samples)
{
    uint32_t sum = 0u;
    uint8_t i;

    if (samples == 0u)
    {
        return adc_read(channel);
    }

    for (i = 0u; i < samples; i++)
    {
        sum += adc_read(channel);
    }

    return (uint16_t)(sum / samples);
}

void adc_read_multiple(uint8_t* channels, uint16_t* results, uint8_t count)
{
    uint8_t i;

    if ((channels == (uint8_t*)0) || (results == (uint16_t*)0))
    {
        return;
    }

    for (i = 0u; i < count; i++)
    {
        results[i] = adc_read(channels[i]);
    }
}

uint16_t adc_to_millivolts(uint16_t adc_value, uint16_t vref_mv)
{
    uint32_t mv = ((uint32_t)adc_value * (uint32_t)vref_mv) / 1023u;
    return (uint16_t)mv;
}

uint16_t adc_read_voltage(uint8_t channel)
{
    return adc_to_millivolts(adc_read(channel), ADC_DEFAULT_VREF_MV);
}

int16_t adc_read_thermistor(uint8_t channel)
{
    uint16_t raw = adc_read_avg(channel, 8u);
    int16_t temp = (int16_t)(((int32_t)raw * 140) / 1023) - 20;
    return temp;
}

uint8_t adc_read_button(uint8_t channel)
{
    uint16_t raw = adc_read_avg(channel, 4u);

    if (raw <= 100u)
    {
        return 1u;
    }
    if (raw <= 300u)
    {
        return 2u;
    }
    if (raw <= 500u)
    {
        return 3u;
    }
    if (raw <= 700u)
    {
        return 4u;
    }

    return ADC_BUTTON_NONE;
}

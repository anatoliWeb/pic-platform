#include "drivers/ds18b20/ds18b20.h"

#if defined(DRV_COMPILER_C18)
    #include "../../C18/drivers/ds18b20/ds18b20.c"
#elif defined(DRV_COMPILER_XC8)
    #include "../../XC8/drivers/ds18b20/ds18b20.c"
#else

#include "drivers/onewire/onewire.h"
#include "core/delay.h"

static const uint8_t DS18B20_CMD_CONVERT_T = 0x44u;
static const uint8_t DS18B20_CMD_READ_SCRATCHPAD = 0xBEu;

static uint8_t ds18b20_crc8(const uint8_t* data, uint8_t len)
{
    uint8_t i;
    uint8_t j;
    uint8_t crc = 0u;

    for (i = 0u; i < len; i++)
    {
        uint8_t in_byte = data[i];
        for (j = 0u; j < 8u; j++)
        {
            uint8_t mix = (uint8_t)((crc ^ in_byte) & 0x01u);
            crc >>= 1u;
            if (mix != 0u)
            {
                crc ^= 0x8Cu;
            }
            in_byte >>= 1u;
        }
    }

    return crc;
}

static void ds18b20_select_rom(uint8_t* rom)
{
    if (rom != (uint8_t*)0)
    {
        onewire_match_rom(rom);
    }
    else
    {
        onewire_skip_rom();
    }
}

uint8_t ds18b20_start_conversion(uint8_t* rom)
{
    if (onewire_reset() == 0u)
    {
        return 0u;
    }

    ds18b20_select_rom(rom);
    onewire_write_byte(DS18B20_CMD_CONVERT_T);

    return 1u;
}

uint8_t ds18b20_read_scratchpad(uint8_t* rom, uint8_t* data)
{
    uint8_t i;

    if (data == (uint8_t*)0)
    {
        return 0u;
    }

    if (onewire_reset() == 0u)
    {
        return 0u;
    }

    ds18b20_select_rom(rom);
    onewire_write_byte(DS18B20_CMD_READ_SCRATCHPAD);

    for (i = 0u; i < 9u; i++)
    {
        data[i] = onewire_read_byte();
    }

    if (ds18b20_crc8(data, 8u) != data[8])
    {
        return 0u;
    }

    return 1u;
}

int16_t ds18b20_get_temperature_raw(uint8_t* rom)
{
    uint8_t scratchpad[9];

    if (ds18b20_start_conversion(rom) == 0u)
    {
        return 0;
    }

    DRV_DELAY_MS(750u);

    if (ds18b20_read_scratchpad(rom, scratchpad) == 0u)
    {
        return 0;
    }

    return (int16_t)(((uint16_t)scratchpad[1] << 8u) | scratchpad[0]);
}

int16_t ds18b20_get_temperature_celsius_x10(uint8_t* rom)
{
    int16_t raw = ds18b20_get_temperature_raw(rom);
    return (int16_t)((raw * 10) / 16);
}

#endif

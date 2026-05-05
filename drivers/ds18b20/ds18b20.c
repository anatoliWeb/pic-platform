#include "drivers/ds18b20/ds18b20.h"

#if defined(DRV_COMPILER_C18)
    #include "../../C18/drivers/ds18b20/ds18b20.c"
#elif defined(DRV_COMPILER_XC8)
    #include "../../XC8/drivers/ds18b20/ds18b20.c"
#else

#include "drivers/onewire/onewire.h"
#include "core/delay.h"
#include "drivers/crc/crc.h"

static const uint8_t DS18B20_CMD_CONVERT_T = 0x44u;
static const uint8_t DS18B20_CMD_READ_SCRATCHPAD = 0xBEu;

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

    if (crc8_dallas(data, 8u) != data[8])
    {
        return 0u;
    }

    return 1u;
}

uint8_t ds18b20_read_temperature_raw(uint8_t* rom, int16_t* out_raw)
{
    uint8_t scratchpad[9];

    if (out_raw == (int16_t*)0)
    {
        return 0u;
    }

    if (ds18b20_start_conversion(rom) == 0u)
    {
        return 0u;
    }

    DRV_DELAY_MS(750u);

    if (ds18b20_read_scratchpad(rom, scratchpad) == 0u)
    {
        return 0u;
    }

    *out_raw = (int16_t)(((uint16_t)scratchpad[1] << 8u) | scratchpad[0]);
    return 1u;
}

uint8_t ds18b20_read_temperature_celsius(uint8_t* rom, int16_t* out_temp_x10)
{
    int16_t raw;

    if (out_temp_x10 == (int16_t*)0)
    {
        return 0u;
    }

    if (ds18b20_read_temperature_raw(rom, &raw) == 0u)
    {
        return 0u;
    }

    *out_temp_x10 = (int16_t)((raw * 10) / 16);
    return 1u;
}

int16_t ds18b20_get_temperature_raw(uint8_t* rom)
{
    int16_t raw = 0;
    (void)ds18b20_read_temperature_raw(rom, &raw);
    return raw;
}

int16_t ds18b20_get_temperature_celsius_x10(uint8_t* rom)
{
    int16_t temp_x10 = 0;
    (void)ds18b20_read_temperature_celsius(rom, &temp_x10);
    return temp_x10;
}

#endif

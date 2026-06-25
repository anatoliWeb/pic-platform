/*
 * File: C18/libraries/sensors/ds18b20/ds18b20.c
 */

#include "libraries/sensors/ds18b20/ds18b20.h"
#include "drivers/communication/onewire/onewire.h"
#include "core/delay.h"
#include "core/crc/crc.h"

static const uint8_t DS18B20_FAMILY_CODE = 0x28u;
static const uint8_t DS18B20_CMD_CONVERT_T = 0x44u;
static const uint8_t DS18B20_CMD_READ_SCRATCHPAD = 0xBEu;

static void ds18b20_copy_rom(uint8_t* dst, const uint8_t* src)
{
    uint8_t i;

    if ((dst == (uint8_t*)0) || (src == (const uint8_t*)0))
    {
        return;
    }

    for (i = 0u; i < 8u; i++)
    {
        dst[i] = src[i];
    }
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

static uint8_t ds18b20_transaction_begin(uint8_t* rom)
{
    if (onewire_reset() == 0u)
    {
        return 0u;
    }

    ds18b20_select_rom(rom);
    return 1u;
}

uint8_t ds18b20_is_valid_family(const uint8_t* rom)
{
    if (rom == (const uint8_t*)0)
    {
        return 0u;
    }

    return (rom[0] == DS18B20_FAMILY_CODE) ? 1u : 0u;
}

uint8_t ds18b20_is_valid_rom(const uint8_t* rom)
{
    if ((rom == (const uint8_t*)0) || (ds18b20_is_valid_family(rom) == 0u))
    {
        return 0u;
    }

    return (onewire_crc8(rom, 8u) == 0u) ? 1u : 0u;
}

uint8_t ds18b20_search(uint8_t (*roms)[8], uint8_t max_devices)
{
    uint8_t found;
    uint8_t kept;
    uint8_t i;
    uint8_t j;

    if ((roms == (uint8_t (*)[8])0) || (max_devices == 0u))
    {
        return 0u;
    }

    found = onewire_search_rom(roms, max_devices);
    kept = 0u;

    for (i = 0u; i < found; i++)
    {
        if (ds18b20_is_valid_rom(roms[i]) != 0u)
        {
            if (kept != i)
            {
                for (j = 0u; j < 8u; j++)
                {
                    roms[kept][j] = roms[i][j];
                }
            }

            kept++;
        }
    }

    return kept;
}

uint8_t ds18b20_find_first(uint8_t* rom)
{
    uint8_t roms[1][8];

    if (rom == (uint8_t*)0)
    {
        return 0u;
    }

    if (ds18b20_search(roms, 1u) == 0u)
    {
        return 0u;
    }

    ds18b20_copy_rom(rom, roms[0]);
    return 1u;
}

uint8_t ds18b20_start_conversion(uint8_t* rom)
{
    if ((rom != (uint8_t*)0) && (ds18b20_is_valid_rom(rom) == 0u))
    {
        return 0u;
    }

    if (ds18b20_transaction_begin(rom) == 0u)
    {
        return 0u;
    }

    onewire_write_byte(DS18B20_CMD_CONVERT_T);
    return 1u;
}

uint8_t ds18b20_start_conversion_skip_rom(void)
{
    return ds18b20_start_conversion((uint8_t*)0);
}

uint8_t ds18b20_read_scratchpad(uint8_t* rom, uint8_t* data)
{
    uint8_t i;

    if (data == (uint8_t*)0)
    {
        return 0u;
    }

    if ((rom != (uint8_t*)0) && (ds18b20_is_valid_rom(rom) == 0u))
    {
        return 0u;
    }

    if (ds18b20_transaction_begin(rom) == 0u)
    {
        return 0u;
    }

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

uint8_t ds18b20_read_scratchpad_skip_rom(uint8_t* data)
{
    return ds18b20_read_scratchpad((uint8_t*)0, data);
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

uint8_t ds18b20_read_temperature_raw_skip_rom(int16_t* out_raw)
{
    return ds18b20_read_temperature_raw((uint8_t*)0, out_raw);
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

uint8_t ds18b20_read_temperature_celsius_skip_rom(int16_t* out_temp_x10)
{
    return ds18b20_read_temperature_celsius((uint8_t*)0, out_temp_x10);
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

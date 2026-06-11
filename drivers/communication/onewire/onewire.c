#include "drivers/communication/onewire/onewire.h"

#if defined(DRV_COMPILER_C18)
    #include "../../../C18/drivers/communication/onewire/onewire.c"
#elif defined(DRV_COMPILER_XC8)
    #include "../../../XC8/drivers/communication/onewire/onewire.c"
#else

#include "core/delay.h"
#include "core/bit_utils.h"
#include "core/interrupts.h"

static volatile uint8_t* g_onewire_port = (volatile uint8_t*)0;
static volatile uint8_t* g_onewire_tris = (volatile uint8_t*)0;
static uint8_t g_onewire_pin = 0u;

static const uint8_t OW_CMD_READ_ROM = 0x33u;
static const uint8_t OW_CMD_MATCH_ROM = 0x55u;
static const uint8_t OW_CMD_SKIP_ROM = 0xCCu;

static void onewire_set_low(void)
{
    CLEAR_BIT((*g_onewire_port), g_onewire_pin);
    CLEAR_BIT((*g_onewire_tris), g_onewire_pin);
}

static void onewire_release_line(void)
{
    SET_BIT((*g_onewire_tris), g_onewire_pin);
}

static uint8_t onewire_read_line(void)
{
    return (uint8_t)READ_BIT((*g_onewire_port), g_onewire_pin);
}

DRV_INLINE void ow_delay_us(uint16_t us)
{
    DRV_DELAY_US(us);
}

void onewire_init(volatile uint8_t* port, volatile uint8_t* tris, uint8_t pin)
{
    g_onewire_port = port;
    g_onewire_tris = tris;
    g_onewire_pin = pin;

    onewire_release_line();
}

uint8_t onewire_reset(void)
{
    uint8_t present;

    DRV_INT_DISABLE();

    onewire_set_low();
    DRV_DELAY_US(480u);

    onewire_release_line();
    DRV_DELAY_US(70u);

    present = (onewire_read_line() == 0u) ? 1u : 0u;

    DRV_DELAY_US(410u);

    DRV_INT_ENABLE();

    return present;
}

void onewire_write_bit(uint8_t bit)
{
    DRV_INT_DISABLE();

    onewire_set_low();
    if ((bit & 0x01u) != 0u)
    {
        ow_delay_us(6u);
        onewire_release_line();
        ow_delay_us(64u);
    }
    else
    {
        ow_delay_us(60u);
        onewire_release_line();
        ow_delay_us(10u);
    }

    DRV_INT_ENABLE();
}

uint8_t onewire_read_bit(void)
{
    uint8_t bit_value;

    DRV_INT_DISABLE();

    onewire_set_low();
    ow_delay_us(6u);
    onewire_release_line();
    ow_delay_us(9u);

    bit_value = (onewire_read_line() != 0u) ? 1u : 0u;
    ow_delay_us(55u);

    DRV_INT_ENABLE();

    return bit_value;
}

void onewire_write_byte(uint8_t byte)
{
    uint8_t i;

    for (i = 0u; i < 8u; i++)
    {
        onewire_write_bit(byte & 0x01u);
        byte >>= 1u;
    }
}

uint8_t onewire_read_byte(void)
{
    uint8_t i;
    uint8_t value = 0u;

    for (i = 0u; i < 8u; i++)
    {
        value >>= 1u;
        if (onewire_read_bit() != 0u)
        {
            value |= 0x80u;
        }
    }

    return value;
}

void onewire_skip_rom(void)
{
    onewire_write_byte(OW_CMD_SKIP_ROM);
}

void onewire_match_rom(uint8_t* rom)
{
    uint8_t i;

    if (rom == (uint8_t*)0)
    {
        return;
    }

    onewire_write_byte(OW_CMD_MATCH_ROM);
    for (i = 0u; i < 8u; i++)
    {
        onewire_write_byte(rom[i]);
    }
}

void onewire_read_rom(uint8_t* rom)
{
    uint8_t i;

    if (rom == (uint8_t*)0)
    {
        return;
    }

    onewire_write_byte(OW_CMD_READ_ROM);
    for (i = 0u; i < 8u; i++)
    {
        rom[i] = onewire_read_byte();
    }
}

uint8_t onewire_search_rom(uint8_t (*roms)[8], uint8_t max_devices)
{
    uint8_t found_devices = 0u;
    uint8_t search_rom[8] = {0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u};
    uint8_t last_discrepancy = 0u;
    uint8_t last_device_flag = 0u;
    uint8_t last_family_discrepancy = 0u;
    uint8_t search_result = 0u;
    uint8_t id_bit_number;
    uint8_t last_zero;
    uint8_t rom_byte_number;
    uint8_t rom_byte_mask;
    uint8_t id_bit;
    uint8_t cmp_id_bit;
    uint8_t search_direction;

    if ((roms == (uint8_t(*)[8])0) || (max_devices == 0u))
    {
        return 0u;
    }

    while ((last_device_flag == 0u) && (found_devices < max_devices))
    {
        last_zero = 0u;
        id_bit_number = 1u;
        rom_byte_number = 0u;
        rom_byte_mask = 1u;

        search_result = onewire_reset();
        if (search_result == 0u)
        {
            break;
        }

        onewire_write_byte(0xF0u);

        while (rom_byte_number < 8u)
        {
            id_bit = onewire_read_bit();
            cmp_id_bit = onewire_read_bit();

            if ((id_bit != 0u) && (cmp_id_bit != 0u))
            {
                search_result = 0u;
                break;
            }

            if (id_bit != cmp_id_bit)
            {
                search_direction = id_bit;
            }
            else
            {
                if (id_bit_number < last_discrepancy)
                {
                    search_direction = (uint8_t)((search_rom[rom_byte_number] & rom_byte_mask) != 0u);
                }
                else
                {
                    search_direction = (uint8_t)((id_bit_number == last_discrepancy) ? 1u : 0u);
                }

                if (search_direction == 0u)
                {
                    last_zero = id_bit_number;
                    if (last_zero < 9u)
                    {
                        last_family_discrepancy = last_zero;
                    }
                }
            }

            if (search_direction != 0u)
            {
                search_rom[rom_byte_number] |= rom_byte_mask;
            }
            else
            {
                search_rom[rom_byte_number] &= (uint8_t)~rom_byte_mask;
            }

            onewire_write_bit(search_direction);

            id_bit_number++;
            rom_byte_mask <<= 1u;
            if (rom_byte_mask == 0u)
            {
                rom_byte_number++;
                rom_byte_mask = 1u;
            }

            if (id_bit_number > 64u)
            {
                break;
            }
        }

        if ((search_result == 0u) || (id_bit_number <= 64u))
        {
            break;
        }

        if (onewire_crc8(search_rom, 8u) == 0u)
        {
            roms[found_devices][0u] = search_rom[0u];
            roms[found_devices][1u] = search_rom[1u];
            roms[found_devices][2u] = search_rom[2u];
            roms[found_devices][3u] = search_rom[3u];
            roms[found_devices][4u] = search_rom[4u];
            roms[found_devices][5u] = search_rom[5u];
            roms[found_devices][6u] = search_rom[6u];
            roms[found_devices][7u] = search_rom[7u];
            found_devices++;
        }

        last_discrepancy = last_zero;
        if (last_discrepancy == 0u)
        {
            last_device_flag = 1u;
        }

        if (last_family_discrepancy == 0u)
        {
            /* No family-level discrepancy was tracked for this scan. */
        }
    }

    return found_devices;
}

uint8_t onewire_crc8(const uint8_t* data, uint8_t len)
{
    uint8_t i;
    uint8_t j;
    uint8_t crc = 0u;

    if (data == (const uint8_t*)0)
    {
        return 0u;
    }

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

#endif

/*
 * File: XC8/core/crc/crc.c
 */

#include "core/crc/crc.h"

uint8_t crc8_dallas(const uint8_t* data, uint16_t len)
{
    uint8_t crc = 0u;
    uint16_t i;
    uint8_t j;

    if (data == (const uint8_t*)0)
    {
        return 0u;
    }

    for (i = 0u; i < len; i++)
    {
        crc ^= data[i];
        for (j = 0u; j < 8u; j++)
        {
            if ((crc & 0x01u) != 0u)
            {
                crc = (uint8_t)((crc >> 1u) ^ 0x8Cu);
            }
            else
            {
                crc >>= 1u;
            }
        }
    }

    return crc;
}

uint16_t crc16_modbus(const uint8_t* data, uint16_t len)
{
    uint16_t crc = 0xFFFFu;
    uint16_t i;
    uint8_t j;

    if (data == (const uint8_t*)0)
    {
        return 0u;
    }

    for (i = 0u; i < len; i++)
    {
        crc ^= data[i];

        for (j = 0u; j < 8u; j++)
        {
            if ((crc & 0x0001u) != 0u)
            {
                crc = (uint16_t)((crc >> 1u) ^ 0xA001u);
            }
            else
            {
                crc >>= 1u;
            }
        }
    }

    return crc;
}

uint8_t checksum8(const uint8_t* data, uint16_t len)
{
    uint16_t i;
    uint8_t sum = 0u;

    if (data == (const uint8_t*)0)
    {
        return 0u;
    }

    for (i = 0u; i < len; i++)
    {
        sum = (uint8_t)(sum + data[i]);
    }

    return sum;
}

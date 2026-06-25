/*
 * File: libraries/sensors/bmp/bmp.h
 */

#ifndef LIBRARIES_SENSORS_BMP_H
#define LIBRARIES_SENSORS_BMP_H

#include "core/compiler.h"
#include "core/types.h"

typedef enum
{
    BMP_SENSOR_BMP180 = 0,
    BMP_SENSOR_BMP280
} bmp_sensor_type_t;

typedef enum
{
    BMP_STATUS_OK = 0,
    BMP_STATUS_ERROR,
    BMP_STATUS_I2C_ERROR,
    BMP_STATUS_INVALID_CHIP_ID,
    BMP_STATUS_INVALID_CALIB,
    BMP_STATUS_TIMEOUT
} bmp_status_t;

typedef struct
{
    bmp_sensor_type_t sensor_type;
    uint8_t i2c_address;
    uint8_t oversampling;
} bmp_config_t;

typedef struct
{
    int32_t temperature_x10;
    uint32_t pressure_pa;
    uint8_t valid;
} bmp_data_t;

typedef struct
{
    bmp_config_t config;
    bmp_data_t data;

    uint8_t initialized;

    int32_t t_fine;

    /* BMP180 calibration */
    int16_t ac1;
    int16_t ac2;
    int16_t ac3;
    uint16_t ac4;
    uint16_t ac5;
    uint16_t ac6;
    int16_t b1;
    int16_t b2;
    int16_t mb;
    int16_t mc;
    int16_t md;

    /* BMP280 calibration */
    uint16_t dig_t1;
    int16_t dig_t2;
    int16_t dig_t3;
    uint16_t dig_p1;
    int16_t dig_p2;
    int16_t dig_p3;
    int16_t dig_p4;
    int16_t dig_p5;
    int16_t dig_p6;
    int16_t dig_p7;
    int16_t dig_p8;
    int16_t dig_p9;
} bmp_t;

uint8_t bmp_init(bmp_t* bmp, const bmp_config_t* config);
bmp_status_t bmp_read(bmp_t* bmp);

int32_t bmp_get_temperature(const bmp_t* bmp);
uint32_t bmp_get_pressure(const bmp_t* bmp);
int32_t bmp_get_altitude(const bmp_t* bmp, uint32_t sea_level_pa);
uint8_t bmp_is_valid(const bmp_t* bmp);

#endif /* LIBRARIES_SENSORS_BMP_H */

#include "libraries/sensors/bmp/bmp.h"

#include "core/delay.h"
#include "drivers/communication/i2c/i2c.h"

typedef signed long long bmp_i64_t;
typedef unsigned long long bmp_u64_t;

static uint8_t bmp_i2c_read_u8(uint8_t addr, uint8_t reg, uint8_t* value)
{
    return i2c_read_register(addr, reg, value);
}

static uint8_t bmp_i2c_write_u8(uint8_t addr, uint8_t reg, uint8_t value)
{
    return i2c_write_register(addr, reg, value);
}

static uint8_t bmp_i2c_read_buf(uint8_t addr, uint8_t reg, uint8_t* buf, uint8_t len)
{
    uint8_t i;

    if (buf == (uint8_t*)0)
    {
        return 1u;
    }

    i2c_start();
    if (i2c_write_byte((uint8_t)(addr << 1u)) != 0u) { i2c_stop(); return 1u; }
    if (i2c_write_byte(reg) != 0u) { i2c_stop(); return 1u; }

    i2c_restart();
    if (i2c_write_byte((uint8_t)((addr << 1u) | 0x01u)) != 0u) { i2c_stop(); return 1u; }

    for (i = 0u; i < len; i++)
    {
        buf[i] = i2c_read_byte((uint8_t)(i < (uint8_t)(len - 1u) ? 1u : 0u));
    }

    i2c_stop();
    return 0u;
}

static uint16_t bmp_u16_le(const uint8_t* b)
{
    return (uint16_t)(((uint16_t)b[1] << 8u) | b[0]);
}

static int16_t bmp_s16_le(const uint8_t* b)
{
    return (int16_t)bmp_u16_le(b);
}

static uint16_t bmp_u16_be(const uint8_t* b)
{
    return (uint16_t)(((uint16_t)b[0] << 8u) | b[1]);
}

static int16_t bmp_s16_be(const uint8_t* b)
{
    return (int16_t)bmp_u16_be(b);
}

static bmp_status_t bmp180_read_calibration(bmp_t* bmp)
{
    uint8_t c[22];

    if (bmp_i2c_read_buf(bmp->config.i2c_address, 0xAAu, c, 22u) != 0u)
    {
        return BMP_STATUS_I2C_ERROR;
    }

    bmp->ac1 = bmp_s16_be(&c[0]);
    bmp->ac2 = bmp_s16_be(&c[2]);
    bmp->ac3 = bmp_s16_be(&c[4]);
    bmp->ac4 = bmp_u16_be(&c[6]);
    bmp->ac5 = bmp_u16_be(&c[8]);
    bmp->ac6 = bmp_u16_be(&c[10]);
    bmp->b1 = bmp_s16_be(&c[12]);
    bmp->b2 = bmp_s16_be(&c[14]);
    bmp->mb = bmp_s16_be(&c[16]);
    bmp->mc = bmp_s16_be(&c[18]);
    bmp->md = bmp_s16_be(&c[20]);

    if ((bmp->ac4 == 0u) || (bmp->ac5 == 0u) || (bmp->ac6 == 0u))
    {
        return BMP_STATUS_INVALID_CALIB;
    }

    return BMP_STATUS_OK;
}

static bmp_status_t bmp280_read_calibration(bmp_t* bmp)
{
    uint8_t c[24];

    if (bmp_i2c_read_buf(bmp->config.i2c_address, 0x88u, c, 24u) != 0u)
    {
        return BMP_STATUS_I2C_ERROR;
    }

    bmp->dig_t1 = bmp_u16_le(&c[0]);
    bmp->dig_t2 = bmp_s16_le(&c[2]);
    bmp->dig_t3 = bmp_s16_le(&c[4]);
    bmp->dig_p1 = bmp_u16_le(&c[6]);
    bmp->dig_p2 = bmp_s16_le(&c[8]);
    bmp->dig_p3 = bmp_s16_le(&c[10]);
    bmp->dig_p4 = bmp_s16_le(&c[12]);
    bmp->dig_p5 = bmp_s16_le(&c[14]);
    bmp->dig_p6 = bmp_s16_le(&c[16]);
    bmp->dig_p7 = bmp_s16_le(&c[18]);
    bmp->dig_p8 = bmp_s16_le(&c[20]);
    bmp->dig_p9 = bmp_s16_le(&c[22]);

    if ((bmp->dig_t1 == 0u) || (bmp->dig_p1 == 0u))
    {
        return BMP_STATUS_INVALID_CALIB;
    }

    return BMP_STATUS_OK;
}

static bmp_status_t bmp180_read_raw(bmp_t* bmp, int32_t* ut, int32_t* up)
{
    uint8_t d[3];
    uint8_t oss;
    uint8_t cmd;

    oss = (uint8_t)(bmp->config.oversampling & 0x03u);

    if (bmp_i2c_write_u8(bmp->config.i2c_address, 0xF4u, 0x2Eu) != 0u)
    {
        return BMP_STATUS_I2C_ERROR;
    }
    DRV_DELAY_MS(5u);

    if (bmp_i2c_read_buf(bmp->config.i2c_address, 0xF6u, d, 2u) != 0u)
    {
        return BMP_STATUS_I2C_ERROR;
    }
    *ut = (int32_t)(((uint16_t)d[0] << 8u) | d[1]);

    cmd = (uint8_t)(0x34u + (uint8_t)(oss << 6u));
    if (bmp_i2c_write_u8(bmp->config.i2c_address, 0xF4u, cmd) != 0u)
    {
        return BMP_STATUS_I2C_ERROR;
    }

    if (oss == 0u) DRV_DELAY_MS(5u);
    else if (oss == 1u) DRV_DELAY_MS(8u);
    else if (oss == 2u) DRV_DELAY_MS(14u);
    else DRV_DELAY_MS(26u);

    if (bmp_i2c_read_buf(bmp->config.i2c_address, 0xF6u, d, 3u) != 0u)
    {
        return BMP_STATUS_I2C_ERROR;
    }

    *up = (int32_t)((((uint32_t)d[0] << 16u) | ((uint16_t)d[1] << 8u) | d[2]) >> (8u - oss));
    return BMP_STATUS_OK;
}

static bmp_status_t bmp180_compensate(bmp_t* bmp, int32_t ut, int32_t up)
{
    int32_t x1;
    int32_t x2;
    int32_t x3;
    int32_t b3;
    uint32_t b4;
    uint32_t b7;
    int32_t p;
    uint8_t oss;

    oss = (uint8_t)(bmp->config.oversampling & 0x03u);

    x1 = (int32_t)(((ut - (int32_t)bmp->ac6) * (int32_t)bmp->ac5) >> 15u);
    x2 = (int32_t)(((int32_t)bmp->mc << 11u) / (x1 + (int32_t)bmp->md));
    bmp->t_fine = x1 + x2;
    bmp->data.temperature_x10 = (int32_t)((bmp->t_fine + 8) >> 4u);

    x1 = (int32_t)(((bmp->t_fine >> 2u) * (bmp->t_fine >> 2u)) >> 12u);
    x1 = (int32_t)((x1 * (int32_t)bmp->b2) >> 11u);
    x2 = (int32_t)(((int32_t)bmp->ac2 * bmp->t_fine) >> 11u);
    x3 = x1 + x2;
    b3 = (int32_t)(((((int32_t)bmp->ac1 * 4 + x3) << oss) + 2) >> 2u);

    x1 = (int32_t)(((int32_t)bmp->ac3 * bmp->t_fine) >> 13u);
    x2 = (int32_t)(((int32_t)bmp->b1 * (((bmp->t_fine >> 2u) * (bmp->t_fine >> 2u)) >> 12u)) >> 16u);
    x3 = (int32_t)((x1 + x2 + 2) >> 2u);
    b4 = (uint32_t)(((uint32_t)bmp->ac4 * (uint32_t)(x3 + 32768)) >> 15u);
    b7 = (uint32_t)(((uint32_t)up - (uint32_t)b3) * (uint32_t)(50000u >> oss));

    if (b7 < 0x80000000u)
    {
        p = (int32_t)((b7 << 1u) / b4);
    }
    else
    {
        p = (int32_t)((b7 / b4) << 1u);
    }

    x1 = (int32_t)((p >> 8u) * (p >> 8u));
    x1 = (int32_t)((x1 * 3038) >> 16u);
    x2 = (int32_t)((-7357 * p) >> 16u);
    p = (int32_t)(p + ((x1 + x2 + 3791) >> 4u));

    if (p < 0)
    {
        return BMP_STATUS_INVALID_FRAME;
    }

    bmp->data.pressure_pa = (uint32_t)p;
    bmp->data.valid = 1u;
    return BMP_STATUS_OK;
}

static bmp_status_t bmp280_read_raw(bmp_t* bmp, int32_t* adc_t, int32_t* adc_p)
{
    uint8_t d[6];
    uint8_t ctrl_meas;

    ctrl_meas = (uint8_t)(((uint8_t)1u << 5u) | ((uint8_t)1u << 2u) | 0x01u);
    if (bmp_i2c_write_u8(bmp->config.i2c_address, 0xF4u, ctrl_meas) != 0u)
    {
        return BMP_STATUS_I2C_ERROR;
    }
    DRV_DELAY_MS(10u);

    if (bmp_i2c_read_buf(bmp->config.i2c_address, 0xF7u, d, 6u) != 0u)
    {
        return BMP_STATUS_I2C_ERROR;
    }

    *adc_p = (int32_t)((((uint32_t)d[0] << 12u) | ((uint16_t)d[1] << 4u) | ((uint16_t)d[2] >> 4u)));
    *adc_t = (int32_t)((((uint32_t)d[3] << 12u) | ((uint16_t)d[4] << 4u) | ((uint16_t)d[5] >> 4u)));

    return BMP_STATUS_OK;
}

static bmp_status_t bmp280_compensate(bmp_t* bmp, int32_t adc_t, int32_t adc_p)
{
    int32_t var1;
    int32_t var2;
    bmp_i64_t pvar1;
    bmp_i64_t pvar2;
    bmp_i64_t p;

    var1 = (int32_t)((((adc_t >> 3u) - ((int32_t)bmp->dig_t1 << 1u)) * (int32_t)bmp->dig_t2) >> 11u);
    var2 = (int32_t)((((((adc_t >> 4u) - (int32_t)bmp->dig_t1) * ((adc_t >> 4u) - (int32_t)bmp->dig_t1)) >> 12u) *
                      (int32_t)bmp->dig_t3) >> 14u);
    bmp->t_fine = var1 + var2;
    bmp->data.temperature_x10 = (int32_t)((bmp->t_fine * 5 + 128) >> 8u);

    pvar1 = ((bmp_i64_t)bmp->t_fine) - 128000;
    pvar2 = pvar1 * pvar1 * (bmp_i64_t)bmp->dig_p6;
    pvar2 = pvar2 + ((pvar1 * (bmp_i64_t)bmp->dig_p5) << 17u);
    pvar2 = pvar2 + (((bmp_i64_t)bmp->dig_p4) << 35u);
    pvar1 = ((pvar1 * pvar1 * (bmp_i64_t)bmp->dig_p3) >> 8u) + ((pvar1 * (bmp_i64_t)bmp->dig_p2) << 12u);
    pvar1 = (((((bmp_i64_t)1) << 47u) + pvar1) * (bmp_i64_t)bmp->dig_p1) >> 33u;

    if (pvar1 == 0)
    {
        return BMP_STATUS_INVALID_FRAME;
    }

    p = 1048576 - (bmp_i64_t)adc_p;
    p = (((p << 31u) - pvar2) * 3125) / pvar1;
    pvar1 = (((bmp_i64_t)bmp->dig_p9) * (p >> 13u) * (p >> 13u)) >> 25u;
    pvar2 = (((bmp_i64_t)bmp->dig_p8) * p) >> 19u;
    p = ((p + pvar1 + pvar2) >> 8u) + (((bmp_i64_t)bmp->dig_p7) << 4u);

    if (p < 0)
    {
        return BMP_STATUS_INVALID_FRAME;
    }

    bmp->data.pressure_pa = (uint32_t)(p >> 8u);
    bmp->data.valid = 1u;
    return BMP_STATUS_OK;
}

uint8_t bmp_init(bmp_t* bmp, const bmp_config_t* config)
{
    uint8_t chip_id;
    bmp_status_t st;

    if ((bmp == (bmp_t*)0) || (config == (const bmp_config_t*)0))
    {
        return 0u;
    }

    bmp->config = *config;
    bmp->data.temperature_x10 = 0;
    bmp->data.pressure_pa = 0u;
    bmp->data.valid = 0u;
    bmp->initialized = 0u;
    bmp->t_fine = 0;

    if (bmp_i2c_read_u8(config->i2c_address, 0xD0u, &chip_id) != 0u)
    {
        return 0u;
    }

    if ((config->sensor_type == BMP_SENSOR_BMP180) && (chip_id != 0x55u))
    {
        return 0u;
    }
    if ((config->sensor_type == BMP_SENSOR_BMP280) && (chip_id != 0x58u))
    {
        return 0u;
    }

    if (config->sensor_type == BMP_SENSOR_BMP180)
    {
        st = bmp180_read_calibration(bmp);
    }
    else
    {
        st = bmp280_read_calibration(bmp);
    }

    if (st != BMP_STATUS_OK)
    {
        return 0u;
    }

    bmp->initialized = 1u;
    return 1u;
}

bmp_status_t bmp_read(bmp_t* bmp)
{
    bmp_status_t st;
    int32_t ut;
    int32_t up;

    if ((bmp == (bmp_t*)0) || (bmp->initialized == 0u))
    {
        return BMP_STATUS_ERROR;
    }

    bmp->data.valid = 0u;

    if (bmp->config.sensor_type == BMP_SENSOR_BMP180)
    {
        st = bmp180_read_raw(bmp, &ut, &up);
        if (st != BMP_STATUS_OK)
        {
            return st;
        }
        return bmp180_compensate(bmp, ut, up);
    }

    st = bmp280_read_raw(bmp, &ut, &up);
    if (st != BMP_STATUS_OK)
    {
        return st;
    }
    return bmp280_compensate(bmp, ut, up);
}

int32_t bmp_get_temperature(const bmp_t* bmp)
{
    if ((bmp == (const bmp_t*)0) || (bmp->data.valid == 0u))
    {
        return 0;
    }
    return bmp->data.temperature_x10;
}

uint32_t bmp_get_pressure(const bmp_t* bmp)
{
    if ((bmp == (const bmp_t*)0) || (bmp->data.valid == 0u))
    {
        return 0u;
    }
    return bmp->data.pressure_pa;
}

int32_t bmp_get_altitude(const bmp_t* bmp, uint32_t sea_level_pa)
{
    int32_t delta;

    if ((bmp == (const bmp_t*)0) || (bmp->data.valid == 0u))
    {
        return 0;
    }

    if (sea_level_pa == 0u)
    {
        sea_level_pa = 101325u;
    }

    /* Lightweight linear approximation near sea level (meters). */
    delta = (int32_t)sea_level_pa - (int32_t)bmp->data.pressure_pa;
    return (int32_t)(delta / 12);
}

uint8_t bmp_is_valid(const bmp_t* bmp)
{
    if (bmp == (const bmp_t*)0)
    {
        return 0u;
    }
    return bmp->data.valid;
}

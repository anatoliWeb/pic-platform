# BMP Sensor Library (BMP180 + BMP280)

## Description

`bmp` is a unified pressure/temperature sensor library for BMP180 and BMP280.
Sensor type is selected via config, while API remains the same.

## Public API

- `bmp_init()`
- `bmp_read()`
- `bmp_get_temperature()` (`x10` deg C)
- `bmp_get_pressure()` (Pa)
- `bmp_get_altitude()` (optional helper, meters, approximate)
- `bmp_is_valid()`

## Supported Sensor Types

- `BMP_SENSOR_BMP180`
- `BMP_SENSOR_BMP280`

## Configuration

- `sensor_type`
- `i2c_address` (for example `0x77` or `0x76`)
- `oversampling` (used by BMP180 path)

## Calibration and Compensation

### BMP180
- Reads calibration words from `0xAA..0xBF`.
- Uses datasheet-style integer compensation for temperature and pressure.
- Oversampling `OSS` from config (`0..3`).

### BMP280
- Reads calibration block from `0x88..0x9F`.
- Computes temperature with `t_fine`.
- Computes pressure using fixed-point compensation chain.

## Error Handling

`bmp_read()` and init logic detect:

- I2C communication failure
- invalid chip ID
- invalid calibration values
- invalid frame/compensation edge cases

## BMP180 vs BMP280 Notes

- BMP180: older part, different register map and conversion flow.
- BMP280: newer part, separate calibration set and different compensation math.
- Unified library hides these differences behind one API.

## Wiring and I2C Notes

- Use pull-up resistors on SDA/SCL.
- Common addresses:
  - BMP180: usually `0x77`
  - BMP280: `0x76` or `0x77` (depends on SDO strap)

## Dependencies

- `core/*`
- `drivers/communication/i2c`

## Limitations

- Altitude helper uses a lightweight linear approximation (not full barometric equation).
- For high-precision altitude applications, use external filtering and full formula in application layer.

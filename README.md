# PIC PLATFORM - Universal Drivers (C18 + XC8)

Бібліотека універсальних драйверів для PIC (фокус: PIC18).
Це база драйверів, не application-проєкт.

## Підтримувані компілятори

- MPLAB C18
- MPLAB XC8

## Архітектура

- Спільний API для кожного драйвера
- Universal entrypoint у `drivers/`
- Compiler-specific реалізації у `C18/` і `XC8/`
- Fallback реалізація, якщо override не вибрано

## I2C / MSSP Driver

I2C реалізовано через MSSP у режимі Master.

Підтримка:

- `i2c_init(clock_hz)`
- `i2c_start()` / `i2c_restart()` / `i2c_stop()`
- `i2c_write_byte()` / `i2c_read_byte(ack)`
- `i2c_write_register()` / `i2c_read_register()`
- `i2c_device_ready()`
- `i2c_scan()`

Особливості:

- ACK/NACK handling через `ACKSTAT`, `ACKDT`, `ACKEN`
- scan адрес `0x08..0x77`
- без `malloc`
- без прив'язки до конкретного I2C-пристрою

### I2C Driver Architecture

- `drivers/i2c/i2c.c` — universal entrypoint + fallback
- `C18/drivers/i2c/i2c.c` — C18-specific implementation
- `XC8/drivers/i2c/i2c.c` — XC8-specific implementation

### I2C Examples

- `drivers/i2c/example.c`:
: scan шини
: sensor-like register read
: LCD-like byte writes (без повного LCD driver)
- `C18/examples/i2c_example.c`
- `XC8/examples/i2c_example.c`

## Інші драйвери

У репозиторії також підготовлені базові драйвери GPIO, UART, UART debug, RS485, ADC, PWM, Timer, EEPROM, SPI.

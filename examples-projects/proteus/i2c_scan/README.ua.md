# i2c_scan Proteus simulation

## Статус
Потрібне ручне налаштування Proteus.

## Firmware

XC8 HEX:

`../../hex/xc8/i2c_scan/i2c_scan.hex`

C18 HEX поки недоступний для цього проєкту.

## MPLAB-проєкти

- `../../xc8/communication/i2c/scan.X`
- `../../c18/i2c_scan.X`

## Підключення до PIC18F452

| Сигнал | Ніжка PIC18F452 | Підключення в Proteus |
|---|---:|---|
| I2C SCL | RC3 / pin 18 | Лінія I2C SCL з підтяжкою |
| I2C SDA | RC4 / pin 23 | Лінія I2C SDA з підтяжкою |
| VDD | pins 11, 32 | +5V |
| VSS | pins 12, 31 | GND |
| MCLR | pin 1 | Pull-up reset за потреби |

## Очікуваний результат

UART друкує кількість знайдених I2C-пристроїв та їх адреси.

## Примітки

- Не копіюйте HEX у цю папку.
- Після збірки оновлюйте HEX через MPLAB-проєкт.
- Proteus має завантажувати HEX із спільної папки `hex/`.
- Оновлюйте `proteus-version.txt` після ручного збереження проєкту Proteus.

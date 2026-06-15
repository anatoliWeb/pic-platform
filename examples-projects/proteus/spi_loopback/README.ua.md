# spi_loopback Proteus simulation

## Статус
Потрібне ручне налаштування Proteus.

## Firmware

XC8 HEX:

`../../hex/xc8/spi_loopback/spi_loopback.hex`

C18 HEX поки недоступний для цього проєкту.

## MPLAB-проєкти

- `../../xc8/spi_loopback.X`
- `../../c18/spi_loopback.X`

## Підключення до PIC18F452

| Сигнал | Ніжка PIC18F452 | Підключення в Proteus |
|---|---:|---|
| SPI SCK | RC3 / pin 18 | Лінія SPI clock |
| SPI SDI | RC4 / pin 23 | З’єднати зі SPI SDO для loopback |
| SPI SDO | RC5 / pin 24 | З’єднати зі SPI SDI для loopback |
| VDD | pins 11, 32 | +5V |
| VSS | pins 12, 31 | GND |
| MCLR | pin 1 | Pull-up reset за потреби |

## Очікуваний результат

UART друкує тестовий байт, отриманий назад через SPI loopback wiring.

## Примітки

- Не копіюйте HEX у цю папку.
- Після збірки оновлюйте HEX через MPLAB-проєкт.
- Proteus має завантажувати HEX із спільної папки `hex/`.
- Оновлюйте `proteus-version.txt` після ручного збереження проєкту Proteus.

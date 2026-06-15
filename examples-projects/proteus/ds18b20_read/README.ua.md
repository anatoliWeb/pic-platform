# ds18b20_read Proteus simulation

## Статус
Потрібне ручне налаштування Proteus.

## Firmware

XC8 HEX:

`../../hex/xc8/ds18b20_read/ds18b20_read.hex`

C18 HEX поки недоступний для цього проєкту.

## MPLAB-проєкти

- `../../xc8/ds18b20_read.X`
- `../../c18/ds18b20_read.X`

## Підключення до PIC18F452

| Сигнал | Ніжка PIC18F452 | Підключення в Proteus |
|---|---:|---|
| DS18B20 DQ | RB1 / pin 34 | 1-Wire data line з підтяжкою 4.7k до +5V |
| VDD | pins 11, 32 | +5V |
| VSS | pins 12, 31 | GND |
| MCLR | pin 1 | Pull-up reset за потреби |

## Очікуваний результат

UART друкує коректну температуру DS18B20 через режим одиночного датчика.

## Примітки

- Не копіюйте HEX у цю папку.
- Після збірки оновлюйте HEX через MPLAB-проєкт.
- Proteus має завантажувати HEX із спільної папки `hex/`.
- Оновлюйте `proteus-version.txt` після ручного збереження проєкту Proteus.

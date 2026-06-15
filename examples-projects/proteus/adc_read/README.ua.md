# adc_read Proteus simulation

## Статус
Потрібне ручне налаштування Proteus.

## Firmware

XC8 HEX:

`../../hex/xc8/adc_read/adc_read.hex`

C18 HEX поки недоступний для цього проєкту.

## MPLAB-проєкти

- `../../xc8/adc_read.X`
- `../../c18/adc_read.X`

## Підключення до PIC18F452

| Сигнал | Ніжка PIC18F452 | Підключення в Proteus |
|---|---:|---|
| ADC input | RA0 / AN0 / pin 2 | Повзунок потенціометра або джерело аналогової напруги |
| VDD | pins 11, 32 | +5V |
| VSS | pins 12, 31 | GND |
| MCLR | pin 1 | Pull-up reset за потреби |

## Очікуваний результат

UART друкує сирі значення ADC0 під час зміни аналогового входу.

## Примітки

- Не копіюйте HEX у цю папку.
- Після збірки оновлюйте HEX через MPLAB-проєкт.
- Proteus має завантажувати HEX із спільної папки `hex/`.
- Оновлюйте `proteus-version.txt` після ручного збереження проєкту Proteus.

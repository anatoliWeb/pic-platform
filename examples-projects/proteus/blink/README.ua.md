# blink Proteus simulation

## Статус
Реальні Proteus-файли вже існують: `blink.pdsprj`, `_blink_.pdsprj`.

## Firmware

XC8 HEX:

`../../hex/xc8/blink/blink.hex`

C18 HEX:

`../../hex/c18/blink/blink.hex`

## MPLAB-проєкти

- `../../xc8/blink.X`
- `../../c18/blink.X`

## Підключення до PIC18F452

| Сигнал | Ніжка PIC18F452 | Підключення в Proteus |
|---|---:|---|
| Вихід LED | RB1 / pin 34 | LED + послідовний резистор на GND |
| VDD | pins 11, 32 | +5V |
| VSS | pins 12, 31 | GND |
| MCLR | pin 1 | Pull-up reset за потреби |

## Очікуваний результат

RB1 перемикається кожні 500 ms, LED блимає стабільно.

## Примітки

- Не копіюйте HEX у цю папку.
- Після збірки оновлюйте HEX через MPLAB-проєкт.
- Proteus має завантажувати HEX із спільної папки `hex/`.
- Оновлюйте `proteus-version.txt` після ручного збереження проєкту Proteus.

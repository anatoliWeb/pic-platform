# uart_debug Proteus simulation

## Статус
Потрібне ручне налаштування Proteus.

## Firmware

XC8 HEX:

`../../hex/xc8/uart_debug/uart_debug.hex`

C18 HEX поки недоступний для цього проєкту.

## MPLAB-проєкти

- `../../xc8/uart_debug.X`
- `../../c18/uart_debug.X`

## Підключення до PIC18F452

| Сигнал | Ніжка PIC18F452 | Підключення в Proteus |
|---|---:|---|
| UART TX | RC6 / pin 25 | RXD Virtual Terminal |
| UART RX | RC7 / pin 26 | TXD Virtual Terminal (за потреби) |
| VDD | pins 11, 32 | +5V |
| VSS | pins 12, 31 | GND |
| MCLR | pin 1 | Pull-up reset за потреби |

## Очікуваний результат

Virtual Terminal отримує періодичні debug-рядки на 9600 baud, 8N1.

## Примітки

- Не копіюйте HEX у цю папку.
- Після збірки оновлюйте HEX через MPLAB-проєкт.
- Proteus має завантажувати HEX із спільної папки `hex/`.
- Оновлюйте `proteus-version.txt` після ручного збереження проєкту Proteus.

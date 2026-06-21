# software_pwm Proteus Налаштування

## Призначення

Підготувати нотатки для Proteus simulation для початкового XC8-прикладу `software_pwm.X`.

## Компоненти

- PIC18F452
- кварц 10 MHz з конденсаторами
- 4 LED
- 4 резистори по 330R
- підтягування MCLR, 10 k
- живлення 5V і GND

## Підключення

- RB0 pin 33 -> LED0 -> 330R -> GND
- RB1 pin 34 -> LED1 -> 330R -> GND
- RB2 pin 35 -> LED2 -> 330R -> GND
- RB3 pin 36 -> LED3 -> 330R -> GND
- VDD pins 11 і 32 -> +5V
- VSS pins 12 і 31 -> GND
- MCLR pin 1 -> підтягування 10 k до +5V
- OSC1 pin 13 / OSC2 pin 14 -> кварц 10 MHz

## Очікувана поведінка

- Чотири LED мають різну середню яскравість.
- RB0 є найтьмянішим каналом.
- RB3 є найяскравішим каналом.
- Усі канали мають одну базову PWM-частоту.

## Примітки

- Файл Proteus `.pdsprj` потрібно створити вручну за потреби.
- Майбутній Proteus project слід зберігати у `examples-projects/proteus/software_pwm/`.
- Властивість PIC Clock Frequency у Proteus має бути `10MHz`.

## Статус

Готово до перевірки в Proteus.

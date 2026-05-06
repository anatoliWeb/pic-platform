# Build Validation (MPLAB C18 / XC8)

Цей файл описує ручну перевірку збірки в MPLAB X, оскільки в поточному середовищі немає доступу до toolchain C18/XC8.

## 1. Загальна підготовка

1. Відкрити репозиторій `pic-platform` у MPLAB X.
2. Вказати цільовий MCU (наприклад, PIC18F458 або ваш цільовий PIC18).
3. Додати include paths:
   - `core/`
   - `drivers/`
   - `C18/drivers/` або `XC8/drivers/` залежно від конфігурації.
4. Перевірити config bits та `_XTAL_FREQ / DRV_XTAL_FREQ` у `core/device.h`.

## 2. Перевірка збірки C18

1. Створити конфігурацію проєкту під C18.
2. Зібрати спочатку прості приклади:
   - `C18/examples/template_example.c`
   - `C18/examples/gpio_example.c`
   - `C18/examples/uart_example.c`
3. Потім зібрати приклади комунікацій:
   - `C18/examples/i2c_example.c`
   - `C18/examples/spi_example.c`
   - `C18/examples/rs485_example.c`
4. Потім периферійні/сервісні:
   - `C18/examples/ds18b20_example.c`
   - `C18/examples/timer0_example.c`, `timer1_example.c`, `timer2_example.c`, `timer3_example.c`
   - `C18/examples/encoder_example.c`

## 3. Перевірка збірки XC8

1. Створити конфігурацію проєкту під XC8.
2. Повторити той самий порядок прикладів:
   - `XC8/examples/template_example.c`
   - `XC8/examples/gpio_example.c`
   - `XC8/examples/uart_example.c`
   - `XC8/examples/i2c_example.c`
   - `XC8/examples/spi_example.c`
   - `XC8/examples/rs485_example.c`
   - `XC8/examples/ds18b20_example.c`
   - `XC8/examples/timer0_example.c` ... `timer3_example.c`
   - `XC8/examples/encoder_example.c`

## 4. Очікувані помилки/ризики

- Різниця імен бітів/регістрів між конкретними PIC18 моделями.
- Розбіжності config bits між C18 і XC8.
- Відсутність або інша версія заголовків MCU (`p18f458.h`, `xc.h`).
- Помилки лінкування, якщо не підключені ISR hooks у прикладах, що їх очікують.

## 5. Що вважати успішною валідацією

- Кожен приклад компілюється без синтаксичних помилок.
- Немає unresolved symbols по API драйверів.
- Для runtime-перевірки: базові тести GPIO/UART/Timer проходять на платі.

## Примітка

У цьому середовищі реальна збірка C18/XC8 не виконувалась.

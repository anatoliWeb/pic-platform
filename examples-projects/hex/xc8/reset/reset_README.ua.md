# PIC18F452 Reset cause test

Цей приклад перевіряє драйвер `reset` і показує, з якої причини мікроконтролер PIC18F452 стартував або перезапустився.

Ідея прикладу проста: після кожного запуску програма читає reset-прапори мікроконтролера, друкує причину reset у UART, очищає прапори і чекає наступного reset.

## Для чого це потрібно

Reset-драйвер потрібен, щоб після старту прошивки зрозуміти, що саме сталося з мікроконтролером:

- увімкнули живлення;
- натиснули кнопку `MCLR`;
- спрацював Watchdog Timer;
- було просідання живлення Brown-out;
- був software reset через інструкцію `RESET`.

Це корисно для діагностики реальних пристроїв. Наприклад, якщо плата іноді перезапускається сама, reset cause допомагає зрозуміти: це просідання живлення, зависання програми з Watchdog reset, чи зовнішній reset по `MCLR`.

## Що саме робить приклад

При старті `main.c` виконує такі кроки:

1. запускає UART на `9600 baud`;
2. ініціалізує reset-драйвер через `reset_init()`;
3. читає причину reset через `reset_get_cause()`;
4. друкує код і текст причини reset у Virtual Terminal;
5. очищає reset-прапори через `reset_clear_flags()`;
6. чекає наступного reset.

Після натискання кнопки на `MCLR` мікроконтролер перезапускається, і приклад знову друкує причину reset.

## Важливо про PIC18F452

У PIC18F452 немає окремого простого прапора типу `MCLR_RESET_FLAG`. Тому драйвер визначає причину приблизно так:

```text
POR == 0 -> POWER_ON
BOR == 0 -> BROWN_OUT
TO  == 0 -> WATCHDOG
RI  == 0 -> SOFTWARE
інакше    -> EXTERNAL_MCLR
```

Тобто `EXTERNAL_MCLR` використовується як fallback, коли немає ознак Power-on, Brown-out, Watchdog або Software reset.

## Reset causes

У драйвері використовуються такі значення:

| Код | Назва | Значення |
|---:|---|---|
| 0 | `RESET_CAUSE_UNKNOWN` | причина не визначена |
| 1 | `RESET_CAUSE_POWER_ON` | старт після подачі живлення |
| 2 | `RESET_CAUSE_BROWN_OUT` | reset через просідання живлення |
| 3 | `RESET_CAUSE_WATCHDOG` | reset через Watchdog Timer |
| 4 | `RESET_CAUSE_EXTERNAL` | зовнішній reset, у цьому прикладі `MCLR` |
| 5 | `RESET_CAUSE_SOFTWARE` | software reset через інструкцію `RESET` |

## Proteus wiring

| Елемент | PIC18F452 | DIP-40 pin | Призначення |
|---|---|---:|---|
| MCLR pull-up | `MCLR` -> `10k` -> `+5V` | 1 | нормальна робота PIC |
| Reset button | `MCLR` -> button -> `GND` | 1 | ручний reset |
| UART TX | `RC6/TX` -> Virtual Terminal `RXD` | 25 | вивід повідомлень |
| UART RX | `RC7/RX` <- Virtual Terminal `TXD` | 26 | можна підключити для повної UART-схеми |
| VDD | `+5V` | 11, 32 | живлення |
| VSS | `GND` | 12, 31 | земля |
| Crystal | `OSC1/OSC2` | 13, 14 | такт, наприклад `10 MHz` |

Схема кнопки reset:

```text
+5V
 |
10k
 |
MCLR pin 1 ---- button ---- GND
```

## Expected UART output

Після першого старту симуляції:

```text
PIC18F452 Reset cause test
MCLR: pin 1 -> 10k -> +5V
MCLR button: pin 1 -> button -> GND

Reset cause code=1 POWER_ON

Reset flags cleared.
Press MCLR button in Proteus to test external reset.
```

Після натискання кнопки `MCLR`:

```text
PIC18F452 Reset cause test
MCLR: pin 1 -> 10k -> +5V
MCLR button: pin 1 -> button -> GND

Reset cause code=4 EXTERNAL_MCLR

Reset flags cleared.
Press MCLR button in Proteus to test external reset.
```

## Source files

Для `reset.X` потрібні такі файли:

```text
config_bits.c
main.c
../../../core/delay.c
../../../drivers/communication/uart/uart.c
../../../libraries/system/uart_debug/uart_debug.c
../../../drivers/system/reset/reset.c
```

## Що перевіряти, якщо не працює

1. `MCLR` має мати pull-up `10k` до `+5V`.
2. Кнопка має замикати `MCLR` на `GND`.
3. UART `RC6/TX` має йти на `RXD` Virtual Terminal.
4. Частота PIC у Proteus має відповідати `PIC_PLATFORM_CLOCK_HZ`, наприклад `10 MHz`.
5. `reset_clear_flags()` треба викликати тільки після `reset_get_cause()`.

## Примітка про Watchdog

Цей приклад тестує Power-on reset і зовнішній reset через `MCLR`. Для Watchdog reset краще зробити окремий приклад, наприклад `reset_wdt.X`, де Watchdog увімкнений у config bits і програма навмисно не викликає `CLRWDT`.

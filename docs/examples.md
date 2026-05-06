> Навігація: [README (EN)](../README.md) | [README (UA)](../README.ua.md) | [Конвенції](driver-convention.md) | [Компілятори](compiler.md) | [Іменування](naming.md) | [Приклади](examples.md) | [Портинг](porting.md) | [Build Validation](build-validation.md)
# Робота З Прикладами

## Призначення прикладів
Приклади показують базову інтеграцію драйвера, не повний application.

## Де шукати приклади
- Universal: `drivers/<driver>/example.c`
- C18: `C18/examples/<driver>_example.c`
- XC8: `XC8/examples/<driver>_example.c`

## Як запускати перевірку
1. Обрати цільовий драйвер
2. Відкрити відповідний `example.c`
3. Перевірити include paths (`core`, `drivers`, `C18`/`XC8`)
4. Зібрати в MPLAB X під потрібний компілятор

## Що перевіряти спочатку
- `gpio`, `uart`, `timer`/`tick`
- Потім `i2c`, `spi`, `adc`
- Потім протокольні/складні (`rs485`, `onewire`, `encoder`)

## Порада
Якщо є помилка, починайте з найпростішого прикладу для швидкої локалізації проблеми.


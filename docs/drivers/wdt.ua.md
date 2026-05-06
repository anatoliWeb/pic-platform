[🇬🇧 English version](./wdt.md)

# WDT Helper

## Опис
Допоміжний модуль Watchdog для періодичного очищення таймера.

## API
- `wdt_clear()`
- `WDT_CLEAR()`

## Приклад
```c
while (1)
{
    // логіка застосунку
    wdt_clear();
}
```

## Особливості
- Runtime enable/disable залежить від конкретного MCU.
- Поведінка WDT переважно задається config bits.

## Залежності
- `core/compiler.h`

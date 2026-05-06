> Навігація: [README (EN)](../README.md) | [README (UA)](../README.ua.md) | [Конвенції](driver-convention.md) | [Компілятори](compiler.md) | [Іменування](naming.md) | [Приклади](examples.md) | [Портинг](porting.md) | [Build Validation](build-validation.md)
# Перенесення Драйвера В Новий Проєкт

## Що копіювати
Мінімально:
- `drivers/<driver>/`
- потрібні залежності з `drivers/`
- `core/` файли (мінімум: `compiler.h`, `types.h`, `device.h`, `delay.h`)

## Порядок інтеграції
1. Додати `core` в include path
2. Додати `drivers` в include path
3. Додати C18 або XC8 реалізацію (за потреби)
4. Підключити драйвер у коді
5. Викликати `init()` і базові API

## Перевірка залежностей
Перед переносом перевірити, чи драйвер використовує:
- `uart`, `gpio`, `tick`, `crc` або інші helper-и

## Конфігурація MCU
- Перевірити `_XTAL_FREQ / DRV_XTAL_FREQ`
- Перевірити config bits
- Перевірити відповідність пінів платі

## Типові помилки
- Не підключено залежний драйвер
- Невірний include path
- Невідповідність MCU header
- Неініціалізований clock/tick


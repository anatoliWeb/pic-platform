# PIC Drivers Platform (C18 + XC8)

> [English version](README.md)

Багаторазова embedded-платформа для PIC18 з чітким розділенням шарів і
переносимою архітектурою.

Це платформа драйверів і бібліотек, а не прикладний застосунок.

## Архітектура

- `/core` - абстракція компілятора та програмні утиліти
- `/drivers` - низькорівневі hardware/protocol модулі
- `/libraries` - високорівневі багаторазові компоненти
- `/C18` - compiler-specific файли для MPLAB C18
- `/XC8` - compiler-specific файли для MPLAB XC8
- `/docs` - документація
- `/examples-projects` - готові MPLAB приклади
- `/prompts` - готові prompt-файли для AI-чатів

## Шар Core

- `core/compiler.h`, `core/types.h`, `core/bit_utils.h`, `core/delay.*`
- `core/crc/*`, `core/ring_buffer/*`, `core/scheduler/*`
- `core/rtos/*` для опціональної RTOS-абстракції

## RTOS Абстракція

- Залежність від FreeRTOS не додається.
- Перемикач збірки: `DRV_USE_FREERTOS`
- `rtos_sleep_ms()` працює у bare-metal режимі через `delay_ms()`.
- `rtos_sleep_us()` працює через `delay_us()`.
- Бібліотеки мають використовувати `core/rtos` API, а не прямі RTOS-виклики.

## Швидкий старт

1. Тримайте `pic-platform` окремо від application project.
2. Додавайте в MPLAB лише потрібні `.c` файли у `Source Files`.
3. Не додавайте `.h` файли вручну у `Source Files`.
4. Налаштуйте include paths:
   - `../pic-platform`
   - `../pic-platform/core`
   - `../pic-platform/drivers`
   - `../pic-platform/libraries`
5. Підключайте заголовки та викликайте init API у коді застосунку.

## Документація

- Архітектура: [docs/architecture.ua.md](docs/architecture.ua.md) | [EN](docs/architecture.md)
- MPLAB інтеграція: [docs/mplab-integration.ua.md](docs/mplab-integration.ua.md) | [EN](docs/mplab-integration.md)
- Приклади проєктів: [examples-projects/README.ua.md](examples-projects/README.ua.md) | [EN](examples-projects/README.md)
- Набір промптів: [docs/prompts.ua.md](docs/prompts.ua.md) | [EN](docs/prompts.md)

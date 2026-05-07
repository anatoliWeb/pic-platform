# PIC Drivers Platform (C18 + XC8)

> [English version](README.md)

Перевикористовувана платформа для PIC18 із чітким розділенням шарів та переносимою архітектурою.
Це платформа драйверів/бібліотек, а не прикладний застосунок.

## Архітектура
- `/core` - абстракція компілятора та програмні утиліти
- `/drivers` - низькорівневі модулі периферії/протоколів
- `/libraries` - високорівневі перевикористовувані компоненти
- `/C18` - compiler-specific файли для MPLAB C18
- `/XC8` - compiler-specific файли для MPLAB XC8
- `/docs` - документація
- `/examples-projects` - готові MPLAB приклади

## Шар Core
- `core/compiler.h`, `core/types.h`, `core/bit_utils.h`, `core/delay.*`
- `core/crc/*`, `core/ring_buffer/*`, `core/scheduler/*`
- `core/rtos/*` для опційної RTOS-абстракції

## RTOS-абстракція (перший етап)
- Залежність від FreeRTOS не додається.
- Перемикач збірки: `DRV_USE_FREERTOS`
- `rtos_delay_ms()` у bare-metal режимі працює через `delay_ms()`.
- API для mutex/queue додано як заглушки для майбутнього backend.
- Бібліотеки повинні використовувати тільки `core/rtos` API (без прямих RTOS-викликів).

## Швидкий старт
1. Тримайте `pic-platform` окремо від застосунку.
2. Додавайте в MPLAB тільки потрібні `.c` у `Source Files`.
3. Налаштуйте include paths:
   - `../pic-platform`
   - `../pic-platform/core`
   - `../pic-platform/drivers`
   - `../pic-platform/libraries`
4. Підключайте заголовки та викликайте init API у коді застосунку.

## Документація
- Архітектура: [docs/architecture.ua.md](docs/architecture.ua.md) | [EN](docs/architecture.md)
- Інтеграція MPLAB: [docs/mplab-integration.ua.md](docs/mplab-integration.ua.md) | [EN](docs/mplab-integration.md)
- Приклади проєктів: [examples-projects/README.ua.md](examples-projects/README.ua.md) | [EN](examples-projects/README.md)
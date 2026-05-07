[English version](./architecture.md)

# Архітектура

## Рівні
- `core/`: абстракція компілятора, спільні типи, бітові утиліти, delay, CRC, ring buffer, scheduler, RTOS-абстракція.
- `drivers/`: низькорівневі модулі роботи з периферією та протоколами.
- `libraries/`: високорівневі перевикористовувані компоненти поверх core/drivers.

## Відповідальність рівнів
- Core: переносимі програмні утиліти та платформений шар.
- Drivers: тільки доступ до периферії/протоколів MCU.
- Libraries: логіка компонентів (дисплеї, сенсори, input тощо).

## Правила залежностей
- `libraries` можуть залежати від `drivers` і `core`.
- `drivers` можуть залежати від `core`.
- `drivers` не повинні залежати від `libraries`.
- `core` бажано не залежить від `drivers` та `libraries`.

## RTOS-абстракція
- Розташування: `core/rtos/`.
- Файли:
  - `core/rtos/rtos.h`
  - `core/rtos/rtos_port.h`
  - `core/rtos/rtos.c`
- Поточний стан:
  - `rtos_delay_ms()` реалізовано для bare-metal через `delay_ms()`.
  - API mutex/queue додано як заглушки для майбутнього backend.
- Перемикач збірки:
  - `DRV_USE_FREERTOS` не задано або `0`: bare-metal режим.
  - `DRV_USE_FREERTOS=1`: зарезервовано для майбутнього мапінгу на FreeRTOS.

## Цілі переносимості
- Ізолювати compiler-specific відмінності.
- Тримати device-specific заголовки на рівні application.
- Не залежати від RTOS у бібліотеках напряму: використовувати `core/rtos` API.

## Приклади шарування
- Правильно: `libraries/sensors/ds18b20` -> `drivers/communication/onewire` + `core/crc`.
- Правильно: `drivers/communication/rs485` -> `drivers/communication/uart` + `drivers/gpio` + `core/crc`.
- Неправильно: include з `drivers/*` до `libraries/*`.
- Неправильно: прямі виклики `xTaskCreate()/vTaskDelay()` у бібліотеках.
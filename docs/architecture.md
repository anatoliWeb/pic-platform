[Ukrainian version](./architecture.ua.md)

# Architecture

## Layers
- `core/`: compiler abstraction, shared types, bit utilities, delay, CRC, ring buffer, scheduler, RTOS abstraction.
- `drivers/`: low-level hardware and protocol modules.
- `libraries/`: high-level reusable components built on core/drivers.

## Responsibilities
- Core: portable software utilities and platform glue.
- Drivers: MCU peripheral/protocol access only.
- Libraries: reusable component logic (display, sensors, input, etc.).

## Dependency Rules
- `libraries` may depend on `drivers` and `core`.
- `drivers` may depend on `core`.
- `drivers` must not depend on `libraries`.
- `core` should avoid dependencies on `drivers` and `libraries`.

## RTOS Abstraction
- Location: `core/rtos/`.
- Files:
  - `core/rtos/rtos.h`
  - `core/rtos/rtos_port.h`
  - `core/rtos/rtos.c`
- Current state:
  - `rtos_delay_ms()` implemented for bare-metal mode via `delay_ms()`.
  - mutex/queue APIs are placeholders for future backend mapping.
- Build switch:
  - `DRV_USE_FREERTOS` undefined or `0`: bare-metal behavior.
  - `DRV_USE_FREERTOS=1`: reserved path for future FreeRTOS mapping.

## Portability Goals
- Keep compiler-specific behavior isolated.
- Keep device-specific headers in application layer.
- Keep libraries RTOS-independent by using `core/rtos` APIs.

## Layering Examples
- Correct: `libraries/sensors/ds18b20` -> `drivers/communication/onewire` + `core/crc`.
- Correct: `drivers/communication/rs485` -> `drivers/communication/uart` + `drivers/gpio` + `core/crc`.
- Incorrect: include from `drivers/*` to `libraries/*`.
- Incorrect: direct `xTaskCreate()/vTaskDelay()` calls in libraries.
# Architecture

## Layers
- `core/`: compiler abstraction, shared types, bit utils, delay, CRC, ring buffer, scheduler, RTOS abstraction.
- `drivers/`: low-level hardware and protocol modules.
- `libraries/`: high-level reusable components built on core/drivers.

## Responsibility
- Core provides portable software utilities and platform glue.
- Drivers access MCU peripherals/protocols and must not contain product logic.
- Libraries implement reusable component logic (display, sensors, input, etc.).

## Dependency Rules
- `libraries` may depend on `drivers` and `core`.
- `drivers` may depend on `core`.
- `drivers` must not depend on `libraries`.
- `core` must not depend on `drivers` or `libraries` where avoidable.

## Portability Goals
- Keep compiler-specific behavior isolated.
- Keep device-specific headers in application layer.
- Use `core/compiler.h` unified compiler macros.

## FreeRTOS Preparation
- Optional abstraction layer: `core/rtos/`.
- Current files:
  - `core/rtos/rtos.h`
  - `core/rtos/rtos_port.h`
- Goal: allow bare-metal and FreeRTOS modes without changing library APIs.

## Layering Examples
- Correct: `libraries/sensors/ds18b20` -> `drivers/communication/onewire` + `core/crc`.
- Correct: `drivers/communication/rs485` -> `drivers/communication/uart` + `drivers/gpio` + `core/crc`.
- Incorrect: any include from `drivers/*` to `libraries/*`.
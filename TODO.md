# TODO

> [РЈРєСЂР°С—РЅСЃСЊРєР° РІРµСЂСЃС–СЏ](TODO.ua.md)

## Structure Refactor
- [x] Split source tree into `/drivers` and `/libraries`
- [x] Move low-level modules to grouped driver folders
- [x] Move high-level modules to grouped library folders
- [x] Move compiler-specific C18/XC8 sources to mirrored grouped folders
- [x] Move driver/library docs to `/docs/drivers` and `/docs/libraries`
- [x] Update include paths and internal source routing paths

## Validation
- [x] No old flat source folders used in code includes
- [ ] Validate MPLAB XC8 build after file moves
- [ ] Validate MPLAB C18 build after file moves
- [ ] Validate examples-projects `.X` projects with updated source references

## Core Utility Relocation
- [x] Moved CRC from `drivers/utils` to `core/crc`
- [x] Moved ring buffer from `drivers/utils` to `core/ring_buffer`
- [x] Moved scheduler from `drivers/utils` to `core/scheduler`

## RTOS Preparation
- [x] Added `core/rtos/rtos.h`
- [x] Added `core/rtos/rtos_port.h`
- [ ] Future FreeRTOS backend implementation (optional)
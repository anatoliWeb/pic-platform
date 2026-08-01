# Module Inventory

## Core

| Name | Path | Public header | Implementation file | Depends on | Used by | Notes | Status |
| --- | --- | --- | --- | --- | --- | --- | --- |
| compiler | `core/compiler.h` | `core/compiler.h` | header-only | compiler built-ins | almost every module | compiler selection and interrupt helpers | example-tested |
| types | `core/types.h` | `core/types.h` | header-only | `core/compiler.h` | drivers and libraries | `drv_status_t`, `drv_bool_t` | example-tested |
| device | `core/device.h` | `core/device.h` | header-only | `core/config/project_config_template.h` | delay, uart, timers | resolves clock macros | example-tested |
| delay | `core/delay.h` | `core/delay.h` | `core/delay.c` | compiler, device | seven-segment examples, shared-line input examples | blocking delays | example-tested |
| config | `core/config.h` | `core/config.h` | header-only | none | `uart_debug` | debug feature flags | example-tested |
| pic platform config | `core/pic_platform_config.h` | `core/pic_platform_config.h` | header-only | none | `seven_segment`, `position_drive` | default timer backend and feature flags | example-tested |
| bit utils | `core/bit_utils.h` | `core/bit_utils.h` | header-only | none | `button` | bit macros for GPIO reads | example-tested |
| interrupts | `core/interrupts.h` | `core/interrupts.h` | header-only | compiler | `seven_segment` | central interrupt-control contract | example-tested |

## Drivers

| Name | Path | Public header | Implementation file | Depends on | Used by | Notes | Status |
| --- | --- | --- | --- | --- | --- | --- | --- |
| gpio | `drivers/gpio/` | `drivers/gpio/gpio.h` | `drivers/gpio/gpio.c` | compiler, types | `seven_segment`, `segment_keys`, wrappers | direct register helpers | example-tested |
| timer0 | `drivers/timers/timer0/` | `drivers/timers/timer0/timer0.h` | `drivers/timers/timer0/timer0.c` | compiler, types | `seven_segment` backend | callback-based timer driver | example-tested |
| timer1 | `drivers/timers/timer1/` | `drivers/timers/timer1/timer1.h` | `drivers/timers/timer1/timer1.c` | compiler, types | `tick`, `seven_segment` backend | owns millisecond tick in this repo | example-tested |
| timer2 | `drivers/timers/timer2/` | `drivers/timers/timer2/timer2.h` | `drivers/timers/timer2/timer2.c` | compiler, types | `seven_segment` backend | used by `multiplex_timer.X` | example-tested |
| timer3 | `drivers/timers/timer3/` | `drivers/timers/timer3/timer3.h` | `drivers/timers/timer3/timer3.c` | compiler, types | `seven_segment` backend | not used by seven-segment examples here | example-tested |
| tick | `drivers/timers/tick/` | `drivers/timers/tick/tick.h` | `drivers/timers/tick/tick.c` | `timer1`, compiler, device | `button`, many examples | millisecond time base | example-tested |
| uart | `drivers/communication/uart/` | `drivers/communication/uart/uart.h` | `drivers/communication/uart/uart.c` | compiler, device | `uart_debug`, UART examples | RC6/RC7 UART peripheral | example-tested |

## Libraries

| Name | Path | Public header | Implementation file | Depends on | Used by | Notes | Status |
| --- | --- | --- | --- | --- | --- | --- | --- |
| seven_segment | `libraries/display/seven_segment/` | `libraries/display/seven_segment/seven_segment.h` | `libraries/display/seven_segment/seven_segment.c` | `gpio`, timer drivers, core headers | seven-segment examples, `segment_keys` | manual and timer-backed refresh | example-tested |
| button | `libraries/input/button/` | `libraries/input/button/button.h` | `libraries/input/button/button.c` | `tick`, `bit_utils`, core headers | `segment_keys`, button examples | debounce/click/hold logic | example-tested |
| segment_keys | `libraries/input/segment_keys/` | `libraries/input/segment_keys/segment_keys.h` | `libraries/input/segment_keys/segment_keys.c` | `seven_segment`, `button`, `gpio` | seven-segment shared-line examples | exact-mask decoder | example-tested |
| position_drive | `libraries/actuator/position_drive/` | `libraries/actuator/position_drive/position_drive.h` | `libraries/actuator/position_drive/position_drive.c` | core headers, optional `tick`/`adc`/`pwm` via callbacks | position drive examples | ADC backend implemented, encoder placeholder unsupported, callback-based debug | example-tested |
| uart_debug | `libraries/system/uart_debug/` | `libraries/system/uart_debug/uart_debug.h` | `libraries/system/uart_debug/uart_debug.c` | `core/config.h`, `uart` | UART-based debug examples | macro-based debug layer | example-tested |

## Compiler Wrappers / Backends

| Name | Path | Public header | Implementation file | Depends on | Used by | Notes | Status |
| --- | --- | --- | --- | --- | --- | --- | --- |
| gpio XC8 backend | `XC8/drivers/gpio/` | `drivers/gpio/gpio.h` | `XC8/drivers/gpio/gpio.c` | XC8 device headers | top-level `drivers/gpio/gpio.c` | XC8 implementation only | wrapper-only |
| gpio C18 backend | `C18/drivers/gpio/` | `drivers/gpio/gpio.h` | `C18/drivers/gpio/gpio.c` | C18 device headers | top-level `drivers/gpio/gpio.c` | C18 implementation only | wrapper-only |
| uart XC8 backend | `XC8/drivers/communication/uart/` | `drivers/communication/uart/uart.h` | `XC8/drivers/communication/uart/uart.c` | XC8 device headers | top-level `drivers/communication/uart/uart.c` | XC8 UART implementation | wrapper-only |
| uart C18 backend | `C18/drivers/communication/uart/` | `drivers/communication/uart/uart.h` | `C18/drivers/communication/uart/uart.c` | C18 device headers | top-level `drivers/communication/uart/uart.c` | C18 UART implementation | wrapper-only |
| seven_segment XC8 backend | `XC8/libraries/display/seven_segment/` | `libraries/display/seven_segment/seven_segment.h` | `XC8/libraries/display/seven_segment/seven_segment.c` | GPIO backend | top-level `libraries/display/seven_segment/seven_segment.c` | XC8 optimized backend | wrapper-only |
| seven_segment C18 backend | `C18/libraries/display/seven_segment/` | `libraries/display/seven_segment/seven_segment.h` | `C18/libraries/display/seven_segment/seven_segment.c` | GPIO backend | top-level `libraries/display/seven_segment/seven_segment.c` | C18 backend | wrapper-only |
| button XC8 backend | `XC8/libraries/input/button/` | `libraries/input/button/button.h` | `XC8/libraries/input/button/button.c` | `tick` | top-level `libraries/input/button/button.c` | XC8 backend | wrapper-only |
| button C18 backend | `C18/libraries/input/button/` | `libraries/input/button/button.h` | `C18/libraries/input/button/button.c` | `tick` | top-level `libraries/input/button/button.c` | C18 backend | wrapper-only |
| segment_keys XC8 backend | `XC8/libraries/input/segment_keys/` | `libraries/input/segment_keys/segment_keys.h` | `XC8/libraries/input/segment_keys/segment_keys.c` | generic source | top-level `libraries/input/segment_keys/segment_keys.c` | XC8 include stub | wrapper-only |
| segment_keys C18 backend | `C18/libraries/input/segment_keys/` | `libraries/input/segment_keys/segment_keys.h` | `C18/libraries/input/segment_keys/segment_keys.c` | generic source | top-level `libraries/input/segment_keys/segment_keys.c` | C18 include stub | wrapper-only |
| position_drive XC8 backend | `XC8/libraries/actuator/position_drive/` | `libraries/actuator/position_drive/position_drive.h` | `XC8/libraries/actuator/position_drive/position_drive.c` | generic source | top-level `libraries/actuator/position_drive/position_drive.c` | XC8 include stub | wrapper-only |
| position_drive C18 backend | `C18/libraries/actuator/position_drive/` | `libraries/actuator/position_drive/position_drive.h` | `C18/libraries/actuator/position_drive/position_drive.c` | generic source | top-level `libraries/actuator/position_drive/position_drive.c` | C18 include stub | wrapper-only |
| uart_debug XC8 backend | `XC8/libraries/system/uart_debug/` | `libraries/system/uart_debug/uart_debug.h` | `XC8/libraries/system/uart_debug/uart_debug.c` | `uart` | top-level `libraries/system/uart_debug/uart_debug.c` | XC8 debug backend | wrapper-only |
| uart_debug C18 backend | `C18/libraries/system/uart_debug/` | `libraries/system/uart_debug/uart_debug.h` | `C18/libraries/system/uart_debug/uart_debug.c` | `uart` | top-level `libraries/system/uart_debug/uart_debug.c` | C18 debug backend | wrapper-only |

## Example Projects

| Name | Path | Public header | Implementation file | Depends on | Used by | Notes | Status |
| --- | --- | --- | --- | --- | --- | --- | --- |
| basic_manual | `examples-projects/xc8/seven_segment/basic_manual.X` | `project_config.h` | `main.c` | `delay`, `gpio`, `seven_segment` | Proteus-ready HEX artifact | single-digit manual refresh | example-tested |
| multiplex_manual | `examples-projects/xc8/seven_segment/multiplex_manual.X` | `project_config.h` | `main.c` | `delay`, `gpio`, `seven_segment` | Proteus-ready HEX artifact | four-digit manual multiplex | example-tested |
| multiplex_timer | `examples-projects/xc8/seven_segment/multiplex_timer.X` | `project_config.h` | `main.c` | `delay`, `gpio`, `timer2`, `seven_segment` | Proteus-ready HEX artifact | Timer2 refresh ownership | example-tested |
| keys_single_line | `examples-projects/xc8/seven_segment/keys_single_line.X` | `project_config.h` | `main.c` | `delay`, `uart`, `gpio`, `tick`, `timer1`, `seven_segment`, `button`, `segment_keys`, `uart_debug` | Proteus-ready HEX artifact | single-line keys | example-tested |
| keys_diode_coded | `examples-projects/xc8/seven_segment/keys_diode_coded.X` | `project_config.h` | `main.c` | `delay`, `uart`, `gpio`, `tick`, `timer1`, `seven_segment`, `button`, `segment_keys`, `uart_debug` | Proteus-ready HEX artifact | diode-coded keys | example-tested |
| position_drive_adc | `examples-projects/xc8/actuator/position_drive_adc.X` | `project_config.h` | `main.c` | `delay`, `adc`, `gpio`, `tick`, `timer1`, `position_drive`, `uart_debug` | Proteus-ready HEX artifact | ADC position sensor demo | example-tested |

## Docs

| Name | Path | Public header | Implementation file | Depends on | Used by | Notes | Status |
| --- | --- | --- | --- | --- | --- | --- | --- |
| architecture overview | `docs/architecture.md` | n/a | n/a | repo docs | README | general layering rules | docs |
| console build | `docs/build/console-build.md` | n/a | n/a | MPLAB/XC8 workflow | README | command-line build reference | docs |
| dependency graph | `docs/architecture/dependency-graph.md` | n/a | n/a | repo code and project files | README | dependency graph for platform | docs |
| example matrix | `docs/architecture/example-matrix.md` | n/a | n/a | project files and HEX artifacts | README | example-to-file mapping | docs |
| module inventory | `docs/architecture/module-inventory.md` | n/a | n/a | repo code and project files | README | inventory used for this audit | docs |
| generation workflow | `docs/architecture/generation-workflow.md` | n/a | n/a | build outputs and project metadata | README | build and artifact regeneration | docs |

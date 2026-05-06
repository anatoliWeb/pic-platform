# MPLAB Integration

Guide for integrating `pic-platform` as an external driver library in a PIC18 MPLAB X project.

## Integration Rules

- Keep `pic-platform` in a separate folder.
- Do not copy drivers into the application project directory.
- Use only relative paths (portable setup, no absolute paths).

## Add Source Files Correctly

- Add only `.c` files to **Source Files** in MPLAB X.
- Do not add `.h` files manually.
- Header files are resolved by include paths and `#include` directives.

### Example (`Source Files`)

- `../pic-platform/drivers/gpio/gpio.c`
- `../pic-platform/drivers/uart/uart.c`
- `../pic-platform/drivers/adc/adc.c`

## Configure Include Paths

In project properties, set include search paths to:

- `../pic-platform`
- `../pic-platform/core`
- `../pic-platform/drivers`

These paths allow includes like:

```c
#include "core/compiler.h"
#include "drivers/uart/uart.h"
```

## Compiler Compatibility (XC8 + C18)

- The library supports:
- `MPLAB XC8`
- `MPLAB C18`

- Compiler selection is done in MPLAB project configuration, not by runtime/code switching.
- `core/compiler.h` handles compiler abstraction automatically.

## Compiler Abstraction Requirements

- For XC8 builds, `__XC8` must be defined by toolchain.
- For C18 builds, `__18CXX` must be defined by toolchain.
- Do not add custom code to force compiler mode.
- Do not attempt to switch compiler inside source code.

## Build Checklist

- Required driver `.c` files are present in **Source Files**.
- No `.h` files were added to **Source Files**.
- Include paths use relative paths only.
- Project toolchain is set to XC8 or C18.
- Build succeeds without unresolved symbol/header errors.

## Minimal Example

```c
#include "core/compiler.h"
#include "drivers/uart/uart.h"

void main(void)
{
    uart_init(9600u);
    uart_write_string("MPLAB integration OK\r\n");
    while (1) { }
}
```

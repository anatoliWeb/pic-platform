# onewire

## Purpose

Bit-banged 1-Wire base driver over GPIO.

## Location

```text
drivers/communication/onewire/onewire.h
drivers/communication/onewire/onewire.c
drivers/communication/onewire/example.c
XC8/drivers/communication/onewire/onewire.c
C18/drivers/communication/onewire/onewire.c
docs/drivers/communication/onewire.md
docs/drivers/communication/onewire.ua.md
examples-projects/xc8/onewire_bus_test.X
examples-projects/xc8/ds18b20.X
examples-projects/xc8/ds18b20_search_rom.X
examples-projects/hex/xc8/onewire/onewire.hex
examples-projects/proteus/onewire/README.md
```

## Use when

- you need a shared 1-Wire bus;
- you need ROM search or addressed device access;
- you need the base driver for DS18B20 or other Dallas/Maxim parts.

## Do not use when

- you want a higher-level sensor wrapper instead of bus primitives;
- you need non-bit-banged hardware offload.

## Public API

| Function/type/macro | Exact signature or type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `onewire_timing_t` | struct | timing profile | reset/read/write timings | struct | none | profile-dependent tuning |
| `onewire_init` | `void onewire_init(volatile uint8_t* port, volatile uint8_t* tris, uint8_t pin);` | bind bus pin | port, tris, bit | none | configures state | single-wire line |
| `onewire_set_timing` | `void onewire_set_timing(const onewire_timing_t* timing);` | set timing profile | timing pointer | none | updates timing |
| `onewire_use_standard_timing` | `void onewire_use_standard_timing(void);` | standard timing preset | none | none | updates timing | |
| `onewire_use_proteus_pic18f452_timing` | `void onewire_use_proteus_pic18f452_timing(void);` | validated Proteus preset | none | none | updates timing | |
| `onewire_use_proteus_pic18f452_8mhz_timing` | `void onewire_use_proteus_pic18f452_8mhz_timing(void);` | compatibility alias | none | none | updates timing | legacy alias |
| `onewire_reset` | `uint8_t onewire_reset(void);` | reset/presence detect | none | presence flag | toggles bus | |
| `onewire_write_bit` | `void onewire_write_bit(uint8_t bit);` | write one bit | bit | none | bus transaction | |
| `onewire_read_bit` | `uint8_t onewire_read_bit(void);` | read one bit | none | bit value | bus transaction | |
| `onewire_write_byte` | `void onewire_write_byte(uint8_t byte);` | write one byte | byte | none | bus transaction | |
| `onewire_read_byte` | `uint8_t onewire_read_byte(void);` | read one byte | none | byte value | bus transaction | |
| `onewire_skip_rom` | `void onewire_skip_rom(void);` | broadcast command | none | none | bus transaction | |
| `onewire_match_rom` | `void onewire_match_rom(uint8_t* rom);` | addressed command | rom[8] | none | bus transaction | |
| `onewire_read_rom` | `void onewire_read_rom(uint8_t* rom);` | read ROM code | rom buffer | none | bus transaction | |
| `onewire_search_rom` | `uint8_t onewire_search_rom(uint8_t (*roms)[8], uint8_t max_devices);` | enumerate bus devices | ROM array, max count | count | bus transaction | multi-drop search |
| `onewire_crc8` | `uint8_t onewire_crc8(const uint8_t* data, uint8_t len);` | Dallas CRC8 | data, len | CRC byte | none | helper |

## Configuration structures

| Field | Type | Required | Meaning | Valid range | Notes |
| --- | --- | --- | --- | --- | --- |
| `onewire_timing_t.reset_low_us` | `uint16_t` | yes for custom timing | reset low pulse | profile-specific |  |
| `onewire_timing_t.presence_sample_us` | `uint16_t` | yes | sample presence window | profile-specific |  |
| `onewire_timing_t.reset_finish_us` | `uint16_t` | yes | finish reset slot | profile-specific |  |
| `onewire_timing_t.write_1_low_us` | `uint16_t` | yes | write-1 low time | profile-specific |  |
| `onewire_timing_t.write_1_release_us` | `uint16_t` | yes | write-1 release time | profile-specific |  |
| `onewire_timing_t.write_0_low_us` | `uint16_t` | yes | write-0 low time | profile-specific |  |
| `onewire_timing_t.write_0_release_us` | `uint16_t` | yes | write-0 release time | profile-specific |  |
| `onewire_timing_t.read_low_us` | `uint16_t` | yes | read slot low pulse | profile-specific |  |
| `onewire_timing_t.read_sample_us` | `uint16_t` | yes | read sample time | profile-specific |  |
| `onewire_timing_t.read_finish_us` | `uint16_t` | yes | read slot finish | profile-specific |  |

## Error/status model

| Status/error | Meaning | Typical cause | Required handling |
| --- | --- | --- | --- |
| `uint8_t` presence flag | presence detected | device on bus | continue bus command flow |
| `0/1` bit/byte returns | bus data | timing/bus state | validate before higher-level use |

## Source inclusion strategy

### Shared implementation

```text
drivers/communication/onewire/onewire.c
```

### XC8 implementation/wrapper

```text
XC8/drivers/communication/onewire/onewire.c
```

### C18 implementation/wrapper

```text
C18/drivers/communication/onewire/onewire.c
```

### Select exactly one route

```text
Choose either the shared source or the compiler wrapper route.
In this repo the wrappers include the shared source, so do not compile both the shared .c and the wrapper .c together.
```

## Required files

### Core/library files

```text
drivers/communication/onewire/onewire.h
drivers/communication/onewire/onewire.c
drivers/communication/onewire/example.c
```

### XC8 wrapper files

```text
XC8/drivers/communication/onewire/onewire.c
```

### C18 wrapper files

```text
C18/drivers/communication/onewire/onewire.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
core/delay.h
core/interrupts.h
```

### Callback-provided dependencies

```text
none
```

### Optional feature dependencies

```text
none
```

### Example-only dependencies

```text
drivers/communication/uart/uart.h
libraries/system/uart_debug/uart_debug.h
```

### Transitive dependencies

```text
core/bit_utils.h
```

## Compile-time defines

| Define | Default | Where defined | Effect | Required files | Resource impact | Code-size impact |
| --- | ---: | --- | --- | --- | --- | --- |
| none module-specific | n/a | n/a | timing preset chosen at runtime or via helper | shared/wrapper source | bus pin ownership | small |

## Runtime model

- Bit-banged 1-Wire base driver.
- Timing preset must match the board clock and hardware behavior.

## ISR requirements

```text
none, but timing-sensitive bus access should avoid unexpected interrupt jitter
```

## Resource ownership

- One GPIO pin and its TRIS register.
- Shared 1-Wire bus line.

## Integration recipe

1. Add `drivers/communication/onewire/onewire.h` and `onewire.c` or the compiler wrapper route used by your project.
2. Bind the bus pin in the project.
3. Choose a timing preset.
4. Use `onewire_reset()` before ROM commands.
5. Use `onewire_search_rom()` for enumeration or `onewire_read_rom()` for a single device.

## Minimal usable example

```c
uint8_t rom[8];

onewire_init(&PORTB, &TRISB, 1u);
onewire_use_proteus_pic18f452_timing();

if (onewire_reset() != 0u)
{
    onewire_read_rom(rom);
}
```

## Working examples

```text
drivers/communication/onewire/example.c
examples-projects/xc8/onewire_bus_test.X
examples-projects/xc8/ds18b20.X
examples-projects/xc8/ds18b20_search_rom.X
```

## Proteus integration

```text
examples-projects/proteus/onewire/README.md
```

## HEX artifacts

```text
examples-projects/hex/xc8/onewire/onewire.hex
```

## Human documentation

```text
docs/drivers/communication/onewire.md
docs/drivers/communication/onewire.ua.md
```

## Known limitations

- Timing-sensitive bit-bang protocol.
- The validated Proteus preset uses tuned timing values.

## Extension points

- Add timing presets or alternative pin handling inside the reusable driver, not in projects.

## Common mistakes

- Compiling both shared source and wrapper source together.
- Mixing the wrong timing preset with a board clock.

## AI decision rule

Use this driver for base 1-Wire bus access. Use `ds18b20` for the sensor-specific layer on top.

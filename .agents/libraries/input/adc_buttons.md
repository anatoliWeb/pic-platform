# adc_buttons

## Purpose

ADC ladder button driver for a single analog input.

## Location

```text
libraries/input/adc_buttons/adc_buttons.h
libraries/input/adc_buttons/adc_buttons.c
libraries/input/adc_buttons/example.c
XC8/libraries/input/adc_buttons/adc_buttons.c
C18/libraries/input/adc_buttons/adc_buttons.c
docs/libraries/input/adc_buttons.md
docs/libraries/input/adc_buttons.ua.md
examples-projects/xc8/adc_buttons.X
examples-projects/proteus/adc_buttons/README.md
examples-projects/hex/xc8/adc_buttons/adc_buttons.hex
examples-projects/hex/c18/adc_buttons/adc_buttons.hex
```

## Use when

- you have several buttons on one ADC ladder;
- you need simple stable button IDs with click/hold flags;
- you need a low-pin-count input panel.

## Do not use when

- the hardware does not use a resistor ladder;
- you need a more sophisticated matrix scanner.

## Public API

| Function/type/macro | Exact signature or type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `ADC_BUTTONS_MAX` | macro | max entries | none | value | none | 8 |
| `ADC_BUTTONS_NO_BUTTON` | macro | no button marker | none | value | none | 0 |
| `adc_button_range_t` | struct | one ladder range | min/max/id | state | none | caller table |
| `adc_buttons_t` | struct | input state | channel/table/count/state/times | state | none | stores click/hold flags |
| `adc_buttons_init` | `void adc_buttons_init(adc_buttons_t* btns, uint8_t channel, const adc_button_range_t* table, uint8_t table_size);` | bind channel and ranges | btns, ADC channel, range table, size | none | configures internal state | |
| `adc_buttons_update` | `void adc_buttons_update(adc_buttons_t* btns);` | sample and debounce | btns | none | may update flags | call periodically |
| `adc_buttons_get` | `uint8_t adc_buttons_get(adc_buttons_t* btns);` | current button id | btns | button id | none | stable current button |
| `adc_buttons_is_clicked` | `uint8_t adc_buttons_is_clicked(adc_buttons_t* btns);` | click event | btns | flag | consumes flag | |
| `adc_buttons_is_held` | `uint8_t adc_buttons_is_held(adc_buttons_t* btns);` | hold event | btns | flag | consumes flag | |

## Configuration structures

| Field | Type | Required | Meaning | Valid range | Notes |
| --- | --- | --- | --- | --- | --- |
| `adc_button_range_t.min` | `uint16_t` | yes | minimum ADC value | 0..1023 or project range | inclusive |
| `adc_button_range_t.max` | `uint16_t` | yes | maximum ADC value | 0..1023 or project range | inclusive |
| `adc_button_range_t.id` | `uint8_t` | yes | logical button id | caller-defined |  |
| `adc_buttons_t.channel` | `uint8_t` | yes | ADC channel | valid ADC channel |  |
| `adc_buttons_t.table` | pointer | yes | range table | non-null | caller-owned |
| `adc_buttons_t.table_size` | `uint8_t` | yes | number of ranges | up to `ADC_BUTTONS_MAX` |  |

## Error/status model

```text
No explicit status returns; invalid inputs are ignored in implementation.
```

## Source inclusion strategy

### Shared implementation

```text
libraries/input/adc_buttons/adc_buttons.c
```

### XC8 implementation/wrapper

```text
XC8/libraries/input/adc_buttons/adc_buttons.c
```

### C18 implementation/wrapper

```text
C18/libraries/input/adc_buttons/adc_buttons.c
```

### Select exactly one route

```text
Compile either the shared source or the wrapper source for the selected compiler, not both.
The wrapper .c includes the shared source in this repository.
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
drivers/analog/adc/adc.h
drivers/timers/tick/tick.h
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
uart, uart_debug, LEDs in example project
```

### Transitive dependencies

```text
none
```

## Compile-time defines

| Define | Default | Where defined | Effect | Required files | Resource impact | Code-size impact |
| --- | ---: | --- | --- | --- | --- | --- |
| `ADC_BUTTONS_MAX` | `8u` | header | max range entries | shared/wrapper source | table size | small |
| `ADC_BUTTONS_NO_BUTTON` | `0u` | header | marker value for no button | shared/wrapper source | none | none |

## Runtime model

- Caller provides range table.
- The module maps ADC ranges to logical ids and debounces stable changes.

## ISR requirements

```text
none directly; periodic update driven from main loop or tick-based scheduling
```

## Resource ownership

- One ADC channel.
- Tick dependency via Timer1.

## Integration recipe

1. Add the shared or wrapper source route for the selected compiler.
2. Configure the ADC channel and range table.
3. Call `adc_buttons_update()` periodically.
4. Read `adc_buttons_get()` and event flags.

## Minimal usable example

```c
adc_buttons_t btns;
adc_buttons_init(&btns, 0u, table, table_size);
adc_buttons_update(&btns);
```

## Working examples

```text
libraries/input/adc_buttons/example.c
examples-projects/xc8/adc_buttons.X
```

## Proteus integration

```text
examples-projects/proteus/adc_buttons/README.md
```

## HEX artifacts

```text
examples-projects/hex/xc8/adc_buttons/adc_buttons.hex
examples-projects/hex/c18/adc_buttons/adc_buttons.hex
```

## Human documentation

```text
docs/libraries/input/adc_buttons.md
docs/libraries/input/adc_buttons.ua.md
```

## Known limitations

- Threshold ranges are caller-managed.

## Extension points

- Keep menu logic outside the driver; use it as an input source.

## AI decision rule

Use this driver for resistor-ladder buttons. Do not replace it with project-local ADC range scanning.

# crc

## Purpose

CRC and checksum helpers used by protocol and sensor code.

## Location

```text
core/crc/crc.h
core/crc/crc.c
core/crc/example.c
```

## Public API

| Function/type/macro | Exact signature or type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `crc8_dallas` | `uint8_t crc8_dallas(const uint8_t* data, uint16_t len);` | Dallas/Maxim CRC-8 | data, length | CRC byte | none | used by DS18B20 / 1-Wire |
| `crc16_modbus` | `uint16_t crc16_modbus(const uint8_t* data, uint16_t len);` | Modbus CRC-16 | data, length | CRC word | none | generic |
| `checksum8` | `uint8_t checksum8(const uint8_t* data, uint16_t len);` | simple checksum | data, length | sum byte | none | generic |

## Source inclusion strategy

### Shared implementation

```text
core/crc/crc.c
```

### XC8 implementation/wrapper

```text
none
```

### C18 implementation/wrapper

```text
none
```

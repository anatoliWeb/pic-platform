[Ukrainian version](./porting.ua.md)

# Porting Guide

## What To Copy
- `core/`
- Required folders from `drivers/`
- Optional C18/XC8 overrides

## Steps
1. Select target MCU in project
2. Configure `DRV_XTAL_FREQ`
3. Add include paths
4. Build and validate examples

## Checklist
- No hardcoded board-specific pins
- Config bits match target board
- UART/I2C timing matches oscillator



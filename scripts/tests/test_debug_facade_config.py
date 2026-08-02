from __future__ import annotations

import unittest
import xml.etree.ElementTree as ET
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]


def read_text(relative_path: str) -> str:
    return (ROOT / relative_path).read_text(encoding="utf-8-sig")


def read_bytes(relative_path: str) -> bytes:
    return (ROOT / relative_path).read_bytes()


def makefile_sources(relative_path: str) -> list[str]:
    for line in read_text(relative_path).splitlines():
        if line.startswith("SOURCEFILES="):
            return line.split("=", 1)[1].split()
    raise AssertionError(f"SOURCEFILES not found in {relative_path}")


def config_sources(relative_path: str) -> list[str]:
    xml_text = read_text(relative_path)
    root = ET.fromstring(xml_text)
    source_folder = root.find(".//logicalFolder[@name='SourceFiles']")
    if source_folder is None:
        raise AssertionError(f"SourceFiles folder not found in {relative_path}")
    return [item.text for item in source_folder.iter("itemPath") if item.text]


class DebugFacadeConfigTests(unittest.TestCase):
    def test_core_debug_header_is_uart_free(self) -> None:
        text = read_text("core/debug.h")
        self.assertIn('#include "libraries/system/debug/debug.h"', text)
        self.assertNotIn('uart_debug.h', text)
        self.assertIn('#define DBG_PRINT(str)         debug_write((str))', text)
        self.assertIn('#define DRV_DEBUG_INIT()       debug_init()', text)

    def test_debug_header_validates_booleans_and_ranges(self) -> None:
        text = read_text("libraries/system/debug/debug.h")
        for marker in (
            'DRV_DEBUG_ENABLE must be 0 or 1',
            'DRV_DEBUG_BACKEND_UART must be 0 or 1',
            'DRV_DEBUG_BACKEND_DISPLAY must be 0 or 1',
            'DRV_DEBUG_BACKEND_PINS must be 0 or 1',
            'DRV_DEBUG_DISPLAY_TYPE_LCD_2X16 must be 0 or 1',
            'DRV_DEBUG_DISPLAY_INTERFACE_I2C must be 0 or 1',
            'DRV_DEBUG_DISPLAY_INTERFACE_PARALLEL must be 0 or 1',
            'DRV_DEBUG_DISPLAY_AUTO_INIT must be 0 or 1',
            'DRV_DEBUG_PINS_INTERFACE_GPIO must be 0 or 1',
            'DRV_DEBUG_PINS_INTERFACE_I2C must be 0 or 1',
            'DRV_DEBUG_PINS_START_BIT must be between 0 and 7',
            'DRV_DEBUG_PINS_START_BIT + DRV_DEBUG_PINS_CHANNEL_COUNT must not exceed 8',
            'DRV_DEBUG_PINS_PULSE_US must fit in uint16_t',
        ):
            self.assertIn(marker, text)

    def test_noop_macros_do_not_evaluate_arguments(self) -> None:
        text = read_text("libraries/system/debug/debug.h")
        self.assertNotIn('(void)(s)', text)
        for marker in (
            '#define debug_error(s)  do { } while (0)',
            '#define debug_warn(s)   do { } while (0)',
            '#define debug_info(s)   do { } while (0)',
            '#define debug_trace(s)  do { } while (0)',
        ):
            self.assertIn(marker, text)

    def test_display_auto_init_is_gated(self) -> None:
        display_backend = read_text("libraries/system/debug/debug_backend_display.c")
        lcd_adapter = read_text("libraries/system/debug/debug_display_lcd_2x16.c")
        self.assertRegex(display_backend, r"void debug_display_init\(void\)\s*\{\s*#if DRV_DEBUG_DISPLAY_AUTO_INIT")
        self.assertIn('debug_lcd_clear();', display_backend)
        self.assertIn('display_reset_cursor();', display_backend)
        self.assertIn('#if DRV_DEBUG_DISPLAY_AUTO_INIT', lcd_adapter)
        self.assertIn('i2c_init((uint32_t)DRV_DEBUG_DISPLAY_I2C_FREQ);', lcd_adapter)
        self.assertIn('lcd_init();', lcd_adapter)
        self.assertIn('lcd_hd44780_init();', lcd_adapter)

    def test_debug_write_i16_handles_int16_min(self) -> None:
        text = read_text("libraries/system/debug/debug.c")
        self.assertIn('debug_write_unsigned((uint16_t)(0u - (uint16_t)v));', text)
        self.assertNotIn('-(int16_t)v', text)

    def test_example_source_lists_are_exact(self) -> None:
        expected = {
            'examples-projects/xc8/debug_default_uart.X/nbproject/Makefile-default.mk': [
                'main.c',
                'config_bits.c',
                '../../../core/delay.c',
                '../../../drivers/communication/uart/uart.c',
                '../../../libraries/system/debug/debug.c',
            ],
            'examples-projects/xc8/debug_display_i2c.X/nbproject/Makefile-default.mk': [
                'main.c',
                'config_bits.c',
                '../../../core/delay.c',
                '../../../drivers/communication/i2c/i2c.c',
                '../../../libraries/system/debug/debug.c',
            ],
            'examples-projects/xc8/debug_display_parallel.X/nbproject/Makefile-default.mk': [
                'main.c',
                'config_bits.c',
                '../../../core/delay.c',
                '../../../drivers/gpio/gpio.c',
                '../../../libraries/display/lcd_hd44780/lcd.c',
                '../../../libraries/system/debug/debug.c',
            ],
            'examples-projects/xc8/debug_pins_gpio.X/nbproject/Makefile-default.mk': [
                'main.c',
                'config_bits.c',
                '../../../core/delay.c',
                '../../../drivers/gpio/gpio.c',
                '../../../libraries/system/debug/debug.c',
            ],
            'examples-projects/xc8/debug_multi_backend.X/nbproject/Makefile-default.mk': [
                'main.c',
                'config_bits.c',
                '../../../core/delay.c',
                '../../../drivers/communication/uart/uart.c',
                '../../../drivers/communication/i2c/i2c.c',
                '../../../drivers/gpio/gpio.c',
                '../../../libraries/system/debug/debug.c',
            ],
            'examples-projects/xc8/debug_disabled.X/nbproject/Makefile-default.mk': [
                'main.c',
                'config_bits.c',
                '../../../core/delay.c',
            ],
        }

        for path, expected_sources in expected.items():
            with self.subTest(path=path):
                self.assertEqual(makefile_sources(path), expected_sources)

    def test_configuration_source_trees_are_exact(self) -> None:
        expected = {
            'examples-projects/xc8/debug_default_uart.X/nbproject/configurations.xml': [
                'main.c',
                'config_bits.c',
                '../../../core/delay.c',
                '../../../drivers/communication/uart/uart.c',
                '../../../libraries/system/debug/debug.c',
            ],
            'examples-projects/xc8/debug_display_i2c.X/nbproject/configurations.xml': [
                'main.c',
                'config_bits.c',
                '../../../core/delay.c',
                '../../../drivers/communication/i2c/i2c.c',
                '../../../libraries/system/debug/debug.c',
            ],
            'examples-projects/xc8/debug_display_parallel.X/nbproject/configurations.xml': [
                'main.c',
                'config_bits.c',
                '../../../core/delay.c',
                '../../../drivers/gpio/gpio.c',
                '../../../libraries/display/lcd_hd44780/lcd.c',
                '../../../libraries/system/debug/debug.c',
            ],
            'examples-projects/xc8/debug_pins_gpio.X/nbproject/configurations.xml': [
                'main.c',
                'config_bits.c',
                '../../../core/delay.c',
                '../../../drivers/gpio/gpio.c',
                '../../../libraries/system/debug/debug.c',
            ],
            'examples-projects/xc8/debug_multi_backend.X/nbproject/configurations.xml': [
                'main.c',
                'config_bits.c',
                '../../../core/delay.c',
                '../../../drivers/communication/uart/uart.c',
                '../../../drivers/communication/i2c/i2c.c',
                '../../../drivers/gpio/gpio.c',
                '../../../libraries/system/debug/debug.c',
            ],
            'examples-projects/xc8/debug_disabled.X/nbproject/configurations.xml': [
                'main.c',
                'config_bits.c',
                '../../../core/delay.c',
            ],
        }

        for path, expected_sources in expected.items():
            with self.subTest(path=path):
                self.assertEqual(config_sources(path), expected_sources)

    def test_readmes_use_real_make_command(self) -> None:
        for path in (
            'examples-projects/xc8/debug_default_uart.X/README.md',
            'examples-projects/xc8/debug_default_uart.X/README.ua.md',
            'examples-projects/xc8/debug_display_i2c.X/README.md',
            'examples-projects/xc8/debug_display_i2c.X/README.ua.md',
            'examples-projects/xc8/debug_display_parallel.X/README.md',
            'examples-projects/xc8/debug_display_parallel.X/README.ua.md',
            'examples-projects/xc8/debug_pins_gpio.X/README.md',
            'examples-projects/xc8/debug_pins_gpio.X/README.ua.md',
            'examples-projects/xc8/debug_multi_backend.X/README.md',
            'examples-projects/xc8/debug_multi_backend.X/README.ua.md',
            'examples-projects/xc8/debug_disabled.X/README.md',
            'examples-projects/xc8/debug_disabled.X/README.ua.md',
        ):
            with self.subTest(path=path):
                text = read_text(path)
                self.assertIn('make.exe" -f nbproject\\Makefile-default.mk SUBPROJECTS= .build-conf', text)
                self.assertIn('make.exe" -f nbproject\\Makefile-default.mk SUBPROJECTS= .clean-conf', text)
                self.assertNotIn('mplab-make', text)

    def test_text_files_end_with_newline(self) -> None:
        paths = [
            'core/debug.h',
            'libraries/system/debug/debug.h',
            'libraries/system/debug/debug.c',
            'libraries/system/debug/debug_backend_display.c',
            'libraries/system/debug/debug_display_lcd_2x16.c',
            'libraries/system/debug/debug_backend_pins.c',
            'libraries/system/debug/debug_pins_gpio.c',
            'examples-projects/xc8/debug_default_uart.X/README.md',
            'examples-projects/xc8/debug_default_uart.X/README.ua.md',
            'examples-projects/xc8/debug_display_i2c.X/README.md',
            'examples-projects/xc8/debug_display_i2c.X/README.ua.md',
            'examples-projects/xc8/debug_display_parallel.X/README.md',
            'examples-projects/xc8/debug_display_parallel.X/README.ua.md',
            'examples-projects/xc8/debug_pins_gpio.X/README.md',
            'examples-projects/xc8/debug_pins_gpio.X/README.ua.md',
            'examples-projects/xc8/debug_multi_backend.X/README.md',
            'examples-projects/xc8/debug_multi_backend.X/README.ua.md',
            'examples-projects/xc8/debug_disabled.X/README.md',
            'examples-projects/xc8/debug_disabled.X/README.ua.md',
            'examples-projects/xc8/debug_default_uart.X/nbproject/configurations.xml',
            'examples-projects/xc8/debug_display_i2c.X/nbproject/configurations.xml',
            'examples-projects/xc8/debug_display_parallel.X/nbproject/configurations.xml',
            'examples-projects/xc8/debug_pins_gpio.X/nbproject/configurations.xml',
            'examples-projects/xc8/debug_multi_backend.X/nbproject/configurations.xml',
            'examples-projects/xc8/debug_disabled.X/nbproject/configurations.xml',
            'examples-projects/xc8/debug_default_uart.X/nbproject/Makefile-default.mk',
            'examples-projects/xc8/debug_display_i2c.X/nbproject/Makefile-default.mk',
            'examples-projects/xc8/debug_display_parallel.X/nbproject/Makefile-default.mk',
            'examples-projects/xc8/debug_pins_gpio.X/nbproject/Makefile-default.mk',
            'examples-projects/xc8/debug_multi_backend.X/nbproject/Makefile-default.mk',
            'examples-projects/xc8/debug_disabled.X/nbproject/Makefile-default.mk',
            'examples-projects/xc8/debug_default_uart.X/nbproject/Makefile-variables.mk',
            'examples-projects/xc8/debug_display_i2c.X/nbproject/Makefile-variables.mk',
            'examples-projects/xc8/debug_display_parallel.X/nbproject/Makefile-variables.mk',
            'examples-projects/xc8/debug_pins_gpio.X/nbproject/Makefile-variables.mk',
            'examples-projects/xc8/debug_multi_backend.X/nbproject/Makefile-variables.mk',
            'examples-projects/xc8/debug_disabled.X/nbproject/Makefile-variables.mk',
        ]

        for path in paths:
            with self.subTest(path=path):
                self.assertTrue(read_bytes(path).endswith(b"\n"), path)


if __name__ == "__main__":
    unittest.main()

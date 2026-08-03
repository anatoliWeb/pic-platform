from __future__ import annotations

import unittest
import xml.etree.ElementTree as ET
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]

LCD_HD44780 = ROOT / "libraries" / "display" / "lcd_hd44780"
LCD_I2C_H = LCD_HD44780 / "lcd_i2c.h"
LCD_I2C_C = LCD_HD44780 / "lcd_i2c.c"
ADAPTER_C = ROOT / "libraries" / "system" / "debug" / "debug_display_lcd_2x16.c"


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8-sig")


def read_bytes(path: Path) -> bytes:
    return path.read_bytes()


def config_sources(relative_path: str) -> list[str]:
    root = ET.fromstring((ROOT / relative_path).read_text(encoding="utf-8-sig"))
    source_folder = root.find(".//logicalFolder[@name='SourceFiles']")
    if source_folder is None:
        raise AssertionError(f"SourceFiles folder not found in {relative_path}")
    return [item.text for item in source_folder.iter("itemPath") if item.text]


def define_macros(relative_path: str) -> list[str]:
    root = ET.fromstring((ROOT / relative_path).read_text(encoding="utf-8-sig"))
    for prop in root.iter("property"):
        if prop.get("key") == "define-macros":
            value = prop.get("value", "")
            return [m for m in value.split(";") if m]
    raise AssertionError(f"define-macros not found in {relative_path}")


class LcdI2cHeaderTests(unittest.TestCase):
    def test_include_guard(self) -> None:
        text = read_text(LCD_I2C_H)
        self.assertIn("LIBRARIES_DISPLAY_LCD_HD44780_LCD_I2C_H", text)

    def test_mapping_defines_are_configurable(self) -> None:
        text = read_text(LCD_I2C_H)
        self.assertIn("LCD_I2C_PIN_RS", text)
        self.assertIn("LCD_I2C_PIN_RW", text)
        self.assertIn("LCD_I2C_PIN_EN", text)
        self.assertIn("LCD_I2C_PIN_BL", text)
        self.assertIn("LCD_I2C_DATA_SHIFT", text)
        self.assertIn("0x01u", text)
        self.assertIn("0x02u", text)
        self.assertIn("0x04u", text)
        self.assertIn("0x08u", text)

    def test_status_enum_exists(self) -> None:
        text = read_text(LCD_I2C_H)
        self.assertIn("lcd_i2c_status_t", text)
        for member in (
            "LCD_I2C_OK",
            "LCD_I2C_INVALID_ARGUMENT",
            "LCD_I2C_NOT_INITIALIZED",
            "LCD_I2C_NO_ACK",
        ):
            self.assertIn(member, text)

    def test_status_api_declared(self) -> None:
        text = read_text(LCD_I2C_H)
        for declaration in (
            "lcd_i2c_status_t lcd_i2c_init(uint8_t i2c_addr, uint32_t i2c_clock_hz);",
            "lcd_i2c_status_t lcd_i2c_attach(uint8_t i2c_addr);",
            "lcd_i2c_status_t lcd_i2c_controller_init(uint8_t i2c_addr);",
            "lcd_i2c_status_t lcd_i2c_probe(uint8_t i2c_addr);",
            "lcd_i2c_status_t lcd_i2c_last_status(void);",
            "uint8_t lcd_i2c_is_ready(void);",
        ):
            self.assertIn(declaration, text)

    def test_display_operations_declared_void(self) -> None:
        text = read_text(LCD_I2C_H)
        for declaration in (
            "void lcd_i2c_clear(void);",
            "void lcd_i2c_home(void);",
            "void lcd_i2c_set_cursor(uint8_t row, uint8_t col);",
            "void lcd_i2c_write_char(char c);",
            "void lcd_i2c_write_string(const char* str);",
            "void lcd_i2c_backlight(uint8_t on);",
        ):
            self.assertIn(declaration, text)


class LcdI2cImplementationTests(unittest.TestCase):
    def test_address_is_7bit_without_masking(self) -> None:
        text = read_text(LCD_I2C_C)
        self.assertNotIn("& 0x7F", text)
        self.assertNotIn("& (uint8_t)0x7F", text)
        self.assertIn("> 0x7Fu", text)

    def test_nack_is_not_ignored(self) -> None:
        text = read_text(LCD_I2C_C)
        self.assertIn("nack = i2c_write_byte((uint8_t)(g_i2c_addr << 1u));", text)
        self.assertIn("g_ready = 0u;", text)
        self.assertIn("LCD_I2C_NO_ACK", text)

    def test_compile_time_mapping_validation(self) -> None:
        text = read_text(LCD_I2C_C)
        for marker in (
            "LCD_I2C_PIN_RS must be non-zero",
            "LCD_I2C_PIN_RW must be non-zero",
            "LCD_I2C_PIN_EN must be non-zero",
            "LCD_I2C_PIN_BL must be non-zero",
            "LCD_I2C_PIN_RS overlaps another control pin",
            "LCD_I2C_PIN_RW overlaps another control pin",
            "LCD_I2C_PIN_EN overlaps another control pin",
            "LCD_I2C_PIN_BL overlaps another control pin",
            "LCD_I2C_DATA_SHIFT + 4 must not exceed 8",
            "LCD_I2C data pins overlap the control pins",
            "LCD_I2C control pins must fit in 8 bits",
        ):
            self.assertIn(marker, text)

    def test_controller_init_kept_named_for_debug_adapter(self) -> None:
        text = read_text(LCD_I2C_C)
        self.assertIn("void lcd_hd44780_init(void)", text)

    def test_backlight_does_not_send_before_init(self) -> None:
        text = read_text(LCD_I2C_C)
        self.assertIn("g_i2c_addr == 0u", text)
        self.assertIn("LCD_I2C_NOT_INITIALIZED", text)

    def test_backlight_argument_boundary_checked(self) -> None:
        text = read_text(LCD_I2C_C)
        self.assertIn("(on != 0u) && (on != 1u)", text)
        self.assertIn("LCD_I2C_INVALID_ARGUMENT", text)

    def test_write_string_null_is_safe(self) -> None:
        text = read_text(LCD_I2C_C)
        self.assertIn("str == (const char*)0", text)

    def test_row_does_not_wrap_silently(self) -> None:
        text = read_text(LCD_I2C_C)
        self.assertIn("if (row > 1u)", text)

    def test_no_heap_or_framebuffer(self) -> None:
        for path in (LCD_I2C_H, LCD_I2C_C):
            text = read_text(path)
            self.assertNotIn("malloc", text)
            self.assertNotIn("framebuffer", text)
            self.assertNotIn("calloc", text)

    def test_state_is_static_module_state(self) -> None:
        text = read_text(LCD_I2C_C)
        self.assertIn("static uint8_t g_i2c_addr", text)
        self.assertIn("static uint8_t g_ready", text)
        self.assertIn("static lcd_i2c_status_t g_last_status", text)


class DebugAdapterTests(unittest.TestCase):
    def test_adapter_stays_thin(self) -> None:
        text = read_text(ADAPTER_C)
        self.assertIn("lcd_i2c_init((uint8_t)DRV_DEBUG_DISPLAY_I2C_ADDR, (uint32_t)DRV_DEBUG_DISPLAY_I2C_FREQ);", text)
        self.assertNotIn("i2c_start(", text)
        self.assertNotIn("i2c_write_byte(", text)
        self.assertNotIn("i2c_stop(", text)
        self.assertNotIn("lcd_hd44780_init", text)


class ProjectDefineMacrosTests(unittest.TestCase):
    def test_display_i2c_project_has_full_debug_defines(self) -> None:
        macros = define_macros("examples-projects/xc8/debug/display_i2c.X/nbproject/configurations.xml")
        joined = ";".join(macros)
        for required in (
            "PIC_PLATFORM_CLOCK_HZ=8000000UL",
            "DRV_DEBUG_ENABLE=1",
            "DRV_DEBUG_BACKEND_UART=0",
            "DRV_DEBUG_BACKEND_DISPLAY=1",
            "DRV_DEBUG_BACKEND_PINS=0",
            "DRV_DEBUG_DISPLAY_TYPE_LCD_2X16=1",
            "DRV_DEBUG_DISPLAY_INTERFACE_I2C=1",
            "DRV_DEBUG_DISPLAY_INTERFACE_PARALLEL=0",
            "DRV_DEBUG_DISPLAY_AUTO_INIT=1",
            "DRV_DEBUG_DISPLAY_I2C_ADDR=0x27u",
            "DRV_DEBUG_DISPLAY_I2C_FREQ=100000UL",
        ):
            self.assertIn(required, joined)

    def test_multi_backend_project_has_full_defines(self) -> None:
        macros = define_macros("examples-projects/xc8/debug/multi_backend.X/nbproject/configurations.xml")
        joined = ";".join(macros)
        for required in (
            "PIC_PLATFORM_CLOCK_HZ=8000000UL",
            "DRV_DEBUG_BACKEND_UART=1",
            "DRV_DEBUG_BACKEND_DISPLAY=1",
            "DRV_DEBUG_BACKEND_PINS=1",
            "DRV_DEBUG_DISPLAY_INTERFACE_I2C=1",
            "DRV_DEBUG_DISPLAY_I2C_ADDR=0x27u",
            "DRV_DEBUG_PINS_INTERFACE_GPIO=1",
            "DRV_DEBUG_PINS_PORT=PORTC",
            "DRV_DEBUG_PINS_TRIS=TRISC",
            "DRV_DEBUG_PINS_START_BIT=0u",
            "DRV_DEBUG_PINS_CHANNEL_COUNT=4u",
        ):
            self.assertIn(required, joined)

    def test_direct_project_has_clock_define(self) -> None:
        macros = define_macros(
            "examples-projects/xc8/display/lcd_hd44780/i2c_pcf8574.X/nbproject/configurations.xml"
        )
        self.assertEqual(macros, ["PIC_PLATFORM_CLOCK_HZ=8000000UL"])

    def test_direct_project_sources_are_exact(self) -> None:
        sources = config_sources(
            "examples-projects/xc8/display/lcd_hd44780/i2c_pcf8574.X/nbproject/configurations.xml"
        )
        self.assertEqual(
            sources,
            [
                "main.c",
                "config_bits.c",
                "../../../../../core/delay.c",
                "../../../../../drivers/communication/i2c/i2c.c",
                "../../../../../libraries/display/lcd_hd44780/lcd_i2c.c",
            ],
        )

    def test_text_files_end_with_newline(self) -> None:
        for path in (
            LCD_I2C_H,
            LCD_I2C_C,
            ROOT / "examples-projects" / "xc8" / "display" / "lcd_hd44780" / "i2c_pcf8574.X" / "main.c",
            ROOT / "examples-projects" / "xc8" / "display" / "lcd_hd44780" / "i2c_pcf8574.X" / "config_bits.c",
            ROOT / "examples-projects" / "xc8" / "display" / "lcd_hd44780" / "i2c_pcf8574.X" / "project_config.h",
            ROOT / "examples-projects" / "xc8" / "display" / "lcd_hd44780" / "i2c_pcf8574.X" / "README.md",
            ROOT / "examples-projects" / "xc8" / "display" / "lcd_hd44780" / "i2c_pcf8574.X" / "README.ua.md",
            ROOT / "examples-projects" / "xc8" / "display" / "lcd_hd44780" / "i2c_pcf8574.X" / "nbproject" / "project.xml",
            ROOT / "examples-projects" / "xc8" / "display" / "lcd_hd44780" / "i2c_pcf8574.X" / "nbproject" / "configurations.xml",
        ):
            with self.subTest(path=str(path.relative_to(ROOT))):
                self.assertTrue(read_bytes(path).endswith(b"\n"))


if __name__ == "__main__":
    unittest.main()

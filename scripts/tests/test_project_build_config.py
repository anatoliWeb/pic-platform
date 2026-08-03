from __future__ import annotations

import re
import unittest
import xml.etree.ElementTree as ET
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]

CATEGORY_A = {
    "PIC_PLATFORM_CLOCK_HZ",
    "DRV_DEBUG_ENABLE",
    "DRV_DEBUG_BACKEND_UART",
    "DRV_DEBUG_BACKEND_DISPLAY",
    "DRV_DEBUG_BACKEND_PINS",
    "DRV_DEBUG_LEVEL",
    "DRV_DEBUG_DISPLAY_TYPE_LCD_2X16",
    "DRV_DEBUG_DISPLAY_INTERFACE_I2C",
    "DRV_DEBUG_DISPLAY_INTERFACE_PARALLEL",
    "DRV_DEBUG_DISPLAY_AUTO_INIT",
    "DRV_DEBUG_DISPLAY_I2C_ADDR",
    "DRV_DEBUG_DISPLAY_I2C_FREQ",
    "DRV_DEBUG_PINS_INTERFACE_GPIO",
    "DRV_DEBUG_PINS_INTERFACE_I2C",
    "DRV_DEBUG_PINS_PORT",
    "DRV_DEBUG_PINS_TRIS",
    "DRV_DEBUG_PINS_START_BIT",
    "DRV_DEBUG_PINS_CHANNEL_COUNT",
    "DRV_DEBUG_PINS_PULSE_US",
    "LCD_I2C_PIN_RS",
    "LCD_I2C_PIN_RW",
    "LCD_I2C_PIN_EN",
    "LCD_I2C_PIN_BL",
    "LCD_I2C_DATA_SHIFT",
    "SEVEN_SEGMENT_ENABLE_TIMER0",
    "SEVEN_SEGMENT_ENABLE_TIMER1",
    "SEVEN_SEGMENT_ENABLE_TIMER2",
    "SEVEN_SEGMENT_ENABLE_TIMER3",
    "POSITION_DRIVE_SENSOR_TYPE",
    "POSITION_DRIVE_ENABLE_PWM",
    "POSITION_DRIVE_ENABLE_TIMEOUT",
    "POSITION_DRIVE_ENABLE_STUCK_DETECTION",
    "POSITION_DRIVE_ENABLE_DIRECTION_CHECK",
    "POSITION_DRIVE_ENABLE_UART_DEBUG",
}

PROJECTS = {
    "default_uart": ROOT / "examples-projects" / "xc8" / "debug" / "default_uart.X",
    "display_i2c": ROOT / "examples-projects" / "xc8" / "debug" / "display_i2c.X",
    "display_parallel": ROOT / "examples-projects" / "xc8" / "debug" / "display_parallel.X",
    "pins_gpio": ROOT / "examples-projects" / "xc8" / "debug" / "pins_gpio.X",
    "multi_backend": ROOT / "examples-projects" / "xc8" / "debug" / "multi_backend.X",
    "disabled": ROOT / "examples-projects" / "xc8" / "debug" / "disabled.X",
    "direct_lcd_i2c": ROOT / "examples-projects" / "xc8" / "display" / "lcd_hd44780" / "i2c_pcf8574.X",
}


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8-sig")


def parse_define_lines(text: str) -> dict[str, str]:
    defines: dict[str, str] = {}
    pattern = re.compile(r"^\s*#define\s+([A-Za-z_][A-Za-z0-9_]*)\s+(.+?)\s*(?:/\*.*)?$")
    for line in text.splitlines():
        match = pattern.match(line)
        if match is None:
            continue
        defines[match.group(1)] = match.group(2).strip()
    return defines


def parse_pragma_config(path: Path) -> dict[str, str]:
    values: dict[str, str] = {}
    pattern = re.compile(r"^\s*#pragma\s+config\s+([A-Za-z0-9_]+)\s*=\s*([A-Za-z0-9_xXuUlL]+)\s*$")
    for line in read_text(path).splitlines():
        match = pattern.match(line)
        if match is not None:
            values[match.group(1)] = match.group(2)
    return values


def xml_define_macros(project_dir: Path) -> dict[str, str]:
    root = ET.fromstring(read_text(project_dir / "nbproject" / "configurations.xml"))
    for prop in root.iter("property"):
        if prop.get("key") == "define-macros":
            value = prop.get("value", "")
            macros: dict[str, str] = {}
            for item in value.split(";"):
                if not item:
                    continue
                name, _, macro_value = item.partition("=")
                macros[name] = macro_value
            return macros
    raise AssertionError(f"define-macros not found in {project_dir}")


def xml_property(project_dir: Path, key: str) -> str:
    root = ET.fromstring(read_text(project_dir / "nbproject" / "configurations.xml"))
    for prop in root.iter("property"):
        if prop.get("key") == key:
            return prop.get("value", "")
    raise AssertionError(f"{key} not found in {project_dir}")


def explicit_project_defines(project_dir: Path) -> dict[str, str]:
    return parse_define_lines(read_text(project_dir / "project_config.h"))


class ProjectBuildConfigTests(unittest.TestCase):
    def test_category_a_values_live_in_define_macros(self) -> None:
        for name, project in PROJECTS.items():
            with self.subTest(project=name):
                project_defines = explicit_project_defines(project)
                xml_defines = xml_define_macros(project)
                for macro in CATEGORY_A:
                    self.assertNotIn(macro, project_defines)
                if name == "direct_lcd_i2c":
                    self.assertNotIn("DRV_DEBUG_", " ".join(xml_defines))

    def test_required_library_defines_are_present(self) -> None:
        display_i2c = xml_define_macros(PROJECTS["display_i2c"])
        multi_backend = xml_define_macros(PROJECTS["multi_backend"])
        pins_gpio = xml_define_macros(PROJECTS["pins_gpio"])
        direct_lcd = xml_define_macros(PROJECTS["direct_lcd_i2c"])

        for macro, value in {
            "DRV_DEBUG_ENABLE": "1",
            "DRV_DEBUG_BACKEND_DISPLAY": "1",
            "DRV_DEBUG_DISPLAY_TYPE_LCD_2X16": "1",
            "DRV_DEBUG_DISPLAY_INTERFACE_I2C": "1",
            "DRV_DEBUG_DISPLAY_AUTO_INIT": "1",
            "DRV_DEBUG_DISPLAY_I2C_ADDR": "0x27u",
            "DRV_DEBUG_DISPLAY_I2C_FREQ": "100000UL",
        }.items():
            self.assertEqual(display_i2c.get(macro), value)

        for macro, value in {
            "DRV_DEBUG_ENABLE": "1",
            "DRV_DEBUG_BACKEND_UART": "1",
            "DRV_DEBUG_BACKEND_DISPLAY": "1",
            "DRV_DEBUG_BACKEND_PINS": "1",
            "DRV_DEBUG_DISPLAY_INTERFACE_I2C": "1",
            "DRV_DEBUG_PINS_INTERFACE_GPIO": "1",
            "DRV_DEBUG_PINS_PORT": "PORTC",
            "DRV_DEBUG_PINS_TRIS": "TRISC",
            "DRV_DEBUG_PINS_START_BIT": "0u",
            "DRV_DEBUG_PINS_CHANNEL_COUNT": "4u",
        }.items():
            self.assertEqual(multi_backend.get(macro), value)

        for macro, value in {
            "DRV_DEBUG_ENABLE": "1",
            "DRV_DEBUG_BACKEND_UART": "0",
            "DRV_DEBUG_BACKEND_DISPLAY": "0",
            "DRV_DEBUG_BACKEND_PINS": "1",
            "DRV_DEBUG_PINS_INTERFACE_GPIO": "1",
            "DRV_DEBUG_PINS_PORT": "PORTC",
            "DRV_DEBUG_PINS_TRIS": "TRISC",
        }.items():
            self.assertEqual(pins_gpio.get(macro), value)

        self.assertEqual(direct_lcd, {"PIC_PLATFORM_CLOCK_HZ": "8000000UL"})

    def test_direct_lcd_example_has_no_debug_macros(self) -> None:
        xml_defines = xml_define_macros(PROJECTS["direct_lcd_i2c"])
        project_defines = explicit_project_defines(PROJECTS["direct_lcd_i2c"])
        self.assertFalse(any(name.startswith("DRV_DEBUG_") for name in xml_defines))
        self.assertFalse(any(name.startswith("DRV_DEBUG_") for name in project_defines))

    def test_default_uart_has_no_explicit_backend_selection(self) -> None:
        xml_defines = xml_define_macros(PROJECTS["default_uart"])
        project_defines = explicit_project_defines(PROJECTS["default_uart"])
        self.assertEqual(xml_defines, {"PIC_PLATFORM_CLOCK_HZ": "8000000UL"})
        self.assertFalse(any(name.startswith("DRV_DEBUG_BACKEND_") for name in project_defines))

    def test_disabled_example_disables_debug_layer(self) -> None:
        xml_defines = xml_define_macros(PROJECTS["disabled"])
        self.assertEqual(xml_defines.get("DRV_DEBUG_ENABLE"), "0")

    def test_clock_is_consistent_across_target_examples(self) -> None:
        for name in PROJECTS:
            with self.subTest(project=name):
                self.assertEqual(xml_define_macros(PROJECTS[name]).get("PIC_PLATFORM_CLOCK_HZ"), "8000000UL")

    def test_project_config_has_no_category_a_duplicates(self) -> None:
        for name, project in PROJECTS.items():
            with self.subTest(project=name):
                project_defines = explicit_project_defines(project)
                for macro in CATEGORY_A:
                    self.assertNotIn(macro, project_defines)

    def test_config_bits_helper_flags_match_pragmas(self) -> None:
        template = parse_define_lines(read_text(ROOT / "core" / "config" / "project_config_template.h"))
        config_bits = parse_pragma_config(PROJECTS["direct_lcd_i2c"] / "config_bits.c")
        expected = {
            "PIC_PLATFORM_WDT_ENABLED": "1" if config_bits["WDT"] == "ON" else "0",
            "PIC_PLATFORM_LVP_ENABLED": "1" if config_bits["LVP"] == "ON" else "0",
            "PIC_PLATFORM_BOR_ENABLED": "1" if config_bits["BOR"] == "ON" else "0",
            "PIC_PLATFORM_CCP2MUX_ENABLED": "1" if config_bits["CCP2MUX"] == "ON" else "0",
        }
        for macro, value in expected.items():
            self.assertEqual(template.get(macro), value)

    def test_direct_lcd_include_paths_reach_platform_roots(self) -> None:
        includes = xml_property(PROJECTS["direct_lcd_i2c"], "extra-include-directories")
        self.assertIn("..\\..\\..\\..\\..\\..;", includes)
        self.assertIn("..\\..\\..\\..\\..\\..\\core", includes)
        self.assertIn("..\\..\\..\\..\\..\\..\\drivers", includes)
        self.assertIn("..\\..\\..\\..\\..\\..\\libraries", includes)

    def test_docs_cover_configuration_contract(self) -> None:
        english = read_text(ROOT / "docs" / "configuration.md")
        ukrainian = read_text(ROOT / "docs" / "configuration.ua.md")
        for text in (english, ukrainian):
            self.assertIn("Category A", text)
            self.assertIn("Category B", text)
            self.assertIn("Category C", text)
            self.assertIn("one source of truth", text.lower())


if __name__ == "__main__":
    unittest.main()

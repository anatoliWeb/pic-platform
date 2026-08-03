from __future__ import annotations

import unittest
import xml.etree.ElementTree as ET
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]

EXAMPLE_DIR = ROOT / "examples-projects" / "xc8"

MAIN_EXAMPLES = ("debug/default_uart.X", "debug/display_i2c.X", "debug/pins_gpio.X")
ALL_EXAMPLES = (
    "debug/default_uart.X",
    "debug/display_i2c.X",
    "debug/display_parallel.X",
    "debug/pins_gpio.X",
    "debug/multi_backend.X",
    "debug/disabled.X",
)


def read_text(project: str, name: str) -> str:
    return (EXAMPLE_DIR / project / name).read_text(encoding="utf-8-sig")


def read_bytes(project: str, name: str) -> bytes:
    return (EXAMPLE_DIR / project / name).read_bytes()


def has_path(project: str, needle: str) -> bool:
    project_dir = EXAMPLE_DIR / project
    ignored = {"build", "dist", "nbproject", "__pycache__"}
    for line in project_dir.glob("**/*"):
        if not line.is_file():
            continue
        if line.relative_to(project_dir).parts[0] in ignored:
            continue
        if line.name.endswith((".c", ".h", ".xml", ".md")):
            try:
                if needle in line.read_text(encoding="utf-8-sig", errors="ignore"):
                    return True
            except (UnicodeDecodeError, IsADirectoryError):
                continue
    return False


def config_sources(project: str) -> list[str]:
    return config_sources_from_file(EXAMPLE_DIR / project / "nbproject" / "configurations.xml")


def config_sources_from_file(xml_path: Path) -> list[str]:
    root = ET.fromstring(xml_path.read_text(encoding="utf-8-sig"))
    source_folder = root.find(".//logicalFolder[@name='SourceFiles']")
    if source_folder is None:
        raise AssertionError(f"SourceFiles folder not found in {xml_path}")
    return [item.text for item in source_folder.iter("itemPath") if item.text]


class DebugExamplesTests(unittest.TestCase):
    def test_main_examples_exist(self) -> None:
        for project in MAIN_EXAMPLES:
            with self.subTest(project=project):
                dirs = [p.name for p in EXAMPLE_DIR.iterdir() if p.is_dir()]
                self.assertIn(project, dirs)

    def test_main_examples_have_en_and_ua_readmes(self) -> None:
        for project in MAIN_EXAMPLES:
            with self.subTest(project=project):
                self.assertTrue((EXAMPLE_DIR / project / "README.md").is_file())
                self.assertTrue((EXAMPLE_DIR / project / "README.ua.md").is_file())

    def test_backend_defines_are_selected(self) -> None:
        text = read_text("debug/default_uart.X", "project_config.h")
        self.assertNotIn("DRV_DEBUG_BACKEND_UART 0", text)

        display = read_text("debug/display_i2c.X", "project_config.h")
        self.assertIn("DRV_DEBUG_BACKEND_UART 0", display)
        self.assertIn("DRV_DEBUG_BACKEND_DISPLAY 1", display)
        self.assertIn("DRV_DEBUG_BACKEND_PINS 0", display)
        self.assertIn("DRV_DEBUG_DISPLAY_INTERFACE_I2C 1", display)

        pins = read_text("debug/pins_gpio.X", "project_config.h")
        self.assertIn("DRV_DEBUG_BACKEND_UART 0", pins)
        self.assertIn("DRV_DEBUG_BACKEND_DISPLAY 0", pins)
        self.assertIn("DRV_DEBUG_BACKEND_PINS 1", pins)
        self.assertIn("DRV_DEBUG_PINS_INTERFACE_GPIO 1", pins)

    def test_uart_example_has_no_display_or_pins_sources(self) -> None:
        sources = config_sources("debug/default_uart.X")
        self.assertIn("../../../../drivers/communication/uart/uart.c", sources)
        self.assertFalse(any("i2c/i2c.c" in s for s in sources))
        self.assertFalse(any("gpio/gpio.c" in s for s in sources))
        self.assertFalse(any("lcd_hd44780/lcd.c" in s for s in sources))

    def test_display_i2c_example_has_no_uart_or_parallel_sources(self) -> None:
        sources = config_sources("debug/display_i2c.X")
        self.assertIn("../../../../drivers/communication/i2c/i2c.c", sources)
        self.assertFalse(any("uart/uart.c" in s for s in sources))
        self.assertFalse(any("lcd_hd44780/lcd.c" in s for s in sources))

    def test_pins_example_has_no_uart_i2c_or_lcd_sources(self) -> None:
        sources = config_sources("debug/pins_gpio.X")
        self.assertIn("../../../../drivers/gpio/gpio.c", sources)
        self.assertFalse(any("uart/uart.c" in s for s in sources))
        self.assertFalse(any("i2c/i2c.c" in s for s in sources))
        self.assertFalse(any("lcd_hd44780/lcd.c" in s for s in sources))

    def test_disabled_example_has_no_debug_or_backend_sources(self) -> None:
        sources = config_sources("debug/disabled.X")
        self.assertEqual(
            sources,
            ["main.c", "config_bits.c", "../../../../core/delay.c"],
        )
        self.assertNotIn("libraries/system/debug/debug.c", sources)

    def test_project_identity_has_no_stale_uart_debug(self) -> None:
        tracked = {
            "config_bits.c",
            "main.c",
            "project_config.h",
            "README.md",
            "README.ua.md",
        }
        for project in ALL_EXAMPLES:
            with self.subTest(project=project):
                self.assertFalse(has_path(project, "uart_debug.X"))

    def test_all_main_readmes_cover_required_topics(self) -> None:
        self.assertIn("9600", read_text("debug/default_uart.X", "README.md"))
        self.assertIn("RC6", read_text("debug/default_uart.X", "README.md"))

        display = read_text("debug/display_i2c.X", "README.md")
        self.assertIn("0x27", display.lower())
        self.assertIn("SCL", display)
        self.assertIn("SDA", display)

        pins = read_text("debug/pins_gpio.X", "README.md")
        self.assertIn("330", pins)
        self.assertIn("1000", pins)
        self.assertIn("PORTC", pins)


if __name__ == "__main__":
    unittest.main()
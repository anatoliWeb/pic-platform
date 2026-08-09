from __future__ import annotations

import sys
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from config_contract import CATEGORY_A

DOCS_EN = ROOT / "docs" / "configuration.md"
DOCS_UA = ROOT / "docs" / "configuration.ua.md"
CONFIG_RULES = ROOT / ".agents" / "10-rules" / "CONFIGURATION-RULES.md"


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


class ConfigContractMacroCatalogTests(unittest.TestCase):
    def test_layout_and_api_profile_macros_are_category_a(self) -> None:
        required = {
            "LCD_I2C_MINIMAL",
            "TACHOMETER_LIGHTWEIGHT",
            "CRON_MAX_TASKS",
            "LED_ANIM_ENGINE_MAX_ANIMATIONS",
            "SCHEDULER_MAX_TASKS",
            "AC_PHASE_CONTROL_MAX_CHANNELS",
            "RB_OVERWRITE",
            "DRV_USE_FREERTOS",
            "DRV_USE_UART",
            "DRV_DEBUG_UART_BAUD",
            "_XTAL_FREQ",
            "DRV_XTAL_FREQ",
        }
        missing = required - set(CATEGORY_A)
        self.assertEqual(
            missing,
            set(),
            "Layout/API/profile macros must be project-wide Category A defines",
        )

    def test_every_category_a_macro_documented_in_both_languages(self) -> None:
        english = read_text(DOCS_EN)
        ukrainian = read_text(DOCS_UA)
        for macro in sorted(CATEGORY_A):
            with self.subTest(macro=macro):
                self.assertIn(f"`{macro}`", english, f"Missing {macro} in EN docs")
                self.assertIn(f"`{macro}`", ukrainian, f"Missing {macro} in UA docs")

    def test_configuration_rules_reference_category_a_contract(self) -> None:
        text = read_text(CONFIG_RULES)
        self.assertIn("Category A", text)
        self.assertIn("compiler `-D`", text)
        self.assertIn("define-macros", text)

    def test_lcd_i2c_header_uses_project_wide_minimal_wording(self) -> None:
        header = read_text(ROOT / "libraries" / "display" / "lcd_hd44780" / "lcd_i2c.h")
        self.assertNotIn("define LCD_I2C_MINIMAL=1 before including", header)
        self.assertIn("-DLCD_I2C_MINIMAL=1", header)
        self.assertIn("identical in every translation unit", header)

    def test_seven_segment_source_uses_project_wide_backend_wording(self) -> None:
        source = read_text(ROOT / "libraries" / "display" / "seven_segment" / "seven_segment.c")
        self.assertNotIn("#define SEVEN_SEGMENT_ENABLE_TIMER2 1", source)
        self.assertIn("-DSEVEN_SEGMENT_ENABLE_TIMER2=1", source)


if __name__ == "__main__":
    unittest.main()

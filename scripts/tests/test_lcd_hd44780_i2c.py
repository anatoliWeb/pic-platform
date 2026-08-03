from __future__ import annotations

import subprocess
import sys
import tempfile
import unittest
import xml.etree.ElementTree as ET
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]

LCD_HD44780 = ROOT / "libraries" / "display" / "lcd_hd44780"
LCD_I2C_H = LCD_HD44780 / "lcd_i2c.h"
LCD_I2C_C = LCD_HD44780 / "lcd_i2c.c"
ADAPTER_C = ROOT / "libraries" / "system" / "debug" / "debug_display_lcd_2x16.c"
BACKEND_C = ROOT / "libraries" / "system" / "debug" / "debug_backend_display.c"
FIXTURES = ROOT / "scripts" / "tests" / "fixtures"
HARNESS = FIXTURES / "lcd_i2c_harness.c"
HARNESS_OLD_SIG = FIXTURES / "lcd_i2c_harness_old_sig.c"

XC8 = Path(r"C:\Tools\bin\xc8.bat")
DFP = Path(r"C:\Program Files\Microchip\MPLABX\v6.30\packs\Microchip\PIC18Fxxxx_DFP\1.7.171\xc8")
MCU = "18F452"

XTAL_DEFINE = "PIC_PLATFORM_CLOCK_HZ=8000000UL"

_preprocessed_cache: str | None = None


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


def run_xc8(args: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [str(XC8), *args],
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
    )


def _build_dir() -> Path:
    out_dir = Path(tempfile.gettempdir()) / "lcd_i2c_test_builds"
    out_dir.mkdir(parents=True, exist_ok=True)
    return out_dir


def _stage(source: Path, out_dir: Path) -> Path:
    """Copy the fixture so XC8 intermediate files never land in the repo."""
    staged = out_dir / source.name
    staged.write_bytes(source.read_bytes())
    return staged


def compile_harness(extra_defines: list[str], source: Path = HARNESS) -> subprocess.CompletedProcess[str]:
    out_dir = _build_dir()
    staged = _stage(source, out_dir)
    args = [
        f"-mcpu={MCU}",
        f"-mdfp={DFP}",
        f"-I{ROOT}",
        f"-D{XTAL_DEFINE}",
        "-O0",
        *extra_defines,
        str(staged),
        "-o",
        str(out_dir / "harness_out.hex"),
    ]
    return run_xc8(args)


def preprocess_harness() -> str:
    global _preprocessed_cache
    if _preprocessed_cache is None:
        out_dir = _build_dir()
        staged = _stage(HARNESS, out_dir)
        args = [f"-mcpu={MCU}", f"-mdfp={DFP}", f"-I{ROOT}", f"-D{XTAL_DEFINE}", "-E", str(staged)]
        result = run_xc8(args)
        if result.returncode != 0:
            raise AssertionError(f"XC8 -E failed:\n{result.stderr}")
        _preprocessed_cache = result.stdout
    return _preprocessed_cache


def extract_function(text: str, signature: str) -> str:
    """Extract a function body (balanced braces) by definition signature."""
    idx = text.find(signature)
    while idx != -1:
        rest = text[idx + len(signature):].lstrip()
        if rest.startswith("{"):
            break
        idx = text.find(signature, idx + 1)
    else:
        raise AssertionError(f"function definition {signature!r} not found in preprocessed output")

    open_idx = idx + len(signature) + text[idx + len(signature):].find("{")
    depth = 0
    i = open_idx
    while i < len(text):
        if text[i] == "{":
            depth += 1
        elif text[i] == "}":
            depth -= 1
            if depth == 0:
                return text[open_idx + 1:i]
        i += 1
    raise AssertionError(f"unbalanced braces for {signature!r}")


def extract_attach() -> str:
    return extract_function(preprocess_harness(), "lcd_i2c_attach(uint8_t i2c_addr)")


def extract_controller_init() -> str:
    return extract_function(preprocess_harness(), "lcd_i2c_controller_init(void)")


def extract_init() -> str:
    return extract_function(preprocess_harness(), "lcd_i2c_init(uint8_t i2c_addr, uint32_t i2c_clock_hz)")


def extract_probe() -> str:
    return extract_function(preprocess_harness(), "lcd_i2c_probe(uint8_t i2c_addr)")


def extract_init_sequence() -> str:
    return extract_function(preprocess_harness(), "lcd_hd44780_init_sequence(void)")


def extract_send() -> str:
    return extract_function(preprocess_harness(), "lcd_i2c_send(uint8_t data)")


def extract_public_op(signature: str) -> str:
    return extract_function(preprocess_harness(), signature)


class LcdI2cHeaderTests(unittest.TestCase):
    def test_include_guard(self) -> None:
        self.assertIn("LIBRARIES_DISPLAY_LCD_HD44780_LCD_I2C_H", read_text(LCD_I2C_H))

    def test_mapping_defines_are_configurable(self) -> None:
        text = read_text(LCD_I2C_H)
        for macro in ("LCD_I2C_PIN_RS", "LCD_I2C_PIN_RW", "LCD_I2C_PIN_EN", "LCD_I2C_PIN_BL", "LCD_I2C_DATA_SHIFT"):
            self.assertIn(macro, text)

    def test_status_enum_exists(self) -> None:
        text = read_text(LCD_I2C_H)
        for member in (
            "LCD_I2C_OK",
            "LCD_I2C_INVALID_ARGUMENT",
            "LCD_I2C_NOT_INITIALIZED",
            "LCD_I2C_NO_ACK",
        ):
            self.assertIn(member, text)

    def test_controller_init_takes_no_arguments(self) -> None:
        self.assertIn("lcd_i2c_status_t lcd_i2c_controller_init(void);", read_text(LCD_I2C_H))

    def test_status_api_declared(self) -> None:
        text = read_text(LCD_I2C_H)
        for declaration in (
            "lcd_i2c_status_t lcd_i2c_init(uint8_t i2c_addr, uint32_t i2c_clock_hz);",
            "lcd_i2c_status_t lcd_i2c_attach(uint8_t i2c_addr);",
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


@unittest.skipUnless(XC8.is_file(), "XC8 toolchain not available")
@unittest.skipUnless(DFP.is_dir(), "PIC18 DFP not available")
class LcdI2cCompileFixtureTests(unittest.TestCase):
    def test_valid_configuration_compiles(self) -> None:
        result = compile_harness([])
        self.assertEqual(result.returncode, 0, msg=result.stdout + result.stderr)

    def test_removed_controller_init_signature_is_rejected(self) -> None:
        result = compile_harness([], source=HARNESS_OLD_SIG)
        self.assertNotEqual(result.returncode, 0, msg="old controller_init(addr) must not compile")

    def test_multi_bit_control_mask_rejected(self) -> None:
        result = compile_harness(["-DLCD_I2C_PIN_RS=0x03u"])
        self.assertNotEqual(result.returncode, 0, msg="multi-bit RS mask must fail the build")
        self.assertIn("LCD_I2C_PIN_RS must be a single-bit mask", read_text(LCD_I2C_C))

    def test_zero_control_mask_rejected(self) -> None:
        result = compile_harness(["-DLCD_I2C_PIN_RW=0u"])
        self.assertNotEqual(result.returncode, 0, msg="zero RW mask must fail the build")
        self.assertIn("LCD_I2C_PIN_RW must be non-zero", read_text(LCD_I2C_C))

    def test_overlapping_control_pins_rejected(self) -> None:
        result = compile_harness(["-DLCD_I2C_PIN_RW=0x01u"])
        self.assertNotEqual(result.returncode, 0, msg="overlapping RW mask must fail the build")
        self.assertIn("LCD_I2C_PIN_RW overlaps another control pin", read_text(LCD_I2C_C))

    def test_data_shift_out_of_range_rejected(self) -> None:
        result = compile_harness(["-DLCD_I2C_DATA_SHIFT=5u"])
        self.assertNotEqual(result.returncode, 0, msg="data shift out of range must fail the build")
        self.assertIn("LCD_I2C_DATA_SHIFT + 4 must not exceed 8", read_text(LCD_I2C_C))

    def test_control_pins_above_8_bits_rejected(self) -> None:
        result = compile_harness(["-DLCD_I2C_PIN_RS=0x100u"])
        self.assertNotEqual(result.returncode, 0, msg="control pins above 8 bits must fail the build")
        self.assertIn("LCD_I2C control pins must fit in 8 bits", read_text(LCD_I2C_C))


@unittest.skipUnless(XC8.is_file(), "XC8 toolchain not available")
@unittest.skipUnless(DFP.is_dir(), "PIC18 DFP not available")
class LcdI2cPreprocessedBehaviorTests(unittest.TestCase):
    """Structural behavior checks on the preprocessed translation unit."""

    def test_attach_binds_without_controller_init(self) -> None:
        body = extract_attach()
        self.assertIn("lcd_i2c_probe(", body)
        self.assertNotIn("lcd_hd44780_init_sequence", body)
        self.assertNotIn("lcd_i2c_controller_init", body)
        self.assertNotIn("0x01u", body)

    def test_attach_never_calls_i2c_init(self) -> None:
        body = extract_attach()
        self.assertNotIn("i2c_init(", body)

    def test_controller_init_requires_bound_address(self) -> None:
        body = extract_controller_init()
        self.assertIn("g_i2c_addr == 0u", body)
        self.assertIn("LCD_I2C_NOT_INITIALIZED", body)

    def test_controller_init_runs_init_sequence(self) -> None:
        body = extract_controller_init()
        self.assertIn("lcd_hd44780_init_sequence(", body)

    def test_init_rejects_zero_address(self) -> None:
        body = extract_init()
        self.assertIn("(i2c_addr == 0u)", body)
        self.assertIn("LCD_I2C_INVALID_ARGUMENT", body)

    def test_probe_rejects_zero_and_overflow_address(self) -> None:
        body = extract_probe()
        self.assertIn("(i2c_addr == 0u)", body)
        self.assertIn("i2c_addr > 0x7Fu", body)

    def test_nack_short_circuits_init_sequence(self) -> None:
        body = extract_init_sequence()
        self.assertIn("lcd_i2c_byte(0x01u, 0u)", body)
        self.assertGreaterEqual(body.count("if (status != LCD_I2C_OK)"), 7, msg="sequence must short-circuit on NACK")
        self.assertIn("return status;", body)

    def test_send_reports_nack_and_clears_ready(self) -> None:
        body = extract_send()
        self.assertIn("LCD_I2C_NO_ACK", body)
        self.assertIn("g_ready = 0u", body)
        self.assertIn("return LCD_I2C_NO_ACK", body)

    def test_public_operations_gate_on_ready(self) -> None:
        for signature in (
            "void lcd_i2c_clear(void)",
            "void lcd_i2c_home(void)",
            "void lcd_i2c_set_cursor(uint8_t row, uint8_t col)",
            "void lcd_i2c_write_char(char c)",
            "void lcd_i2c_write_string(const char* str)",
            "void lcd_i2c_backlight(uint8_t on)",
        ):
            with self.subTest(signature=signature):
                self.assertIn("lcd_i2c_check_ready(", extract_public_op(signature))

    def test_write_string_is_null_safe(self) -> None:
        self.assertIn("str == (const char*)0", extract_public_op("void lcd_i2c_write_string(const char* str)"))

    def test_set_cursor_rejects_invalid_rows(self) -> None:
        self.assertIn("row > 1u", extract_public_op("void lcd_i2c_set_cursor(uint8_t row, uint8_t col)"))

    def test_address_is_7bit_without_masking(self) -> None:
        body = extract_send()
        self.assertNotIn("& 0x7F", body)
        self.assertIn("g_i2c_addr << 1u", body)


class DebugAdapterReadyTests(unittest.TestCase):
    def test_adapter_stays_thin(self) -> None:
        text = read_text(ADAPTER_C)
        self.assertNotIn("i2c_start(", text)
        self.assertNotIn("i2c_write_byte(", text)
        self.assertNotIn("i2c_stop(", text)
        self.assertNotIn("lcd_hd44780_init", text)

    def test_adapter_tracks_ready_from_init(self) -> None:
        text = read_text(ADAPTER_C)
        self.assertIn("g_debug_lcd_ready", text)
        self.assertIn("uint8_t debug_lcd_is_ready(void)", read_text(ADAPTER_C.parent / "debug_display_lcd_2x16.h"))
        self.assertIn("lcd_i2c_init((uint8_t)DRV_DEBUG_DISPLAY_I2C_ADDR, (uint32_t)DRV_DEBUG_DISPLAY_I2C_FREQ)", text)
        self.assertIn("LCD_I2C_OK", text)

    def test_display_backend_skips_writes_when_not_ready(self) -> None:
        text = read_text(BACKEND_C)
        self.assertIn("debug_lcd_is_ready()", text)


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
            HARNESS,
            HARNESS_OLD_SIG,
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
    unittest.main(verbosity=2)

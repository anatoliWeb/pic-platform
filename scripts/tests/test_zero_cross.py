from __future__ import annotations

import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]

LIB = ROOT / "libraries" / "input" / "zero_cross"
HDR = LIB / "zero_cross.h"
SRC = LIB / "zero_cross.c"
FIXTURES = ROOT / "scripts" / "tests" / "fixtures"
HARNESS = FIXTURES / "zero_cross_harness.c"
EVENT_HARNESS = FIXTURES / "zero_cross_event_harness.c"

XC8 = shutil.which("xc8")

DFP = Path(r"C:\Program Files\Microchip\MPLABX\v6.30\packs\Microchip\PIC18Fxxxx_DFP\1.7.171\xc8")
MCU = "18F452"

CLOCK_DEFINE = "PIC_PLATFORM_CLOCK_HZ=10000000UL"


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8-sig")


def read_bytes(path: Path) -> bytes:
    return path.read_bytes()


def extract_source_function(text: str, signature: str) -> str:
    needle = signature.rstrip(" (").rstrip("(") + "("
    idx = 0
    while True:
        idx = text.find(needle, idx)
        if idx == -1:
            raise AssertionError(f"function definition {signature!r} not found")
        open_idx = idx + len(needle)
        depth = 1
        i = open_idx
        while i < len(text) and depth > 0:
            if text[i] == "(":
                depth += 1
            elif text[i] == ")":
                depth -= 1
            i += 1
        rest = text[i:].lstrip()
        if rest.startswith("{"):
            brace_idx = i + (len(text[i:]) - len(rest))
            body_depth = 0
            j = brace_idx
            while j < len(text):
                if text[j] == "{":
                    body_depth += 1
                elif text[j] == "}":
                    body_depth -= 1
                    if body_depth == 0:
                        return text[brace_idx + 1:j]
                j += 1
            raise AssertionError(f"unbalanced braces for {signature!r}")
        idx += len(needle)


def run_xc8(args: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [XC8, *args],
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
    )


def _build_dir() -> Path:
    out_dir = Path(tempfile.gettempdir()) / "zero_cross_test_builds"
    out_dir.mkdir(parents=True, exist_ok=True)
    return out_dir


def compile_harness(name: str, extra_args: list[str] | None = None) -> subprocess.CompletedProcess[str]:
    out_dir = _build_dir()
    args = [
        f"-mcpu={MCU}",
        f"-mdfp={DFP}",
        f"-I{ROOT}",
        f"-D{CLOCK_DEFINE}",
        "-O0",
        "-std=c99",
    ]
    if extra_args:
        args.extend(extra_args)
    args.extend([str(out_dir / name), "-o", str(out_dir / f"{name}_out.hex")])
    return run_xc8(args)


class ZeroCrossHeaderTests(unittest.TestCase):
    def test_include_guard(self) -> None:
        self.assertIn("LIBRARIES_INPUT_ZERO_CROSS_H", read_text(HDR))

    def test_status_enum(self) -> None:
        text = read_text(HDR)
        for member in (
            "ZERO_CROSS_STATUS_NOT_INITIALIZED",
            "ZERO_CROSS_STATUS_WAITING",
            "ZERO_CROSS_STATUS_ALIVE",
            "ZERO_CROSS_STATUS_LOST",
            "ZERO_CROSS_STATUS_CONFIG_ERROR",
        ):
            self.assertIn(member, text)

    def test_frequency_enum(self) -> None:
        text = read_text(HDR)
        for member in (
            "ZERO_CROSS_FREQUENCY_UNKNOWN",
            "ZERO_CROSS_FREQUENCY_50_HZ",
            "ZERO_CROSS_FREQUENCY_60_HZ",
        ):
            self.assertIn(member, text)

    def test_config_fields_declared(self) -> None:
        text = read_text(HDR)
        for field in (
            "timeout_ms",
            "min_half_cycle_us",
            "max_half_cycle_us",
            "glitch_reject_us",
            "recovery_event_count",
        ):
            self.assertIn(field, text)

    def test_event_fields_declared(self) -> None:
        text = read_text(HDR)
        for field in (
            "timestamp_us",
            "half_cycle_us",
            "sequence",
            "frequency",
        ):
            self.assertIn(field, text)

    def test_api_declared(self) -> None:
        text = read_text(HDR)
        for declaration in (
            "zero_cross_init(",
            "zero_cross_on_edge(",
            "zero_cross_process(",
            "zero_cross_is_alive(",
            "zero_cross_get_status(",
            "zero_cross_get_frequency(",
            "zero_cross_get_half_cycle_us(",
            "zero_cross_get_sequence(",
            "zero_cross_reset(",
        ):
            self.assertIn(declaration, text)

    def test_feature_blocks_default_on(self) -> None:
        text = read_text(HDR)
        for macro in (
            "ZERO_CROSS_ENABLE_TIMEOUT",
            "ZERO_CROSS_ENABLE_FREQUENCY_DETECTION",
            "ZERO_CROSS_ENABLE_GLITCH_FILTER",
        ):
            self.assertIn(macro, text)


class ZeroCrossBehaviorTests(unittest.TestCase):
    """Structural behavioral checks on the library source text."""

    def test_init_rejects_invalid_arguments(self) -> None:
        body = extract_source_function(read_text(SRC), "drv_status_t zero_cross_init(")
        for guard in (
            "zc == (zero_cross_t*)0",
            "config == (const zero_cross_config_t*)0",
        ):
            self.assertIn(guard, body)

    def test_init_rejects_invalid_config(self) -> None:
        body = extract_source_function(read_text(SRC), "drv_status_t zero_cross_init(")
        self.assertIn("config->recovery_event_count == 0u", body)
        self.assertIn("config->min_half_cycle_us >= config->max_half_cycle_us", body)
        self.assertIn("ZERO_CROSS_STATUS_CONFIG_ERROR", body)

    def test_init_arms_waiting_state(self) -> None:
        body = extract_source_function(read_text(SRC), "drv_status_t zero_cross_init(")
        self.assertIn("ZERO_CROSS_STATUS_WAITING", body)
        self.assertIn("zc->last_edge_us = 0UL", body)
        self.assertIn("zc->sequence = 0UL", body)
        self.assertIn("ZERO_CROSS_FREQUENCY_UNKNOWN", body)

    def test_first_edge_stays_waiting(self) -> None:
        body = extract_source_function(read_text(SRC), "uint8_t zero_cross_on_edge(")
        self.assertIn("zc->last_edge_us == 0UL", body)
        self.assertIn("ZERO_CROSS_STATUS_WAITING", body)
        self.assertIn("return 0u;", body)

    def test_second_edge_computes_half_cycle(self) -> None:
        body = extract_source_function(read_text(SRC), "uint8_t zero_cross_on_edge(")
        self.assertIn("half_cycle = now_us - zc->last_edge_us", body)
        self.assertIn("zc->half_cycle_us = (uint16_t)half_cycle", body)

    def test_half_cycle_bounds_validated(self) -> None:
        body = extract_source_function(read_text(SRC), "uint8_t zero_cross_on_edge(")
        self.assertIn("half_cycle < (uint32_t)zc->config.min_half_cycle_us", body)
        self.assertIn("half_cycle > (uint32_t)zc->config.max_half_cycle_us", body)

    def test_frequency_classification(self) -> None:
        body = extract_source_function(read_text(SRC), "uint8_t zero_cross_on_edge(")
        self.assertIn("ZERO_CROSS_FREQUENCY_50_HZ", body)
        self.assertIn("ZERO_CROSS_FREQUENCY_60_HZ", body)

    def test_sequence_only_for_valid_edges(self) -> None:
        body = extract_source_function(read_text(SRC), "uint8_t zero_cross_on_edge(")
        self.assertIn("zc->sequence++", body)
        self.assertIn("event->sequence = zc->sequence", body)

    def test_glitch_rejection_gated(self) -> None:
        body = extract_source_function(read_text(SRC), "uint8_t zero_cross_on_edge(")
        self.assertIn("config.glitch_reject_us != 0u", body)
        self.assertIn("(now_us - zc->last_edge_us) <", body)
        self.assertIn("(uint32_t)zc->config.glitch_reject_us", body)

    def test_timeout_transitions_lost(self) -> None:
        body = extract_source_function(read_text(SRC), "void zero_cross_process(")
        self.assertIn("zc->config.timeout_ms != 0u", body)
        self.assertIn("now_us - zc->last_edge_us", body)
        self.assertIn("ZERO_CROSS_STATUS_LOST", body)

    def test_recovery_needs_n_edges(self) -> None:
        body = extract_source_function(read_text(SRC), "uint8_t zero_cross_on_edge(")
        self.assertIn("zc->status == ZERO_CROSS_STATUS_LOST", body)
        self.assertIn("zc->recovery_count++", body)
        self.assertIn("zc->config.recovery_event_count", body)

    def test_reset_clears_state(self) -> None:
        body = extract_source_function(read_text(SRC), "void zero_cross_reset(")
        for reset in (
            "ZERO_CROSS_STATUS_WAITING",
            "zc->last_edge_us = 0UL",
            "zc->sequence = 0UL",
            "zc->half_cycle_us = 0u",
            "ZERO_CROSS_FREQUENCY_UNKNOWN",
        ):
            self.assertIn(reset, body)

    def test_status_api(self) -> None:
        text = read_text(SRC)
        body = extract_source_function(text, "uint8_t zero_cross_is_alive(")
        self.assertIn("zc->status == ZERO_CROSS_STATUS_ALIVE", body)
        status = extract_source_function(text, "zero_cross_status_t zero_cross_get_status(")
        self.assertIn("ZERO_CROSS_STATUS_NOT_INITIALIZED", status)

    def test_no_owner_dependencies(self) -> None:
        text = read_text(SRC)
        for forbidden in (
            "ac_phase_control",
            "LAT",
            "TRIS",
            "INT0",
            "timer2",
            "Timer2",
        ):
            self.assertNotIn(forbidden, text)


@unittest.skipUnless(XC8 is not None, "XC8 toolchain not available")
@unittest.skipUnless(DFP.is_dir(), "PIC18 DFP not available")
class ZeroCrossCompileTests(unittest.TestCase):
    def test_standalone_zero_cross_compiles(self) -> None:
        staged = _build_dir() / HARNESS.name
        staged.write_bytes(HARNESS.read_bytes())
        result = compile_harness(staged.name)
        self.assertEqual(result.returncode, 0, msg=result.stdout + result.stderr)

    def test_event_harness_with_ac_phase_compiles(self) -> None:
        staged = _build_dir() / EVENT_HARNESS.name
        staged.write_bytes(EVENT_HARNESS.read_bytes())
        result = compile_harness(staged.name)
        self.assertEqual(result.returncode, 0, msg=result.stdout + result.stderr)

    def test_optional_features_disabled_compile(self) -> None:
        staged = _build_dir() / HARNESS.name
        staged.write_bytes(HARNESS.read_bytes())
        result = compile_harness(
            staged.name,
            [
                "-DZERO_CROSS_DISABLE_TIMEOUT",
                "-DZERO_CROSS_DISABLE_FREQUENCY_DETECTION",
                "-DZERO_CROSS_DISABLE_GLITCH_FILTER",
            ],
        )
        self.assertEqual(result.returncode, 0, msg=result.stdout + result.stderr)

    def test_text_files_end_with_newline(self) -> None:
        for path in (HDR, SRC, HARNESS, EVENT_HARNESS):
            with self.subTest(path=str(path.relative_to(ROOT))):
                self.assertTrue(read_bytes(path).endswith(b"\n"))


if __name__ == "__main__":
    unittest.main(verbosity=2)
from __future__ import annotations

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
LIB = ROOT / "libraries" / "sensors" / "tachometer"
HDR = LIB / "tachometer.h"
SRC = LIB / "tachometer.c"


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8-sig")


def source_function(text: str, signature: str) -> str:
    needle = signature.rstrip(" (").rstrip("(") + "("
    idx = text.find(needle)
    if idx == -1:
        raise AssertionError(f"function definition {signature!r} not found")

    depth = 1
    i = idx + len(needle)
    while i < len(text) and depth > 0:
        if text[i] == "(":
            depth += 1
        elif text[i] == ")":
            depth -= 1
        i += 1

    body = text[i:].lstrip()
    if not body.startswith("{"):
        raise AssertionError(f"function body {signature!r} not found")

    brace = i + (len(text[i:]) - len(body))
    depth = 0
    j = brace
    while j < len(text):
        if text[j] == "{":
            depth += 1
        elif text[j] == "}":
            depth -= 1
            if depth == 0:
                return text[brace + 1:j]
        j += 1

    raise AssertionError(f"unbalanced braces for {signature!r}")


class TachometerHeaderTests(unittest.TestCase):
    def test_status_and_api_declared(self) -> None:
        text = read_text(HDR)
        for symbol in (
            "TACHOMETER_STATUS_NOT_INITIALIZED",
            "TACHOMETER_STATUS_STOPPED",
            "TACHOMETER_STATUS_STARTING",
            "TACHOMETER_STATUS_RUNNING",
            "TACHOMETER_STATUS_TOO_SLOW",
            "TACHOMETER_STATUS_NO_SIGNAL",
            "TACHOMETER_STATUS_CONFIG_ERROR",
            "tachometer_init(",
            "tachometer_set_expected_running(",
            "tachometer_on_pulse(",
            "tachometer_process(",
            "tachometer_get_rpm(",
            "tachometer_get_status(",
            "tachometer_get_pulse_count(",
            "tachometer_reset(",
        ):
            self.assertIn(symbol, text)

    def test_config_fields_present(self) -> None:
        text = read_text(HDR)
        for field in (
            "pulses_per_revolution",
            "minimum_rpm",
            "startup_grace_ms",
            "signal_timeout_ms",
            "minimum_pulse_interval_us",
        ):
            self.assertIn(field, text)


class TachometerBehaviorTests(unittest.TestCase):
    def test_init_validates_input_and_config(self) -> None:
        body = source_function(read_text(SRC), "drv_status_t tachometer_init(")
        self.assertIn("tachometer == (tachometer_t*)0", body)
        self.assertIn("config == (const tachometer_config_t*)0", body)
        self.assertIn("pulses_per_revolution == 0u", body)
        self.assertIn("TACHOMETER_STATUS_CONFIG_ERROR", body)

    def test_expected_running_is_explicit(self) -> None:
        body = source_function(read_text(SRC), "void tachometer_set_expected_running(")
        self.assertIn("expected_running == expected_running", body)
        self.assertIn("tachometer->expected_running_since_us = now_us", body)
        self.assertIn("TACHOMETER_STATUS_STARTING", body)
        self.assertIn("TACHOMETER_STATUS_STOPPED", body)

    def test_first_and_second_pulse_paths_exist(self) -> None:
        body = source_function(read_text(SRC), "uint8_t tachometer_on_pulse(")
        self.assertIn("tachometer->has_pulse == 0u", body)
        self.assertIn("tachometer->pulse_count = 1UL", body)
        self.assertIn("tachometer->pulse_count++", body)
        self.assertIn("tachometer_compute_rpm", body)

    def test_noise_rejection_and_wrap_safe_time(self) -> None:
        body = source_function(read_text(SRC), "uint8_t tachometer_on_pulse(")
        self.assertIn("minimum_pulse_interval_us != 0u", body)
        self.assertIn("interval_us < (uint32_t)tachometer->config.minimum_pulse_interval_us", body)
        self.assertIn("now_us - tachometer->last_pulse_us", body)

    def test_rpm_formula_is_explicit(self) -> None:
        body = source_function(read_text(SRC), "static uint16_t tachometer_compute_rpm(")
        self.assertIn("60000000ULL", body)
        self.assertIn("pulse_interval_us", body)
        self.assertIn("pulses_per_revolution", body)
        self.assertIn("rpm > 65535ULL", body)

    def test_timeout_and_starting_logic(self) -> None:
        body = source_function(read_text(SRC), "static void tachometer_update_status(")
        for symbol in (
            "startup_grace_ms",
            "signal_timeout_ms",
            "pulse_count < 2UL",
            "TACHOMETER_STATUS_NO_SIGNAL",
            "TACHOMETER_STATUS_TOO_SLOW",
            "TACHOMETER_STATUS_RUNNING",
        ):
            self.assertIn(symbol, body)

    def test_reset_clears_runtime_state(self) -> None:
        body = source_function(read_text(SRC), "void tachometer_reset(")
        self.assertIn("TACHOMETER_STATUS_STOPPED", body)
        self.assertIn("tachometer_clear_measurement", body)


if __name__ == "__main__":
    unittest.main(verbosity=2)

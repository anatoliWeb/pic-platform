from __future__ import annotations

import unittest
from pathlib import Path
from dataclasses import dataclass, field


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


def rpm_from_interval(pulses_per_revolution: int, interval_us: int) -> int:
    if interval_us <= 0 or pulses_per_revolution <= 0:
        return 0
    rpm = 60000000 // (interval_us * pulses_per_revolution)
    return min(rpm, 65535)


def u32_diff(now_us: int, past_us: int) -> int:
    return (now_us - past_us) & 0xFFFFFFFF


@dataclass
class TachometerRuntimeHarness:
    pulses_per_revolution: int
    minimum_rpm: int
    startup_grace_ms: int
    signal_timeout_ms: int
    minimum_pulse_interval_us: int
    initialized: int = 0
    expected_running: int = 0
    status: str = "STOPPED"
    expected_running_since_us: int = 0
    last_pulse_us: int = 0
    pulse_count: int = 0
    rpm: int = 0
    session_pulse_count: int = 0

    def init(self) -> None:
        self.initialized = 1
        self.expected_running = 0
        self.status = "STOPPED"
        self.expected_running_since_us = 0
        self.last_pulse_us = 0
        self.pulse_count = 0
        self.rpm = 0
        self.session_pulse_count = 0

    def set_expected_running(self, expected_running: int, now_us: int) -> None:
        expected_running = 1 if expected_running else 0
        if self.expected_running == expected_running:
            return
        self.expected_running = expected_running
        self.expected_running_since_us = now_us
        self.last_pulse_us = 0
        self.rpm = 0
        self.session_pulse_count = 0
        self.status = "STARTING" if expected_running else "STOPPED"

    def _rearm(self) -> None:
        self.session_pulse_count = 0
        self.last_pulse_us = 0
        self.rpm = 0

    def process(self, now_us: int) -> None:
        if not self.initialized:
            return
        if not self.expected_running:
            self._rearm()
            self.status = "STOPPED"
            return
        if u32_diff(now_us, self.expected_running_since_us) < self.startup_grace_ms * 1000:
            self.status = "STARTING"
            return
        if self.session_pulse_count == 0:
            self._rearm()
            self.status = "NO_SIGNAL"
            return
        if self.signal_timeout_ms and u32_diff(now_us, self.last_pulse_us) >= self.signal_timeout_ms * 1000:
            self._rearm()
            self.status = "NO_SIGNAL"
            return
        if self.session_pulse_count < 2:
            self.status = "STARTING"
            return
        if self.minimum_rpm and self.rpm < self.minimum_rpm:
            self.status = "TOO_SLOW"
            return
        self.status = "RUNNING"

    def on_pulse(self, now_us: int) -> int:
        if not self.initialized or not self.expected_running:
            return 0
        if self.session_pulse_count != 0 and self.signal_timeout_ms and u32_diff(now_us, self.last_pulse_us) >= self.signal_timeout_ms * 1000:
            self._rearm()
            self.status = "NO_SIGNAL"
        if self.session_pulse_count == 0:
            self.session_pulse_count = 1
            self.last_pulse_us = now_us
            self.pulse_count += 1
            self.rpm = 0
            self.process(now_us)
            return 1
        interval_us = u32_diff(now_us, self.last_pulse_us)
        if self.minimum_pulse_interval_us and interval_us < self.minimum_pulse_interval_us:
            return 0
        self.last_pulse_us = now_us
        self.session_pulse_count = 2
        self.pulse_count += 1
        self.rpm = rpm_from_interval(self.pulses_per_revolution, interval_us)
        self.process(now_us)
        return 1


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
        self.assertIn("tachometer->session_pulse_count == 0u", body)
        self.assertIn("tachometer->session_pulse_count = 1u", body)
        self.assertIn("tachometer->session_pulse_count = 2u", body)
        self.assertNotIn("tachometer->session_pulse_count++", body)
        self.assertIn("tachometer->pulse_count++", body)
        self.assertIn("tachometer_compute_rpm", body)

    def test_stale_pulse_timestamp_is_re_armed(self) -> None:
        body = source_function(read_text(SRC), "uint8_t tachometer_on_pulse(")
        self.assertIn("tachometer_rearm(tachometer)", body)
        self.assertIn("signal_timeout_ms", body)
        self.assertIn("tachometer->last_pulse_us", body)

    def test_timeout_re_arms_session_keeps_cumulative_count(self) -> None:
        body = source_function(read_text(SRC), "static void tachometer_update_status(")
        self.assertIn("tachometer_rearm(tachometer)", body)
        self.assertIn("session_pulse_count == 0u", body)
        self.assertNotIn("tachometer->pulse_count = 0UL", body)

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
            "session_pulse_count < 2u",
            "TACHOMETER_STATUS_NO_SIGNAL",
            "TACHOMETER_STATUS_TOO_SLOW",
            "TACHOMETER_STATUS_RUNNING",
        ):
            self.assertIn(symbol, body)

    def test_reset_clears_runtime_state(self) -> None:
        body = source_function(read_text(SRC), "void tachometer_reset(")
        self.assertIn("TACHOMETER_STATUS_STOPPED", body)
        self.assertIn("tachometer_clear_measurement", body)


class TachometerRuntimeTests(unittest.TestCase):
    def make_harness(self) -> TachometerRuntimeHarness:
        harness = TachometerRuntimeHarness(
            pulses_per_revolution=2,
            minimum_rpm=900,
            startup_grace_ms=250,
            signal_timeout_ms=500,
            minimum_pulse_interval_us=800,
        )
        harness.init()
        harness.set_expected_running(1, 0)
        return harness

    def test_timestamp_zero_first_second_and_timeout_rearm(self) -> None:
        tach = self.make_harness()

        self.assertEqual(tach.on_pulse(0), 1)
        self.assertEqual(tach.session_pulse_count, 1)
        self.assertEqual(tach.pulse_count, 1)
        self.assertEqual(tach.rpm, 0)

        self.assertEqual(tach.on_pulse(10000), 1)
        self.assertEqual(tach.session_pulse_count, 2)
        self.assertGreater(tach.rpm, 0)

        tach.process(700000)
        self.assertEqual(tach.status, "NO_SIGNAL")
        self.assertEqual(tach.rpm, 0)
        self.assertEqual(tach.session_pulse_count, 0)
        self.assertEqual(tach.pulse_count, 2)

        self.assertEqual(tach.on_pulse(710000), 1)
        self.assertEqual(tach.session_pulse_count, 1)
        self.assertEqual(tach.rpm, 0)

        self.assertEqual(tach.on_pulse(720000), 1)
        self.assertEqual(tach.session_pulse_count, 2)
        self.assertGreater(tach.rpm, 0)
        self.assertIn(tach.status, {"RUNNING", "TOO_SLOW"})

    def test_long_running_stream_saturates_session_state(self) -> None:
        tach = self.make_harness()
        accepted = 0
        checkpoints: dict[int, tuple[int, int, str]] = {}

        for pulse_index in range(300):
            now_us = pulse_index * 10000
            accepted += tach.on_pulse(now_us)
            if pulse_index + 1 in {254, 255, 256, 300}:
                checkpoints[pulse_index + 1] = (tach.session_pulse_count, tach.rpm, tach.status)

        self.assertEqual(accepted, 300)
        self.assertEqual(tach.pulse_count, 300)
        self.assertEqual(tach.session_pulse_count, 2)
        self.assertNotEqual(tach.rpm, 0)
        self.assertIn(tach.status, {"RUNNING", "TOO_SLOW"})
        self.assertEqual(checkpoints[254][0], 2)
        self.assertEqual(checkpoints[255][0], 2)
        self.assertEqual(checkpoints[256][0], 2)
        self.assertEqual(checkpoints[300][0], 2)
        self.assertEqual(checkpoints[255][1], checkpoints[254][1])
        self.assertEqual(checkpoints[256][1], checkpoints[255][1])
        self.assertEqual(checkpoints[300][1], checkpoints[256][1])
        self.assertNotEqual(checkpoints[256][1], 0)
        self.assertEqual(checkpoints[254][2], tach.status)

    def test_timeout_preserves_cumulative_count_and_recovery(self) -> None:
        tach = self.make_harness()
        self.assertEqual(tach.on_pulse(0), 1)
        self.assertEqual(tach.on_pulse(10000), 1)
        self.assertEqual(tach.pulse_count, 2)

        tach.process(700000)
        self.assertEqual(tach.status, "NO_SIGNAL")
        self.assertEqual(tach.pulse_count, 2)

        self.assertEqual(tach.on_pulse(710000), 1)
        self.assertEqual(tach.session_pulse_count, 1)
        self.assertEqual(tach.rpm, 0)
        self.assertEqual(tach.pulse_count, 3)

        self.assertEqual(tach.on_pulse(720000), 1)
        self.assertEqual(tach.session_pulse_count, 2)
        self.assertEqual(tach.pulse_count, 4)
        self.assertNotEqual(tach.rpm, 0)

    def test_wrap_around_interval_and_timeout(self) -> None:
        tach = self.make_harness()
        self.assertEqual(tach.on_pulse(0xFFFFFF00), 1)
        self.assertEqual(tach.on_pulse(0x00002710), 1)
        self.assertEqual(tach.session_pulse_count, 2)
        self.assertNotEqual(tach.rpm, 0)

        tach.process(0x000A1200)
        self.assertIn(tach.status, {"RUNNING", "TOO_SLOW", "NO_SIGNAL"})

    def test_multiple_instances_are_independent(self) -> None:
        first = self.make_harness()
        second = self.make_harness()

        self.assertEqual(first.on_pulse(0), 1)
        self.assertEqual(second.on_pulse(5000), 1)
        self.assertEqual(first.pulse_count, 1)
        self.assertEqual(second.pulse_count, 1)
        self.assertEqual(first.session_pulse_count, 1)
        self.assertEqual(second.session_pulse_count, 1)

    def test_too_slow_path_is_possible(self) -> None:
        tach = TachometerRuntimeHarness(
            pulses_per_revolution=2,
            minimum_rpm=6000,
            startup_grace_ms=250,
            signal_timeout_ms=500,
            minimum_pulse_interval_us=800,
        )
        tach.init()
        tach.set_expected_running(1, 0)
        self.assertEqual(tach.on_pulse(260000), 1)
        self.assertEqual(tach.on_pulse(270000), 1)
        self.assertEqual(tach.status, "TOO_SLOW")
        self.assertNotEqual(tach.rpm, 0)


if __name__ == "__main__":
    unittest.main(verbosity=2)

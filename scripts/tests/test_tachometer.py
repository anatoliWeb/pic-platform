from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path
from dataclasses import dataclass, field


ROOT = Path(__file__).resolve().parents[2]
LIB = ROOT / "libraries" / "sensors" / "tachometer"
HDR = LIB / "tachometer.h"
SRC = LIB / "tachometer.c"


def load_config_contract():
    spec = importlib.util.spec_from_file_location(
        "config_contract", ROOT / "scripts" / "config_contract.py"
    )
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


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


SESSION_UNARMED = 0
SESSION_FIRST_PULSE = 1
SESSION_ACTIVE = 2


@dataclass
class TachometerRuntimeHarness:
    pulses_per_revolution: int
    minimum_rpm: int
    startup_grace_ms: int
    signal_timeout_ms: int
    minimum_pulse_interval_us: int
    lightweight: int = 0
    initialized: int = 0
    expected_running: int = 0
    status: str = "STOPPED"
    expected_running_since_us: int = 0
    last_pulse_us: int = 0
    pulse_count: int = 0
    rpm: int = 0
    session_state: int = SESSION_UNARMED
    minimum_interval_threshold_us: int = 0
    slow_signal: int = 0

    def init(self) -> None:
        self.initialized = 1
        self.expected_running = 0
        self.status = "STOPPED"
        self.expected_running_since_us = 0
        self.last_pulse_us = 0
        self.pulse_count = 0
        self.rpm = 0
        self.session_state = SESSION_UNARMED
        self.slow_signal = 0
        if self.pulses_per_revolution == 0:
            self.status = "CONFIG_ERROR"
        if self.lightweight and self.pulses_per_revolution:
            self.minimum_interval_threshold_us = (
                60000000 // (self.minimum_rpm * self.pulses_per_revolution)
                if self.minimum_rpm
                else 0
            )

    def set_expected_running(self, expected_running: int, now_us: int) -> None:
        expected_running = 1 if expected_running else 0
        if self.expected_running == expected_running:
            return
        self.expected_running = expected_running
        self.expected_running_since_us = now_us
        self.last_pulse_us = 0
        self.rpm = 0
        self.session_state = SESSION_UNARMED
        self.slow_signal = 0
        self.status = "STARTING" if expected_running else "STOPPED"

    def _rearm(self) -> None:
        self.session_state = SESSION_UNARMED
        self.last_pulse_us = 0
        self.rpm = 0
        self.slow_signal = 0

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
        if self.session_state == SESSION_UNARMED:
            self._rearm()
            self.status = "NO_SIGNAL"
            return
        if self.signal_timeout_ms and u32_diff(now_us, self.last_pulse_us) >= self.signal_timeout_ms * 1000:
            self._rearm()
            self.status = "NO_SIGNAL"
            return
        if self.session_state != SESSION_ACTIVE:
            self.status = "STARTING"
            return
        if self.lightweight:
            if self.slow_signal:
                self.status = "TOO_SLOW"
                return
        elif self.minimum_rpm and self.rpm < self.minimum_rpm:
            self.status = "TOO_SLOW"
            return
        self.status = "RUNNING"

    def on_pulse(self, now_us: int) -> int:
        if not self.initialized or not self.expected_running:
            return 0
        if self.session_state != SESSION_UNARMED and self.signal_timeout_ms and u32_diff(now_us, self.last_pulse_us) >= self.signal_timeout_ms * 1000:
            self._rearm()
            self.status = "NO_SIGNAL"
        if self.session_state == SESSION_UNARMED:
            self.session_state = SESSION_FIRST_PULSE
            self.last_pulse_us = now_us
            self.pulse_count += 1
            self.rpm = 0
            self.slow_signal = 0
            self.process(now_us)
            return 1
        interval_us = u32_diff(now_us, self.last_pulse_us)
        if self.minimum_pulse_interval_us and interval_us < self.minimum_pulse_interval_us:
            return 0
        self.last_pulse_us = now_us
        self.session_state = SESSION_ACTIVE
        self.pulse_count += 1
        if self.lightweight:
            self.rpm = 0
            self.slow_signal = 1 if (
                self.minimum_rpm != 0
                and (interval_us == 0 or interval_us > self.minimum_interval_threshold_us)
            ) else 0
        else:
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

    def test_session_state_is_named_enum_not_counter(self) -> None:
        text = read_text(HDR)
        for symbol in (
            "tachometer_session_state_t",
            "TACHOMETER_SESSION_UNARMED",
            "TACHOMETER_SESSION_FIRST_PULSE",
            "TACHOMETER_SESSION_ACTIVE",
            "tachometer_session_state_t session_state",
        ):
            self.assertIn(symbol, text)
        self.assertNotIn("session_pulse_count", text)

    def test_critical_section_macros_declared(self) -> None:
        text = read_text(HDR)
        for symbol in (
            "DRV_INT_SAVE_AND_DISABLE",
            "DRV_INT_RESTORE",
        ):
            self.assertIn(symbol, text)
        self.assertIn("core/compiler.h", text)

    def test_isr_contract_documented(self) -> None:
        text = read_text(HDR)
        for phrase in (
            "ISR/polling contract",
            "Safe to call from a timer or external interrupt",
            "DRV_INT_SAVE_AND_DISABLE / DRV_INT_RESTORE",
            "Main-loop context only",
        ):
            self.assertIn(phrase, text)


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
        self.assertIn("tachometer->session_state == TACHOMETER_SESSION_UNARMED", body)
        self.assertIn("tachometer->session_state = TACHOMETER_SESSION_FIRST_PULSE", body)
        self.assertIn("tachometer->session_state = TACHOMETER_SESSION_ACTIVE", body)
        self.assertNotIn("tachometer->session_state++", body)
        self.assertNotIn("session_pulse_count", body)
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
        self.assertIn("session_state == TACHOMETER_SESSION_UNARMED", body)
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
            "session_state != TACHOMETER_SESSION_ACTIVE",
            "TACHOMETER_STATUS_NO_SIGNAL",
            "TACHOMETER_STATUS_TOO_SLOW",
            "TACHOMETER_STATUS_RUNNING",
        ):
            self.assertIn(symbol, body)

    def test_reset_clears_runtime_state(self) -> None:
        body = source_function(read_text(SRC), "void tachometer_reset(")
        self.assertIn("TACHOMETER_STATUS_STOPPED", body)
        self.assertIn("tachometer_clear_measurement", body)

    def test_on_pulse_uses_critical_section(self) -> None:
        body = source_function(read_text(SRC), "uint8_t tachometer_on_pulse(")
        self.assertIn("DRV_INT_SAVE_AND_DISABLE(int_state)", body)
        self.assertIn("DRV_INT_RESTORE(int_state)", body)

    def test_process_takes_atomic_snapshot(self) -> None:
        body = source_function(read_text(SRC), "void tachometer_process(")
        self.assertIn("DRV_INT_SAVE_AND_DISABLE(int_state)", body)
        self.assertIn("DRV_INT_RESTORE(int_state)", body)
        self.assertIn("last_pulse_us_snap", body)
        self.assertIn("session_state_snap", body)

    def test_process_reverifies_before_rearm(self) -> None:
        body = source_function(read_text(SRC), "void tachometer_process(")
        self.assertIn("tachometer->last_pulse_us == last_pulse_us_snap", body)

    def test_process_no_unconditional_gie_enable(self) -> None:
        body = source_function(read_text(SRC), "void tachometer_process(")
        self.assertNotIn("GIE = 1", body)
        self.assertNotIn("GIE = 1u", body)

    def test_get_rpm_uses_critical_section(self) -> None:
        body = source_function(read_text(SRC), "uint16_t tachometer_get_rpm(")
        self.assertIn("DRV_INT_SAVE_AND_DISABLE(int_state)", body)
        self.assertIn("DRV_INT_RESTORE(int_state)", body)
        self.assertIn("rpm = tachometer->rpm", body)

    def test_get_status_uses_critical_section(self) -> None:
        body = source_function(read_text(SRC), "tachometer_status_t tachometer_get_status(")
        self.assertIn("DRV_INT_SAVE_AND_DISABLE(int_state)", body)
        self.assertIn("DRV_INT_RESTORE(int_state)", body)

    def test_get_pulse_count_uses_critical_section(self) -> None:
        body = source_function(read_text(SRC), "uint32_t tachometer_get_pulse_count(")
        self.assertIn("DRV_INT_SAVE_AND_DISABLE(int_state)", body)
        self.assertIn("DRV_INT_RESTORE(int_state)", body)
        self.assertIn("count = tachometer->pulse_count", body)

    def test_on_pulse_does_not_unconditionally_enable_interrupts(self) -> None:
        body = source_function(read_text(SRC), "uint8_t tachometer_on_pulse(")
        self.assertNotIn("GIE = 1", body)
        self.assertNotIn("GIE = 1u", body)
        self.assertIn("drv_int_state_t", body)


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
        self.assertEqual(tach.session_state, SESSION_FIRST_PULSE)
        self.assertEqual(tach.pulse_count, 1)
        self.assertEqual(tach.rpm, 0)

        self.assertEqual(tach.on_pulse(10000), 1)
        self.assertEqual(tach.session_state, SESSION_ACTIVE)
        self.assertGreater(tach.rpm, 0)

        tach.process(700000)
        self.assertEqual(tach.status, "NO_SIGNAL")
        self.assertEqual(tach.rpm, 0)
        self.assertEqual(tach.session_state, SESSION_UNARMED)
        self.assertEqual(tach.pulse_count, 2)

        self.assertEqual(tach.on_pulse(710000), 1)
        self.assertEqual(tach.session_state, SESSION_FIRST_PULSE)
        self.assertEqual(tach.rpm, 0)

        self.assertEqual(tach.on_pulse(720000), 1)
        self.assertEqual(tach.session_state, SESSION_ACTIVE)
        self.assertGreater(tach.rpm, 0)
        self.assertIn(tach.status, {"RUNNING", "TOO_SLOW"})

    def test_long_running_stream_saturates_session_state(self) -> None:
        # 300+ pulse overflow regression: the session phase must saturate at
        # ACTIVE and RPM must not reset at the 256-pulse boundary; only the
        # cumulative pulse_count keeps growing.
        tach = self.make_harness()
        accepted = 0
        checkpoints: dict[int, tuple[int, int, str]] = {}

        for pulse_index in range(300):
            now_us = pulse_index * 10000
            accepted += tach.on_pulse(now_us)
            if pulse_index + 1 in {254, 255, 256, 300}:
                checkpoints[pulse_index + 1] = (tach.session_state, tach.rpm, tach.status)

        self.assertEqual(accepted, 300)
        self.assertEqual(tach.pulse_count, 300)
        self.assertEqual(tach.session_state, SESSION_ACTIVE)
        self.assertNotEqual(tach.rpm, 0)
        self.assertIn(tach.status, {"RUNNING", "TOO_SLOW"})
        self.assertEqual(checkpoints[254][0], SESSION_ACTIVE)
        self.assertEqual(checkpoints[255][0], SESSION_ACTIVE)
        self.assertEqual(checkpoints[256][0], SESSION_ACTIVE)
        self.assertEqual(checkpoints[300][0], SESSION_ACTIVE)
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

        # Timeout re-arm: the first pulse after the timeout only arms the
        # session again (fresh RPM), and only the second yields new RPM.
        self.assertEqual(tach.on_pulse(710000), 1)
        self.assertEqual(tach.session_state, SESSION_FIRST_PULSE)
        self.assertEqual(tach.rpm, 0)
        self.assertEqual(tach.pulse_count, 3)

        self.assertEqual(tach.on_pulse(720000), 1)
        self.assertEqual(tach.session_state, SESSION_ACTIVE)
        self.assertEqual(tach.pulse_count, 4)
        self.assertNotEqual(tach.rpm, 0)

    def test_wrap_around_interval_and_timeout(self) -> None:
        # Timestamp-wrap regression: an interval that straddles the 32-bit
        # microsecond wrap must still compute RPM and drive the timeout.
        tach = self.make_harness()
        self.assertEqual(tach.on_pulse(0xFFFFFF00), 1)
        self.assertEqual(tach.on_pulse(0x00002710), 1)
        self.assertEqual(tach.session_state, SESSION_ACTIVE)
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
        self.assertEqual(first.session_state, SESSION_FIRST_PULSE)
        self.assertEqual(second.session_state, SESSION_FIRST_PULSE)

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


class TachometerLightweightTests(unittest.TestCase):
    def test_lightweight_macro_declared(self) -> None:
        text = read_text(HDR)
        self.assertIn("TACHOMETER_LIGHTWEIGHT", text)
        self.assertIn("#ifndef TACHOMETER_LIGHTWEIGHT", text)
        self.assertIn("#define TACHOMETER_LIGHTWEIGHT 0", text)

    def test_lightweight_get_rpm_is_macro(self) -> None:
        text = read_text(HDR)
        self.assertIn("#if TACHOMETER_LIGHTWEIGHT", text)
        self.assertIn("#define tachometer_get_rpm(tachometer) ((uint16_t)0u)", text)
        self.assertIn("#else", text)

    def test_lightweight_rpm_computation_excluded(self) -> None:
        text = read_text(SRC)
        self.assertIn("#if !TACHOMETER_LIGHTWEIGHT", text)
        self.assertIn("static uint16_t tachometer_compute_rpm(", text)

    def test_lightweight_rpm_check_in_process_excluded(self) -> None:
        body = source_function(read_text(SRC), "void tachometer_process(")
        self.assertIn("#if !TACHOMETER_LIGHTWEIGHT", body)

    def test_lightweight_rpm_check_in_update_status_excluded(self) -> None:
        body = source_function(read_text(SRC), "static void tachometer_update_status(")
        self.assertIn("#if !TACHOMETER_LIGHTWEIGHT", body)

    def test_lightweight_rearm_skips_rpm_clear(self) -> None:
        body = source_function(read_text(SRC), "static void tachometer_rearm(")
        self.assertIn("#if !TACHOMETER_LIGHTWEIGHT", body)

    def test_lightweight_on_pulse_skips_rpm_calc(self) -> None:
        body = source_function(read_text(SRC), "uint8_t tachometer_on_pulse(")
        self.assertIn("#if !TACHOMETER_LIGHTWEIGHT", body)


class TachometerLightweightMinSpeedStructuralTests(unittest.TestCase):
    def test_lightweight_min_speed_fields_declared(self) -> None:
        text = read_text(HDR)
        self.assertIn("minimum_interval_threshold_us", text)
        self.assertIn("slow_signal", text)
        self.assertIn("#if TACHOMETER_LIGHTWEIGHT", text)
        self.assertIn("#endif", text)

    def test_lightweight_init_computes_interval_threshold(self) -> None:
        body = source_function(read_text(SRC), "drv_status_t tachometer_init(")
        self.assertIn("#if TACHOMETER_LIGHTWEIGHT", body)
        self.assertIn("minimum_interval_threshold_us", body)
        self.assertIn("60000000UL /", body)
        self.assertIn("minimum_rpm", body)
        self.assertIn("pulses_per_revolution", body)

    def test_lightweight_rearm_clears_slow_signal(self) -> None:
        body = source_function(read_text(SRC), "static void tachometer_rearm(")
        self.assertIn("#if !TACHOMETER_LIGHTWEIGHT", body)
        self.assertIn("slow_signal = 0u", body)

    def test_lightweight_on_pulse_sets_slow_signal(self) -> None:
        body = source_function(read_text(SRC), "uint8_t tachometer_on_pulse(")
        self.assertIn("#if !TACHOMETER_LIGHTWEIGHT", body)
        self.assertIn("slow_signal =", body)
        self.assertIn("minimum_interval_threshold_us", body)

    def test_lightweight_process_uses_slow_signal(self) -> None:
        body = source_function(read_text(SRC), "void tachometer_process(")
        self.assertIn("slow_signal", body)
        self.assertIn("TACHOMETER_STATUS_TOO_SLOW", body)

    def test_lightweight_update_status_uses_slow_signal(self) -> None:
        body = source_function(read_text(SRC), "static void tachometer_update_status(")
        self.assertIn("slow_signal", body)
        self.assertIn("TACHOMETER_STATUS_TOO_SLOW", body)

    def test_lightweight_threshold_uses_32bit_division(self) -> None:
        body = source_function(read_text(SRC), "drv_status_t tachometer_init(")
        self.assertIn("60000000UL /", body)
        self.assertNotIn("60000000ULL", body)


class TachometerLightweightBehaviorTests(unittest.TestCase):
    def make_harness(self, minimum_rpm=900, pulses_per_revolution=2,
                     startup_grace_ms=0, signal_timeout_ms=10000,
                     minimum_pulse_interval_us=800) -> TachometerRuntimeHarness:
        harness = TachometerRuntimeHarness(
            pulses_per_revolution=pulses_per_revolution,
            minimum_rpm=minimum_rpm,
            startup_grace_ms=startup_grace_ms,
            signal_timeout_ms=signal_timeout_ms,
            minimum_pulse_interval_us=minimum_pulse_interval_us,
            lightweight=1,
        )
        harness.init()
        harness.set_expected_running(1, 0)
        return harness

    def feed_pair(self, harness: TachometerRuntimeHarness, t0: int, t1: int) -> str:
        self.assertEqual(harness.on_pulse(t0), 1)
        self.assertEqual(harness.on_pulse(t1), 1)
        return harness.status

    def test_initial_state_is_stopped(self) -> None:
        harness = TachometerRuntimeHarness(
            pulses_per_revolution=2, minimum_rpm=900, startup_grace_ms=0,
            signal_timeout_ms=500, minimum_pulse_interval_us=800, lightweight=1,
        )
        harness.init()
        self.assertEqual(harness.status, "STOPPED")
        self.assertEqual(harness.slow_signal, 0)

    def test_first_pulse_is_starting(self) -> None:
        harness = self.make_harness()
        self.assertEqual(harness.on_pulse(0), 1)
        self.assertEqual(harness.status, "STARTING")
        self.assertEqual(harness.rpm, 0)

    def test_clearly_above_minimum_is_running(self) -> None:
        harness = self.make_harness()
        self.assertEqual(self.feed_pair(harness, 0, 10000), "RUNNING")

    def test_just_above_minimum_is_running(self) -> None:
        harness = self.make_harness()
        # threshold for min 900 / ppr 2 is 33333 us; rpm at 33332 us is 900.
        self.assertEqual(self.feed_pair(harness, 0, 33332), "RUNNING")

    def test_exactly_at_threshold_is_running(self) -> None:
        harness = self.make_harness()
        # rpm at exactly 33333 us is 900, which is not below minimum_rpm.
        self.assertEqual(self.feed_pair(harness, 0, 33333), "RUNNING")

    def test_just_below_minimum_is_too_slow(self) -> None:
        harness = self.make_harness()
        # rpm at 33334 us is 899, below minimum_rpm.
        self.assertEqual(self.feed_pair(harness, 0, 33334), "TOO_SLOW")

    def test_very_slow_valid_pulses_are_too_slow(self) -> None:
        harness = self.make_harness()
        # 1 s interval -> rpm 30, well below minimum; still inside 10 s timeout.
        self.assertEqual(self.feed_pair(harness, 0, 1000000), "TOO_SLOW")

    def test_too_slow_persists_between_pulses_in_process(self) -> None:
        harness = self.make_harness()
        self.assertEqual(self.feed_pair(harness, 0, 1000000), "TOO_SLOW")
        harness.process(1500000)
        self.assertEqual(harness.status, "TOO_SLOW")

    def test_no_signal_when_pulses_stop(self) -> None:
        harness = self.make_harness()
        self.assertEqual(self.feed_pair(harness, 0, 10000), "RUNNING")
        harness.process(30000000)
        self.assertEqual(harness.status, "NO_SIGNAL")

    def test_no_signal_distinct_from_too_slow(self) -> None:
        slow = self.make_harness()
        self.assertEqual(self.feed_pair(slow, 0, 1000000), "TOO_SLOW")
        gone = self.make_harness()
        self.assertEqual(self.feed_pair(gone, 0, 10000), "RUNNING")
        gone.process(30000000)
        self.assertEqual(gone.status, "NO_SIGNAL")
        self.assertNotEqual(slow.status, gone.status)

    def test_startup_grace_suppresses_too_slow(self) -> None:
        harness = self.make_harness(startup_grace_ms=250)
        harness.process(0)
        self.assertEqual(harness.status, "STARTING")
        # Slow pulses inside the 250 ms grace window still report STARTING.
        self.assertEqual(harness.on_pulse(0), 1)
        self.assertEqual(harness.on_pulse(200000), 1)
        self.assertEqual(harness.status, "STARTING")

    def test_startup_grace_ends_before_min_speed_applies(self) -> None:
        harness = self.make_harness(startup_grace_ms=250)
        self.assertEqual(self.feed_pair(harness, 0, 1000000), "TOO_SLOW")

    def test_timeout_rearms_session_keeps_count(self) -> None:
        harness = self.make_harness()
        self.assertEqual(self.feed_pair(harness, 0, 10000), "RUNNING")
        self.assertEqual(harness.pulse_count, 2)
        harness.process(30000000)
        self.assertEqual(harness.status, "NO_SIGNAL")
        self.assertEqual(harness.pulse_count, 2)
        self.assertEqual(harness.session_state, SESSION_UNARMED)
        self.assertEqual(harness.slow_signal, 0)

    def test_recovery_from_too_slow_to_running(self) -> None:
        harness = self.make_harness()
        self.assertEqual(self.feed_pair(harness, 0, 1000000), "TOO_SLOW")
        # The rotor speeds up: a new interval above the minimum clears TOO_SLOW.
        self.assertEqual(self.feed_pair(harness, 2000000, 2010000), "RUNNING")
        self.assertEqual(harness.slow_signal, 0)

    def test_recovery_from_no_signal(self) -> None:
        harness = self.make_harness()
        self.assertEqual(self.feed_pair(harness, 0, 10000), "RUNNING")
        harness.process(30000000)
        self.assertEqual(harness.status, "NO_SIGNAL")
        self.assertEqual(self.feed_pair(harness, 30000000, 30010000), "RUNNING")
        self.assertEqual(harness.status, "RUNNING")

    def test_reset_clears_runtime_state(self) -> None:
        harness = self.make_harness()
        self.assertEqual(self.feed_pair(harness, 0, 1000000), "TOO_SLOW")
        harness.set_expected_running(0, 5000000)
        self.assertEqual(harness.status, "STOPPED")
        self.assertEqual(harness.slow_signal, 0)

    def test_repeated_init(self) -> None:
        harness = self.make_harness()
        self.assertEqual(self.feed_pair(harness, 0, 1000000), "TOO_SLOW")
        harness.init()
        self.assertEqual(harness.status, "STOPPED")
        harness.set_expected_running(1, 0)
        self.assertEqual(self.feed_pair(harness, 0, 10000), "RUNNING")

    def test_multiple_instances_are_independent(self) -> None:
        first = self.make_harness(minimum_rpm=900)
        second = self.make_harness(minimum_rpm=10)
        self.assertEqual(self.feed_pair(first, 0, 1000000), "TOO_SLOW")
        # The same slow interval but a low minimum keeps the second instance running.
        self.assertEqual(self.feed_pair(second, 0, 1000000), "RUNNING")

    def test_ppr_one(self) -> None:
        harness = self.make_harness(pulses_per_revolution=1)
        # threshold for min 900 / ppr 1 is 66666 us.
        self.assertEqual(self.feed_pair(harness, 0, 66666), "RUNNING")
        self.assertEqual(self.feed_pair(harness, 100000, 166667), "TOO_SLOW")

    def test_ppr_two(self) -> None:
        harness = self.make_harness(pulses_per_revolution=2)
        # threshold for min 900 / ppr 2 is 33333 us.
        self.assertEqual(self.feed_pair(harness, 0, 33333), "RUNNING")
        self.assertEqual(self.feed_pair(harness, 100000, 133334), "TOO_SLOW")

    def test_ppr_four(self) -> None:
        harness = self.make_harness(pulses_per_revolution=4)
        # threshold for min 900 / ppr 4 is 16666 us.
        self.assertEqual(self.feed_pair(harness, 0, 16666), "RUNNING")
        self.assertEqual(self.feed_pair(harness, 100000, 116667), "TOO_SLOW")

    def test_multiple_minimum_rpm_values(self) -> None:
        low = self.make_harness(minimum_rpm=100)
        mid = self.make_harness(minimum_rpm=2000)
        high = self.make_harness(minimum_rpm=6000)
        self.assertEqual(self.feed_pair(low, 0, 100000), "RUNNING")
        self.assertEqual(self.feed_pair(mid, 0, 100000), "TOO_SLOW")
        self.assertEqual(self.feed_pair(high, 0, 100000), "TOO_SLOW")

    def test_minimum_rpm_zero_disables_check(self) -> None:
        harness = self.make_harness(minimum_rpm=0)
        self.assertEqual(harness.minimum_interval_threshold_us, 0)
        self.assertEqual(self.feed_pair(harness, 0, 1000000), "RUNNING")
        self.assertEqual(harness.slow_signal, 0)

    def test_invalid_ppr_zero_is_rejected(self) -> None:
        harness = TachometerRuntimeHarness(
            pulses_per_revolution=0, minimum_rpm=900, startup_grace_ms=0,
            signal_timeout_ms=500, minimum_pulse_interval_us=800, lightweight=1,
        )
        harness.init()
        self.assertEqual(harness.status, "CONFIG_ERROR")

    def test_minimum_pulse_interval_filters_noise(self) -> None:
        harness = self.make_harness()
        self.assertEqual(harness.on_pulse(0), 1)
        self.assertEqual(harness.on_pulse(100), 0)
        self.assertEqual(harness.pulse_count, 1)
        self.assertEqual(harness.session_state, SESSION_FIRST_PULSE)

    def test_timestamp_wraparound_interval(self) -> None:
        harness = self.make_harness()
        # Interval that straddles the 32-bit wrap is still measured wrap-safe.
        self.assertEqual(harness.on_pulse(0xFFFFFF00), 1)
        self.assertEqual(harness.on_pulse(0x00002710), 1)
        # interval = 0x2810 = 10256 us -> rpm 2925, above minimum.
        self.assertEqual(harness.status, "RUNNING")

    def test_arithmetic_maximum_boundary(self) -> None:
        harness = self.make_harness(minimum_rpm=65535, pulses_per_revolution=255,
                                    minimum_pulse_interval_us=0)
        # threshold = 60000000 / 16711425 = 3.
        self.assertEqual(harness.minimum_interval_threshold_us, 3)
        self.assertEqual(self.feed_pair(harness, 0, 3), "RUNNING")
        self.assertEqual(self.feed_pair(harness, 100, 104), "TOO_SLOW")

    def test_arithmetic_minimum_boundary(self) -> None:
        harness = self.make_harness(minimum_rpm=1, pulses_per_revolution=1,
                                    minimum_pulse_interval_us=0,
                                    signal_timeout_ms=65000)
        # threshold = 60000000 / 1 = 60000000 us (60 s), inside the 65 s timeout.
        self.assertEqual(harness.minimum_interval_threshold_us, 60000000)
        self.assertEqual(self.feed_pair(harness, 0, 60000000), "RUNNING")
        self.assertEqual(self.feed_pair(harness, 100000000, 160000001), "TOO_SLOW")

    def test_zero_interval_flagged_when_noise_filter_disabled(self) -> None:
        harness = self.make_harness(minimum_pulse_interval_us=0)
        # Identical timestamps with the noise filter off match the FULL rpm==0
        # case: the interval of 0 is flagged as TOO_SLOW.
        self.assertEqual(harness.on_pulse(0), 1)
        self.assertEqual(harness.on_pulse(0), 1)
        self.assertEqual(harness.status, "TOO_SLOW")

    def test_get_rpm_stays_zero(self) -> None:
        harness = self.make_harness()
        self.assertEqual(self.feed_pair(harness, 0, 10000), "RUNNING")
        self.assertEqual(harness.rpm, 0)


class TachometerLightweightConfigContractTests(unittest.TestCase):
    """Regression: TACHOMETER_LIGHTWEIGHT changes tachometer_t layout, so it
    must be a project-wide Category A define, identical in every translation
    unit that includes the header. A TU-local #define would make the library
    and the caller disagree on the struct layout."""

    def test_macro_is_category_a_in_config_contract(self) -> None:
        contract = load_config_contract()
        self.assertIn("TACHOMETER_LIGHTWEIGHT", contract.CATEGORY_A)

    def test_header_documents_project_wide_define(self) -> None:
        text = read_text(HDR)
        self.assertIn("project-wide compiler define", text)
        self.assertIn("-DTACHOMETER_LIGHTWEIGHT=1", text)
        self.assertIn("Do not #define it inside a single .c file", text)

    def test_header_default_is_full(self) -> None:
        text = read_text(HDR)
        self.assertIn("#ifndef TACHOMETER_LIGHTWEIGHT", text)
        self.assertIn("#define TACHOMETER_LIGHTWEIGHT 0", text)

    def test_configuration_docs_list_macro_as_category_a(self) -> None:
        en = read_text(ROOT / "docs" / "configuration.md")
        ua = read_text(ROOT / "docs" / "configuration.ua.md")
        for text in (en, ua):
            self.assertIn("TACHOMETER_LIGHTWEIGHT", text)

    def test_library_source_does_not_define_macro_locally(self) -> None:
        src = read_text(SRC)
        self.assertNotIn("#define TACHOMETER_LIGHTWEIGHT 1", src)
        hdr = read_text(HDR)
        self.assertNotIn("#define TACHOMETER_LIGHTWEIGHT 1", hdr)


class TachometerProfileDifferentialTests(unittest.TestCase):
    """FULL and LIGHTWEIGHT must reach the same TOO_SLOW/RUNNING decision for
    every valid (ppr, minimum_rpm, interval) tuple."""

    MINIMUMS = (0, 1, 900, 65535)
    PPMS = (1, 2, 4, 255)

    @staticmethod
    def threshold(ppr: int, minimum_rpm: int) -> int | None:
        if minimum_rpm == 0:
            return None
        return 60000000 // (minimum_rpm * ppr)

    def intervals_for(self, ppr: int, minimum_rpm: int) -> list[int]:
        base = {0, 1, 2, 3, 10000, 60000000}
        threshold = self.threshold(ppr, minimum_rpm)
        if threshold is not None and threshold > 1:
            base.update({threshold - 1, threshold, threshold + 1})
        return sorted(base)

    def run_pair(self, ppr: int, minimum_rpm: int, interval_us: int):
        full = TachometerRuntimeHarness(
            pulses_per_revolution=ppr,
            minimum_rpm=minimum_rpm,
            startup_grace_ms=0,
            signal_timeout_ms=65000,
            minimum_pulse_interval_us=0,
            lightweight=0,
        )
        light = TachometerRuntimeHarness(
            pulses_per_revolution=ppr,
            minimum_rpm=minimum_rpm,
            startup_grace_ms=0,
            signal_timeout_ms=65000,
            minimum_pulse_interval_us=0,
            lightweight=1,
        )
        for harness in (full, light):
            harness.init()
            harness.set_expected_running(1, 0)
            harness.on_pulse(0)
            harness.on_pulse(interval_us)
            harness.process(interval_us + 1)
        return full.status, light.status

    def test_differential_status_matches_all_tuples(self) -> None:
        checked = 0
        for ppr in self.PPMS:
            for minimum_rpm in self.MINIMUMS:
                for interval_us in self.intervals_for(ppr, minimum_rpm):
                    full_status, light_status = self.run_pair(ppr, minimum_rpm, interval_us)
                    with self.subTest(ppr=ppr, minimum_rpm=minimum_rpm, interval_us=interval_us):
                        self.assertEqual(light_status, full_status)
                    checked += 1
        self.assertGreater(checked, 40)

    def test_threshold_arithmetic_never_overflows(self) -> None:
        # maximum product 65535 * 255 = 16711425 fits uint32; the threshold
        # division uses only uint32 operands (60000000UL, cast operands).
        self.assertEqual(60000000 // (65535 * 255), 3)
        self.assertLess(65535 * 255, 1 << 32)

    def test_zero_ppr_is_config_error_in_both_profiles(self) -> None:
        for lightweight in (0, 1):
            harness = TachometerRuntimeHarness(
                pulses_per_revolution=0,
                minimum_rpm=900,
                startup_grace_ms=0,
                signal_timeout_ms=65000,
                minimum_pulse_interval_us=0,
                lightweight=lightweight,
            )
            harness.init()
            self.assertEqual(harness.status, "CONFIG_ERROR")


if __name__ == "__main__":
    unittest.main(verbosity=2)

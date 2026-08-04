from __future__ import annotations

import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]

LIB = ROOT / "libraries" / "output" / "ac_phase_control"
HDR = LIB / "ac_phase_control.h"
SRC = LIB / "ac_phase_control.c"
FIXTURES = ROOT / "scripts" / "tests" / "fixtures"
HARNESS = FIXTURES / "ac_phase_control_harness.c"

XC8 = shutil.which("xc8")

DFP = Path(r"C:\Program Files\Microchip\MPLABX\v6.30\packs\Microchip\PIC18Fxxxx_DFP\1.7.171\xc8")
MCU = "18F452"

CLOCK_DEFINE = "PIC_PLATFORM_CLOCK_HZ=10000000UL"


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8-sig")


def read_bytes(path: Path) -> bytes:
    return path.read_bytes()


def extract_source_function(text: str, signature: str) -> str:
    """Extract a function body by definition name.

    ``signature`` is the prefix ending right before the ``(`` (for example
    ``void ac_phase_control_on_zero_cross``). The first-trusted definition is
    the occurrence whose parameter list is directly followed by a ``{``.
    """
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
    out_dir = Path(tempfile.gettempdir()) / "ac_phase_control_test_builds"
    out_dir.mkdir(parents=True, exist_ok=True)
    return out_dir


def _stage(source: Path, out_dir: Path) -> Path:
    staged = out_dir / source.name
    staged.write_bytes(source.read_bytes())
    return staged


def compile_library() -> subprocess.CompletedProcess[str]:
    out_dir = _build_dir()
    staged = _stage(HARNESS, out_dir)
    args = [
        f"-mcpu={MCU}",
        f"-mdfp={DFP}",
        f"-I{ROOT}",
        f"-D{CLOCK_DEFINE}",
        "-O0",
        "-std=c99",
        str(staged),
        "-o",
        str(out_dir / "ac_phase_control_out.hex"),
    ]
    return run_xc8(args)


class AcPhaseControlHeaderTests(unittest.TestCase):
    def test_include_guard(self) -> None:
        self.assertIn("LIBRARIES_OUTPUT_AC_PHASE_CONTROL_H", read_text(HDR))

    def test_channel_count_default(self) -> None:
        self.assertIn("#define AC_PHASE_CONTROL_MAX_CHANNELS 4U", read_text(HDR))

    def test_relay_defaults(self) -> None:
        text = read_text(HDR)
        for macro in (
            "AC_PHASE_CONTROL_DEFAULT_RELAY_ON_THRESHOLD   98U",
            "AC_PHASE_CONTROL_DEFAULT_RELAY_OFF_THRESHOLD  96U",
            "AC_PHASE_CONTROL_DEFAULT_RELAY_BREAK_MAKE_MS  50U",
            "AC_PHASE_CONTROL_DEFAULT_RELAY_MIN_ON_MS      200U",
            "AC_PHASE_CONTROL_DEFAULT_RELAY_MIN_OFF_MS     200U",
            "AC_PHASE_CONTROL_ZERO_CROSS_RECOVERY_EVENTS   2U",
        ):
            self.assertIn(macro, text)

    def test_status_enum(self) -> None:
        text = read_text(HDR)
        for member in (
            "AC_PHASE_STATUS_OK",
            "AC_PHASE_STATUS_NOT_INITIALIZED",
            "AC_PHASE_STATUS_CONFIG_ERROR",
            "AC_PHASE_STATUS_ZERO_CROSS_LOST",
        ):
            self.assertIn(member, text)

    def test_relay_state_enum(self) -> None:
        text = read_text(HDR)
        for member in (
            "AC_PHASE_RELAY_STATE_PHASE",
            "AC_PHASE_RELAY_STATE_WAIT_ON",
            "AC_PHASE_RELAY_STATE_HOLD_ON",
            "AC_PHASE_RELAY_STATE_WAIT_OFF",
        ):
            self.assertIn(member, text)

    def test_config_fields_declared(self) -> None:
        text = read_text(HDR)
        for field in (
            "relay_on_threshold_percent",
            "relay_off_threshold_percent",
            "relay_break_before_make_ms",
            "relay_min_on_ms",
            "relay_min_off_ms",
            "zero_cross_timeout_ms",
        ):
            self.assertIn(field, text)

    def test_channel_relay_fields_declared(self) -> None:
        text = read_text(HDR)
        for field in (
            "relay_lat",
            "relay_tris",
            "relay_mask",
            "relay_state",
            "relay_active",
            "relay_on_ms",
            "relay_off_ms",
        ):
            self.assertIn(field, text)

    def test_group_relay_fields_declared(self) -> None:
        text = read_text(HDR)
        for field in (
            "zero_cross",
            "owned_zero_cross",
            "zero_cross_bound",
            "status",
        ):
            self.assertIn(field, text)

    def test_zero_cross_state_removed_from_group(self) -> None:
        text = read_text(HDR)
        self.assertNotIn("last_zero_cross_ms", text)
        self.assertNotIn("zero_cross_recovery_count", text)

    def test_zero_cross_dependency_declared(self) -> None:
        text = read_text(HDR)
        self.assertIn("zero_cross.h", text)
        self.assertIn("ac_phase_control_bind_zero_cross(", text)
        self.assertIn("ac_phase_control_on_zero_cross_event(", text)
        self.assertIn("zero_cross_event_t* event", text)

    def test_relay_api_declared(self) -> None:
        text = read_text(HDR)
        for declaration in (
            "ac_phase_control_attach_channel_relay(",
            "ac_phase_control_is_channel_in_relay_mode(",
            "ac_phase_control_process(",
            "ac_phase_control_all_off(",
            "ac_phase_control_get_status(",
            "ac_phase_control_is_zero_cross_alive(",
        ):
            self.assertIn(declaration, text)


class AcPhaseControlBehaviorTests(unittest.TestCase):
    """Structural behavioral checks on the library source text."""

    def test_init_rejects_invalid_arguments(self) -> None:
        body = extract_source_function(read_text(SRC), "drv_status_t ac_phase_control_init_group(")
        for guard in (
            "group == (ac_phase_control_group_t*)0",
            "config == (const ac_phase_control_config_t*)0",
            "channels == (ac_phase_control_channel_t*)0",
            "channel_count == 0u",
        ):
            self.assertIn(guard, body)

    def test_init_validates_half_cycle_and_delays(self) -> None:
        body = extract_source_function(read_text(SRC), "drv_status_t ac_phase_control_init_group(")
        for guard in (
            "config->half_cycle_us == 0u",
            "config->min_delay_us > config->half_cycle_us",
            "config->max_delay_us > config->half_cycle_us",
            "config->min_delay_us > config->max_delay_us",
            "config->gate_pulse_us == 0u",
        ):
            self.assertIn(guard, body)
        self.assertIn("AC_PHASE_STATUS_CONFIG_ERROR", body)

    def test_init_sets_noheap_message_timer_bindings(self) -> None:
        body = extract_source_function(read_text(SRC), "drv_status_t ac_phase_control_init_group(")
        self.assertIn("g_active_group = group;", body)
        self.assertIn("ac_phase_control_timer_init(group)", body)

    def test_only_timer2_returns_ok(self) -> None:
        body = extract_source_function(read_text(SRC), "static drv_status_t ac_phase_control_timer_init(")
        self.assertIn("group->timer != AC_PHASE_CONTROL_TIMER2", body)
        self.assertIn("return DRV_STATUS_ERROR;", body)
        self.assertIn("group->timer_tick_us = 102u;", body)

    def test_relay_defaults_apply_when_zero(self) -> None:
        body = extract_source_function(read_text(SRC), "drv_status_t ac_phase_control_init_group(")
        for macro in (
            "AC_PHASE_CONTROL_DEFAULT_RELAY_ON_THRESHOLD",
            "AC_PHASE_CONTROL_DEFAULT_RELAY_OFF_THRESHOLD",
            "AC_PHASE_CONTROL_DEFAULT_RELAY_BREAK_MAKE_MS",
            "AC_PHASE_CONTROL_DEFAULT_RELAY_MIN_ON_MS",
            "AC_PHASE_CONTROL_DEFAULT_RELAY_MIN_OFF_MS",
        ):
            self.assertIn(macro, body)

    def test_init_validates_threshold_order(self) -> None:
        body = extract_source_function(read_text(SRC), "drv_status_t ac_phase_control_init_group(")
        for guard in (
            "group->config.relay_on_threshold_percent > 100u",
            "group->config.relay_off_threshold_percent > 100u",
            "group->config.relay_on_threshold_percent <= group->config.relay_off_threshold_percent",
        ):
            self.assertIn(guard, body)

    def test_attach_validates_single_bit_mask(self) -> None:
        body = extract_source_function(read_text(SRC), "drv_status_t ac_phase_control_attach_channel(")
        self.assertIn("ac_phase_control_mask_valid(gate_mask) == 0u", body)
        self.assertIn("*(entry->gate_tris) &= (uint8_t)(~entry->gate_mask)", body)

    def test_attach_relay_validates_single_bit_mask(self) -> None:
        body = extract_source_function(read_text(SRC), "drv_status_t ac_phase_control_attach_channel_relay(")
        self.assertIn("ac_phase_control_mask_valid(relay_mask) == 0u", body)
        self.assertIn("*(entry->relay_tris) &= (uint8_t)(~entry->relay_mask)", body)

    def test_set_percent_zero_disables(self) -> None:
        body = extract_source_function(read_text(SRC), "drv_status_t ac_phase_control_set_power_percent(")
        self.assertIn("entry->enabled = 0u", body)
        self.assertIn("ac_phase_control_reset_channel_cycle(entry)", body)

    def test_set_percent_clamps_above_100(self) -> None:
        body = extract_source_function(read_text(SRC), "drv_status_t ac_phase_control_set_power_percent(")
        self.assertIn("percent > 100u", body)

    def test_percent_to_delay_bounds(self) -> None:
        body = extract_source_function(read_text(SRC), "static uint16_t ac_phase_control_map_percent_to_delay(")
        self.assertIn("return group->config.min_delay_us", body)  # 100%
        self.assertIn("return group->config.max_delay_us", body)  # 0%
        self.assertIn("scaled = (uint32_t)delay_range * (uint32_t)(100u - percent)", body)

    def test_zero_cross_event_api_drives_half_cycle(self) -> None:
        body = extract_source_function(read_text(SRC), "void ac_phase_control_on_zero_cross_event(")
        self.assertIn("ac_phase_control_zero_cross_ref_const(group)", body)
        self.assertIn("zero_cross_is_alive(zero_cross)", body)
        self.assertIn("group->half_cycle_active = 1u;", body)
        self.assertIn("group->channels[channel].pulse_active = 0u;", body)

    def test_zero_cross_no_pulse_before_valid_sync(self) -> None:
        body = extract_source_function(read_text(SRC), "void ac_phase_control_on_zero_cross_event(")
        self.assertIn("zero_cross == (const zero_cross_t*)0", body)
        self.assertIn("zero_cross_is_alive(zero_cross) == 0u", body)
        self.assertIn("return;", body)

    def test_zero_cross_recovers_from_lost(self) -> None:
        body = extract_source_function(read_text(SRC), "void ac_phase_control_on_zero_cross_event(")
        self.assertIn("group->status == AC_PHASE_STATUS_ZERO_CROSS_LOST", body)
        self.assertIn("group->status = AC_PHASE_STATUS_OK", body)

    def test_bind_api_declared(self) -> None:
        text = read_text(HDR)
        self.assertIn("ac_phase_control_bind_zero_cross(", text)

    def test_bind_api_updates_group_pointer(self) -> None:
        body = extract_source_function(read_text(SRC), "drv_status_t ac_phase_control_bind_zero_cross(")
        self.assertIn("group->zero_cross = zero_cross;", body)
        self.assertIn("group->zero_cross_bound = 1u;", body)

    def test_zero_cross_legacy_wrapper_drives_detector(self) -> None:
        body = extract_source_function(read_text(SRC), "void ac_phase_control_on_zero_cross(")
        self.assertIn("zero_cross = ac_phase_control_zero_cross_ref(group);", body)
        self.assertIn("zero_cross_on_edge(zero_cross,", body)
        self.assertIn("ac_phase_control_on_zero_cross_event(group, &event)", body)

    def test_init_seeds_zero_cross_detector(self) -> None:
        body = extract_source_function(read_text(SRC), "drv_status_t ac_phase_control_init_group(")
        self.assertIn("zero_cross_init(&group->owned_zero_cross,", body)
        self.assertIn("AC_PHASE_CONTROL_ZERO_CROSS_RECOVERY_EVENTS", body)
        self.assertIn("AC_PHASE_CONTROL_DEFAULT_GLITCH_REJECT_US", body)

    def test_process_delegates_timeout_to_zero_cross(self) -> None:
        body = extract_source_function(read_text(SRC), "void ac_phase_control_process(")
        self.assertIn("zero_cross = ac_phase_control_zero_cross_ref(group);", body)
        self.assertIn("zero_cross_process(zero_cross,", body)
        self.assertIn("ZERO_CROSS_STATUS_LOST", body)
        self.assertIn("ac_phase_control_enter_fault(group)", body)

    def test_fault_clears_relays_and_outputs(self) -> None:
        body = extract_source_function(read_text(SRC), "static void ac_phase_control_enter_fault(")
        self.assertIn("AC_PHASE_STATUS_ZERO_CROSS_LOST", body)
        self.assertIn("ac_phase_control_apply_group_outputs(group)", body)

    def test_relay_step_hysteresis(self) -> None:
        body = extract_source_function(read_text(SRC), "static void ac_phase_control_relay_step(")
        self.assertIn("requested_on = (channel->power_percent >= group->config.relay_on_threshold_percent)", body)

    def test_relay_step_enforces_min_times_and_break(self) -> None:
        body = extract_source_function(read_text(SRC), "static void ac_phase_control_relay_step(")
        for guard in (
            "break_ms = group->config.relay_break_before_make_ms",
            "min_on_ms = group->config.relay_min_on_ms",
            "min_off_ms = group->config.relay_min_off_ms",
            "ac_phase_control_release_relay(channel, now_ms)",
        ):
            self.assertIn(guard, body)

    def test_relay_hold_suppresses_gate_pulses(self) -> None:
        body = extract_source_function(read_text(SRC), "static void ac_phase_control_collect_lat(")
        self.assertIn("entry->relay_state != (uint8_t)AC_PHASE_RELAY_STATE_PHASE", body)
        self.assertIn("entry->relay_active != 0u", body)

    def test_outputs_commit_once_per_lat(self) -> None:
        body = extract_source_function(read_text(SRC), "static void ac_phase_control_apply_group_outputs(")
        self.assertIn("already_processed", body)
        self.assertIn("lat_value &= (uint8_t)(~controlled_mask)", body)

    def test_all_off_disables_channels_and_relays(self) -> None:
        body = extract_source_function(read_text(SRC), "void ac_phase_control_all_off(")
        self.assertIn("entry->enabled = 0u", body)
        self.assertIn("ac_phase_control_channel_relay_off(entry)", body)
        self.assertIn("ac_phase_control_apply_group_outputs(group)", body)

    def test_status_api(self) -> None:
        text = read_text(SRC)
        self.assertIn("AC_PHASE_STATUS_NOT_INITIALIZED", extract_source_function(text, "ac_phase_status_t ac_phase_control_get_status("))
        body = extract_source_function(text, "uint8_t ac_phase_control_is_zero_cross_alive(")
        self.assertIn("ac_phase_control_zero_cross_ref_const(group)", body)
        self.assertIn("zero_cross_is_alive(zero_cross)", body)

    def test_gate_pulse_width_bounded(self) -> None:
        body = extract_source_function(read_text(SRC), "void ac_phase_control_update_us(")
        self.assertIn("pulse_elapsed >= (uint32_t)group->config.gate_pulse_us", body)
        self.assertIn("entry->pulse_active", body)

    def test_no_pulse_without_zero_cross(self) -> None:
        body = extract_source_function(read_text(SRC), "void ac_phase_control_update_us(")
        self.assertIn("group->half_cycle_active == 0u", body)

    def test_multichannel_loop_over_all_channels(self) -> None:
        text = read_text(SRC)
        for signature in (
            "void ac_phase_control_on_zero_cross_event(",
            "void ac_phase_control_update_us(",
        ):
            body = extract_source_function(text, signature)
            self.assertIn("channel < group->channel_count", body)
        collect = extract_source_function(text, "static void ac_phase_control_collect_lat(")
        self.assertIn("scan < group->channel_count", collect)
        self.assertIn("group->channels[scan]", collect)


@unittest.skipUnless(XC8 is not None, "XC8 toolchain not available")
@unittest.skipUnless(DFP.is_dir(), "PIC18 DFP not available")
class AcPhaseControlCompileTests(unittest.TestCase):
    def test_library_and_timer_compile(self) -> None:
        result = compile_library()
        self.assertEqual(result.returncode, 0, msg=result.stdout + result.stderr)

    def test_text_files_end_with_newline(self) -> None:
        for path in (HDR, SRC, HARNESS):
            with self.subTest(path=str(path.relative_to(ROOT))):
                self.assertTrue(read_bytes(path).endswith(b"\n"))


if __name__ == "__main__":
    unittest.main(verbosity=2)

from __future__ import annotations

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
LIB = ROOT / "libraries" / "actuator" / "cooldown_output"
HDR = LIB / "cooldown_output.h"
SRC = LIB / "cooldown_output.c"


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


class CooldownOutputHeaderTests(unittest.TestCase):
    def test_api_declared(self) -> None:
        text = read_text(HDR)
        for symbol in (
            "cooldown_output_init(",
            "cooldown_output_set_requested(",
            "cooldown_output_set_duration_ms(",
            "cooldown_output_cancel(",
            "cooldown_output_process(",
            "cooldown_output_is_active(",
            "cooldown_output_is_cooling_down(",
            "cooldown_output_get_remaining_ms(",
            "cooldown_output_force_off(",
            "cooldown_output_callback_t",
        ):
            self.assertIn(symbol, text)


class CooldownOutputBehaviorTests(unittest.TestCase):
    def test_init_and_safe_off_state(self) -> None:
        body = source_function(read_text(SRC), "drv_status_t cooldown_output_init(")
        self.assertIn("requested = 0u", body)
        self.assertIn("active = 0u", body)
        self.assertIn("cooling_down = 0u", body)

    def test_request_on_and_off_paths_exist(self) -> None:
        body = source_function(read_text(SRC), "void cooldown_output_set_requested(")
        for symbol in (
            "cooldown_output_normalize",
            "requested != 0u",
            "cooldown_ms == 0UL",
            "cooldown_end_ms = now_ms + output->cooldown_ms",
            "cooling_down = 1u",
        ):
            self.assertIn(symbol, body)

    def test_re_request_cancels_shutdown(self) -> None:
        body = source_function(read_text(SRC), "void cooldown_output_set_requested(")
        self.assertIn("output->cooling_down = 0u", body)

    def test_callback_only_on_state_change(self) -> None:
        body = source_function(read_text(SRC), "static void cooldown_output_notify(")
        self.assertIn("callback_context", body)
        self.assertIn("output->active", body)

    def test_remaining_time_is_wrap_safe(self) -> None:
        body = source_function(read_text(SRC), "uint32_t cooldown_output_get_remaining_ms(")
        self.assertIn("cooldown_end_ms - now_ms", body)
        self.assertIn("now_ms - output->cooldown_end_ms", body)

    def test_force_off_exists(self) -> None:
        body = source_function(read_text(SRC), "void cooldown_output_force_off(")
        self.assertIn("requested = 0u", body)
        self.assertIn("cooling_down = 0u", body)
        self.assertIn("cooldown_output_notify", body)

    def test_process_expiry_is_non_blocking(self) -> None:
        body = source_function(read_text(SRC), "void cooldown_output_process(")
        self.assertIn("cooling_down != 0u", body)
        self.assertIn("(int32_t)(now_ms - output->cooldown_end_ms) >= 0", body)

    def test_set_duration_updates_stored_duration(self) -> None:
        body = source_function(read_text(SRC), "void cooldown_output_set_duration_ms(")
        self.assertIn("output->cooldown_ms = duration_ms", body)

    def test_set_duration_recalculates_deadline_when_cooling(self) -> None:
        body = source_function(read_text(SRC), "void cooldown_output_set_duration_ms(")
        self.assertIn("output->cooling_down != 0u", body)
        self.assertIn("output->cooldown_end_ms = now_ms + duration_ms", body)

    def test_set_duration_immediate_off_on_zero(self) -> None:
        body = source_function(read_text(SRC), "void cooldown_output_set_duration_ms(")
        self.assertIn("duration_ms == 0UL", body)
        self.assertIn("output->active = 0u", body)
        self.assertIn("cooldown_output_notify", body)

    def test_cancel_preserves_active_state(self) -> None:
        body = source_function(read_text(SRC), "void cooldown_output_cancel(")
        self.assertIn("output->cooling_down = 0u", body)
        self.assertNotIn("output->active = 0u", body)

    def test_init_stores_cooldown_ms(self) -> None:
        body = source_function(read_text(SRC), "drv_status_t cooldown_output_init(")
        self.assertIn("output->cooldown_ms = config->cooldown_ms", body)


if __name__ == "__main__":
    unittest.main(verbosity=2)

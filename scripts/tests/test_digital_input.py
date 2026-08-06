from __future__ import annotations

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
LIB = ROOT / "libraries" / "input" / "digital_input"
HDR = LIB / "digital_input.h"
SRC = LIB / "digital_input.c"


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


class DigitalInputHeaderTests(unittest.TestCase):
    def test_api_and_config_declared(self) -> None:
        text = read_text(HDR)
        for symbol in (
            "digital_input_init(",
            "digital_input_update(",
            "digital_input_is_active(",
            "digital_input_rose(",
            "digital_input_fell(",
            "digital_input_is_latched(",
            "digital_input_clear_latch(",
            "debounce_ms",
            "active_level",
            "initial_raw_level",
            "latch_active",
            "activate_debounce_ms",
            "release_debounce_ms",
            "immediate_active",
        ):
            self.assertIn(symbol, text)


class DigitalInputBehaviorTests(unittest.TestCase):
    def test_init_validates_levels(self) -> None:
        body = source_function(read_text(SRC), "drv_status_t digital_input_init(")
        for symbol in (
            "active_level > 1u",
            "initial_raw_level > 1u",
            "latch_active > 1u",
            "DRV_STATUS_ERROR",
        ):
            self.assertIn(symbol, body)

    def test_active_high_and_low_mapping(self) -> None:
        body = source_function(read_text(SRC), "static uint8_t digital_input_to_active(")
        self.assertIn("raw_level == input->config.active_level", body)

    def test_debounce_and_wrap_safe_time(self) -> None:
        body = source_function(read_text(SRC), "void digital_input_update(")
        self.assertIn("now_ms - input->pending_since_ms", body)
        self.assertIn("debounce_ms", body)
        self.assertIn("digital_input_commit_level", body)

    def test_event_flags_are_consuming(self) -> None:
        rose = source_function(read_text(SRC), "uint8_t digital_input_rose(")
        fell = source_function(read_text(SRC), "uint8_t digital_input_fell(")
        self.assertIn("input->rose_flag = 0u", rose)
        self.assertIn("input->fell_flag = 0u", fell)

    def test_latch_support_exists(self) -> None:
        body = source_function(read_text(SRC), "static void digital_input_commit_level(")
        self.assertIn("latched = 1u", body)
        self.assertIn("digital_input_clear_latch", read_text(SRC))

    def test_safe_initial_state(self) -> None:
        body = source_function(read_text(SRC), "drv_status_t digital_input_init(")
        self.assertIn("initial_raw_level", body)
        self.assertIn("stable_active", body)
        self.assertIn("latched = 0u", body)

    def test_init_validates_immediate_active(self) -> None:
        body = source_function(read_text(SRC), "drv_status_t digital_input_init(")
        self.assertIn("immediate_active > 1u", body)

    def test_resolve_debounce_uses_symmetric_fallback(self) -> None:
        body = source_function(read_text(SRC), "static uint16_t digital_input_resolve_debounce(")
        self.assertIn("(act_ms == 0u) && (rel_ms == 0u)", body)
        self.assertIn("return input->config.debounce_ms", body)

    def test_resolve_debounce_immediate_active(self) -> None:
        body = source_function(read_text(SRC), "static uint16_t digital_input_resolve_debounce(")
        self.assertIn("immediate_active != 0u", body)
        self.assertIn("return 0u", body)

    def test_update_uses_resolved_debounce(self) -> None:
        body = source_function(read_text(SRC), "void digital_input_update(")
        self.assertIn("digital_input_resolve_debounce", body)
        self.assertIn("debounce_ms", body)

    def test_immediate_commit_on_zero_debounce(self) -> None:
        body = source_function(read_text(SRC), "void digital_input_update(")
        self.assertIn("if (debounce_ms == 0u)", body)
        self.assertIn("digital_input_commit_level(input, normalized_raw)", body)


if __name__ == "__main__":
    unittest.main(verbosity=2)

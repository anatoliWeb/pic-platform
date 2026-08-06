from __future__ import annotations

import re
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]

SHARED_HDR = ROOT / "drivers" / "communication" / "rs485" / "rs485.h"
SHARED_SRC = ROOT / "drivers" / "communication" / "rs485" / "rs485.c"
XC8_SRC = ROOT / "XC8" / "drivers" / "communication" / "rs485" / "rs485.c"
C18_SRC = ROOT / "C18" / "drivers" / "communication" / "rs485" / "rs485.c"
UART_HDR = ROOT / "drivers" / "communication" / "uart" / "uart.h"

FIXTURES = ROOT / "scripts" / "tests" / "fixtures"

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


class Rs485HeaderTests(unittest.TestCase):
    def test_include_guard(self) -> None:
        self.assertIn("DRIVERS_RS485_RS485_H", read_text(SHARED_HDR))

    def test_api_declared(self) -> None:
        text = read_text(SHARED_HDR)
        for declaration in (
            "void rs485_init(",
            "void rs485_set_tx(void)",
            "void rs485_set_rx(void)",
            "void rs485_send_byte(",
            "uint8_t rs485_read_byte(void)",
            "uint8_t rs485_send_frame(",
            "uint8_t rs485_receive_frame(",
            "uint8_t rs485_crc8(",
        ):
            self.assertIn(declaration, text)

    def test_depends_on_compiler_and_types(self) -> None:
        text = read_text(SHARED_HDR)
        self.assertIn("core/compiler.h", text)
        self.assertIn("core/types.h", text)


class Rs485SourceInclusionTests(unittest.TestCase):
    def test_shared_dispatcher_includes_xc8(self) -> None:
        text = read_text(SHARED_SRC)
        self.assertIn('#elif defined(DRV_COMPILER_XC8)', text)
        self.assertIn('XC8/drivers/communication/rs485/rs485.c', text)

    def test_shared_dispatcher_includes_c18(self) -> None:
        text = read_text(SHARED_SRC)
        self.assertIn('#if defined(DRV_COMPILER_C18)', text)
        self.assertIn('C18/drivers/communication/rs485/rs485.c', text)

    def test_no_duplicate_symbols_when_only_wrapper_used(self) -> None:
        xc8_text = read_text(XC8_SRC)
        c18_text = read_text(C18_SRC)

        for wrapper_text in (xc8_text, c18_text):
            self.assertIn(
                "static uint8_t rs485_wait_tx_complete",
                wrapper_text,
                "wait_tx_complete must be static uint8_t to avoid duplicate symbols",
            )


class Rs485TxCompleteTests(unittest.TestCase):
    def test_xc8_wrapper_has_trmt_polling(self) -> None:
        text = read_text(XC8_SRC)
        self.assertIn("rs485_wait_tx_complete", text)
        self.assertIn("TXSTAbits.TRMT", text)

    def test_xc8_wrapper_calls_wait_before_rx(self) -> None:
        text = read_text(XC8_SRC)
        body = extract_source_function(text, "uint8_t rs485_send_frame(")
        wait_pos = body.find("rs485_wait_tx_complete()")
        rx_pos = body.find("rs485_set_rx()")
        self.assertGreater(wait_pos, -1, "wait_tx_complete must be called")
        self.assertGreater(rx_pos, -1, "set_rx must be called")
        self.assertGreater(
            rx_pos, wait_pos,
            "set_rx must follow wait_tx_complete",
        )

    def test_c18_wrapper_has_trmt_polling(self) -> None:
        text = read_text(C18_SRC)
        self.assertIn("rs485_wait_tx_complete", text)
        self.assertIn("TXSTAbits.TRMT", text)

    def test_c18_wrapper_calls_wait_before_rx(self) -> None:
        text = read_text(C18_SRC)
        body = extract_source_function(text, "uint8_t rs485_send_frame(")
        wait_pos = body.find("rs485_wait_tx_complete()")
        rx_pos = body.find("rs485_set_rx()")
        self.assertGreater(wait_pos, -1, "wait_tx_complete must be called")
        self.assertGreater(rx_pos, -1, "set_rx must be called")
        self.assertGreater(
            rx_pos, wait_pos,
            "set_rx must follow wait_tx_complete",
        )

    def test_send_frame_rx_always_restored(self) -> None:
        for src in (XC8_SRC, C18_SRC, SHARED_SRC):
            body = extract_source_function(read_text(src), "uint8_t rs485_send_frame(")
            rx_pos = body.find("rs485_set_rx()")
            self.assertGreater(rx_pos, -1, f"set_rx must be called in {src.name}")

    def test_send_frame_returns_zero_on_invalid_args(self) -> None:
        for src in (XC8_SRC, C18_SRC, SHARED_SRC):
            body = extract_source_function(read_text(src), "uint8_t rs485_send_frame(")
            self.assertIn("return 0u", body)

    def test_send_frame_uses_result_variable(self) -> None:
        for src in (XC8_SRC, C18_SRC, SHARED_SRC):
            body = extract_source_function(read_text(src), "uint8_t rs485_send_frame(")
            self.assertIn("uint8_t result = 0u", body)
            self.assertIn("result = 1u", body)
            self.assertIn("return result", body)

    def test_send_frame_guard_delay_only_after_success(self) -> None:
        for src in (XC8_SRC, C18_SRC, SHARED_SRC):
            body = extract_source_function(read_text(src), "uint8_t rs485_send_frame(")
            wait_pos = body.find("rs485_wait_tx_complete()")
            delay_pos = body.find("DRV_DELAY_US(50u)", wait_pos + 1)
            self.assertGreater(wait_pos, -1)
            self.assertGreater(delay_pos, wait_pos, f"guard delay after wait in {src.name}")
            result_pos = body.find("result = 1u", wait_pos)
            self.assertGreater(result_pos, delay_pos, f"result=1u after guard delay in {src.name}")

class Rs485SharedFallbackTests(unittest.TestCase):
    def test_shared_fallback_requires_compiler_define(self) -> None:
        compiler_h = ROOT / "core" / "compiler.h"
        text = read_text(compiler_h)
        self.assertIn('#error "Unsupported compiler', text)

    def test_shared_dispatcher_always_dispatches(self) -> None:
        text = read_text(SHARED_SRC)
        self.assertIn("#if defined(DRV_COMPILER_C18)", text)
        self.assertIn("#elif defined(DRV_COMPILER_XC8)", text)
        self.assertIn("#else", text)

    def test_shared_fallback_has_trmt_as_safety_net(self) -> None:
        text = read_text(SHARED_SRC)
        fallback_start = text.find("#else")
        self.assertGreater(fallback_start, -1)
        fallback = text[fallback_start:]
        self.assertIn("rs485_wait_tx_complete", fallback)
        self.assertIn("TXSTAbits.TRMT", fallback)


class Rs485C18ConsistencyTests(unittest.TestCase):
    def test_c18_wrapper_follows_same_sfr_pattern_as_uart(self) -> None:
        uart_c18 = ROOT / "C18" / "drivers" / "communication" / "uart" / "uart.c"
        rs485_c18 = read_text(C18_SRC)
        uart_text = read_text(uart_c18)
        self.assertIn("TXSTAbits", rs485_c18)
        self.assertIn("TXSTAbits", uart_text)

    def test_c18_wrapper_does_not_include_device_header(self) -> None:
        text = read_text(C18_SRC)
        self.assertNotIn("<p18f", text)
        self.assertNotIn("<xc.h>", text)

    def test_c18_wait_tx_complete_is_static(self) -> None:
        text = read_text(C18_SRC)
        self.assertIn("static uint8_t rs485_wait_tx_complete", text)


class Rs485TrmtBoundedWaitTests(unittest.TestCase):
    def test_trmt_is_hardware_register(self) -> None:
        for src in (XC8_SRC, C18_SRC):
            text = read_text(src)
            self.assertIn("TXSTAbits.TRMT", text)

    def test_wait_tx_complete_returns_uint8_t(self) -> None:
        for src in (XC8_SRC, C18_SRC, SHARED_SRC):
            text = read_text(src)
            self.assertIn("static uint8_t rs485_wait_tx_complete", text)

    def test_wait_tx_complete_has_timeout_counter(self) -> None:
        for src in (XC8_SRC, C18_SRC, SHARED_SRC):
            text = read_text(src)
            body = extract_source_function(text, "uint8_t rs485_wait_tx_complete(")
            self.assertIn("RS485_TX_COMPLETE_TIMEOUT", body)
            self.assertIn("timeout", body)
            self.assertIn("timeout--", body)

    def test_wait_tx_complete_polls_trmt_with_timeout(self) -> None:
        for src in (XC8_SRC, C18_SRC, SHARED_SRC):
            text = read_text(src)
            body = extract_source_function(text, "uint8_t rs485_wait_tx_complete(")
            self.assertIn("TXSTAbits.TRMT", body)
            self.assertIn("timeout > 0u", body)

    def test_wait_tx_complete_returns_success_or_failure(self) -> None:
        for src in (XC8_SRC, C18_SRC, SHARED_SRC):
            text = read_text(src)
            body = extract_source_function(text, "uint8_t rs485_wait_tx_complete(")
            self.assertIn("return", body)

    def test_tx_complete_timeout_define_exists(self) -> None:
        for src in (XC8_SRC, C18_SRC, SHARED_SRC):
            text = read_text(src)
            self.assertIn("RS485_TX_COMPLETE_TIMEOUT", text)


class Rs485FrameOrderingTests(unittest.TestCase):
    def test_send_frame_null_returns_zero(self) -> None:
        for src in (SHARED_SRC, XC8_SRC, C18_SRC):
            body = extract_source_function(read_text(src), "uint8_t rs485_send_frame(")
            self.assertIn("data == (uint8_t*)0", body)
            self.assertIn("return 0u", body)

    def test_send_frame_zero_len_returns_zero(self) -> None:
        for src in (SHARED_SRC, XC8_SRC, C18_SRC):
            body = extract_source_function(read_text(src), "uint8_t rs485_send_frame(")
            self.assertIn("len == 0u", body)

    def test_send_frame_sets_tx_before_bytes(self) -> None:
        for src in (SHARED_SRC, XC8_SRC, C18_SRC):
            body = extract_source_function(read_text(src), "uint8_t rs485_send_frame(")
            tx_pos = body.find("rs485_set_tx()")
            byte_pos = body.find("rs485_send_byte(")
            self.assertGreater(tx_pos, -1, f"set_tx missing in {src.name}")
            self.assertGreater(byte_pos, -1, f"send_byte missing in {src.name}")
            self.assertGreater(
                byte_pos, tx_pos,
                f"set_tx must precede send_byte in {src.name}",
            )

    def test_send_frame_sends_start_len_payload_crc(self) -> None:
        for src in (SHARED_SRC, XC8_SRC, C18_SRC):
            text = read_text(src)
            self.assertIn("RS485_START_BYTE", text)
            self.assertIn("crc16_modbus", text)

    def test_send_frame_restores_rx(self) -> None:
        for src in (SHARED_SRC, XC8_SRC, C18_SRC):
            body = extract_source_function(read_text(src), "uint8_t rs485_send_frame(")
            self.assertIn("rs485_set_rx()", body)


class Rs485InitTests(unittest.TestCase):
    def test_init_sets_direction_output(self) -> None:
        for src in (SHARED_SRC, XC8_SRC, C18_SRC):
            body = extract_source_function(read_text(src), "void rs485_init(")
            self.assertIn("gpio_set_output", body)

    def test_init_starts_in_rx(self) -> None:
        for src in (SHARED_SRC, XC8_SRC, C18_SRC):
            body = extract_source_function(read_text(src), "void rs485_init(")
            self.assertIn("rs485_set_rx()", body)

    def test_set_tx_drives_pin_high(self) -> None:
        for src in (SHARED_SRC, XC8_SRC, C18_SRC):
            body = extract_source_function(read_text(src), "void rs485_set_tx(")
            self.assertIn("gpio_write_high", body)

    def test_set_rx_drives_pin_low(self) -> None:
        for src in (SHARED_SRC, XC8_SRC, C18_SRC):
            body = extract_source_function(read_text(src), "void rs485_set_rx(")
            self.assertIn("gpio_write_low", body)


class Rs485ReceiveFrameTests(unittest.TestCase):
    def test_receive_rejects_null_buffer(self) -> None:
        for src in (SHARED_SRC, XC8_SRC, C18_SRC):
            body = extract_source_function(read_text(src), "uint8_t rs485_receive_frame(")
            self.assertIn("buffer == (uint8_t*)0", body)
            self.assertIn("return 0u", body)

    def test_receive_rejects_zero_max_len(self) -> None:
        for src in (SHARED_SRC, XC8_SRC, C18_SRC):
            body = extract_source_function(read_text(src), "uint8_t rs485_receive_frame(")
            self.assertIn("max_len == 0u", body)

    def test_receive_calls_set_rx(self) -> None:
        for src in (SHARED_SRC, XC8_SRC, C18_SRC):
            body = extract_source_function(read_text(src), "uint8_t rs485_receive_frame(")
            self.assertIn("rs485_set_rx()", body)

    def test_receive_validates_crc(self) -> None:
        for src in (SHARED_SRC, XC8_SRC, C18_SRC):
            body = extract_source_function(read_text(src), "uint8_t rs485_receive_frame(")
            self.assertIn("crc16_modbus", body)
            self.assertIn("recv_crc != calc_crc", body)


class Rs485BaudRateTests(unittest.TestCase):
    def test_no_fixed_delay_as_tx_complete_guard(self) -> None:
        for src in (XC8_SRC, C18_SRC, SHARED_SRC):
            text = read_text(src)
            body = extract_source_function(text, "uint8_t rs485_send_frame(")
            has_wait = "rs485_wait_tx_complete()" in body
            self.assertTrue(
                has_wait,
                f"{src.name} send_frame must use wait_tx_complete, not fixed delay",
            )

    def test_guard_delay_preserved_after_wait(self) -> None:
        for src in (XC8_SRC, C18_SRC, SHARED_SRC):
            text = read_text(src)
            body = extract_source_function(text, "uint8_t rs485_send_frame(")
            wait_pos = body.find("rs485_wait_tx_complete()")
            delay_pos = body.find("DRV_DELAY_US(50u)", wait_pos + 1)
            self.assertGreater(wait_pos, -1)
            self.assertGreater(
                delay_pos, wait_pos,
                f"guard delay after wait_tx_complete in {src.name}",
            )

    def test_send_frame_uses_bounded_wait(self) -> None:
        for src in (XC8_SRC, C18_SRC, SHARED_SRC):
            text = read_text(src)
            body = extract_source_function(text, "uint8_t rs485_send_frame(")
            self.assertIn("if (rs485_wait_tx_complete() != 0u)", body)


class Rs485ErrorContractTests(unittest.TestCase):
    def test_receive_timeout_returns_zero(self) -> None:
        for src in (SHARED_SRC, XC8_SRC, C18_SRC):
            body = extract_source_function(read_text(src), "uint8_t rs485_receive_frame(")
            self.assertIn("timeout == 0u", body)
            self.assertIn("return 0u", body)

    def test_receive_crc_mismatch_returns_zero(self) -> None:
        for src in (SHARED_SRC, XC8_SRC, C18_SRC):
            body = extract_source_function(read_text(src), "uint8_t rs485_receive_frame(")
            self.assertIn("recv_crc != calc_crc", body)
            self.assertIn("return 0u", body)


class Rs485DocumentationConsistencyTests(unittest.TestCase):
    def test_docs_mention_trmt(self) -> None:
        docs_en = ROOT / "docs" / "drivers" / "communication" / "rs485.md"
        docs_ua = ROOT / "docs" / "drivers" / "communication" / "rs485.ua.md"
        for doc in (docs_en, docs_ua):
            text = read_text(doc)
            self.assertIn("TRMT", text, f"{doc.name} must document TRMT-based wait")

    def test_docs_describe_guard_delay(self) -> None:
        docs_en = ROOT / "docs" / "drivers" / "communication" / "rs485.md"
        docs_ua = ROOT / "docs" / "drivers" / "communication" / "rs485.ua.md"
        for doc in (docs_en, docs_ua):
            text = read_text(doc)
            self.assertIn("50", text, f"{doc.name} must document guard delay")


class Rs485SourceInclusionNoDuplicationTests(unittest.TestCase):
    def test_xc8_wrapper_not_added_alongside_shared(self) -> None:
        xc8_conf = (
            ROOT / "examples-projects" / "xc8" / "communication" / "rs485"
            / "basic.X" / "nbproject" / "configurations.xml"
        )
        text = read_text(xc8_conf)
        source_items = [
            item.text for item in
            __import__("xml.etree.ElementTree", fromlist=["ET"]).fromstring(text).iter("itemPath")
            if item.text and item.text.endswith(".c")
        ]
        rs485_sources = [s for s in source_items if "rs485" in s.lower()]
        self.assertEqual(
            len(rs485_sources), 1,
            f"exactly one rs485 source allowed, found: {rs485_sources}",
        )
        self.assertIn("rs485.c", rs485_sources[0])
        self.assertNotIn("XC8/drivers", rs485_sources[0])


@unittest.skipUnless(XC8 is not None, "XC8 toolchain not available")
@unittest.skipUnless(DFP.is_dir(), "PIC18 DFP not available")
class Rs485CompileTests(unittest.TestCase):
    def _compile_harness(self, harness: Path) -> subprocess.CompletedProcess[str]:
        out_dir = Path(tempfile.gettempdir()) / "rs485_test_builds"
        out_dir.mkdir(parents=True, exist_ok=True)
        staged = out_dir / harness.name
        staged.write_bytes(harness.read_bytes())
        args = [
            f"-mcpu={MCU}",
            f"-mdfp={DFP}",
            f"-I{ROOT}",
            f"-D{CLOCK_DEFINE}",
            "-O0",
            str(staged),
            "-o",
            str(out_dir / "rs485_out.hex"),
        ]
        return run_xc8(args)

    def test_shared_dispatcher_compiles(self) -> None:
        harness = FIXTURES / "rs485_shared_harness.c"
        if not harness.exists():
            self.skipTest(f"harness not found: {harness}")
        result = self._compile_harness(harness)
        self.assertEqual(result.returncode, 0, msg=result.stdout + result.stderr)

    def test_xc8_wrapper_compiles(self) -> None:
        harness = FIXTURES / "rs485_xc8_harness.c"
        if not harness.exists():
            self.skipTest(f"harness not found: {harness}")
        result = self._compile_harness(harness)
        self.assertEqual(result.returncode, 0, msg=result.stdout + result.stderr)


if __name__ == "__main__":
    unittest.main(verbosity=2)

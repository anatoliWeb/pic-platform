from __future__ import annotations

import argparse
import os
import re
import shutil
import subprocess
import sys
import tempfile
import xml.etree.ElementTree as ET
from pathlib import Path


CATEGORY_A = {
    "PIC_PLATFORM_CLOCK_HZ",
    "DRV_DEBUG_ENABLE",
    "DRV_DEBUG_BACKEND_UART",
    "DRV_DEBUG_BACKEND_DISPLAY",
    "DRV_DEBUG_BACKEND_PINS",
    "DRV_DEBUG_LEVEL",
    "DRV_DEBUG_DISPLAY_TYPE_LCD_2X16",
    "DRV_DEBUG_DISPLAY_INTERFACE_I2C",
    "DRV_DEBUG_DISPLAY_INTERFACE_PARALLEL",
    "DRV_DEBUG_DISPLAY_AUTO_INIT",
    "DRV_DEBUG_DISPLAY_I2C_ADDR",
    "DRV_DEBUG_DISPLAY_I2C_FREQ",
    "DRV_DEBUG_PINS_INTERFACE_GPIO",
    "DRV_DEBUG_PINS_INTERFACE_I2C",
    "DRV_DEBUG_PINS_PORT",
    "DRV_DEBUG_PINS_TRIS",
    "DRV_DEBUG_PINS_START_BIT",
    "DRV_DEBUG_PINS_CHANNEL_COUNT",
    "DRV_DEBUG_PINS_PULSE_US",
    "LCD_I2C_PIN_RS",
    "LCD_I2C_PIN_RW",
    "LCD_I2C_PIN_EN",
    "LCD_I2C_PIN_BL",
    "LCD_I2C_DATA_SHIFT",
    "SEVEN_SEGMENT_ENABLE_TIMER0",
    "SEVEN_SEGMENT_ENABLE_TIMER1",
    "SEVEN_SEGMENT_ENABLE_TIMER2",
    "SEVEN_SEGMENT_ENABLE_TIMER3",
    "POSITION_DRIVE_SENSOR_TYPE",
    "POSITION_DRIVE_ENABLE_PWM",
    "POSITION_DRIVE_ENABLE_TIMEOUT",
    "POSITION_DRIVE_ENABLE_STUCK_DETECTION",
    "POSITION_DRIVE_ENABLE_DIRECTION_CHECK",
    "POSITION_DRIVE_ENABLE_UART_DEBUG",
}


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8-sig")


def parse_define_lines(text: str) -> dict[str, str]:
    defines: dict[str, str] = {}
    pattern = re.compile(r"^\s*#define\s+([A-Za-z_][A-Za-z0-9_]*)\s+(.+?)\s*(?:/\*.*)?$")
    for line in text.splitlines():
        match = pattern.match(line)
        if match is None:
            continue
        defines[match.group(1)] = match.group(2).strip()
    return defines


def xml_root(project_dir: Path) -> ET.Element:
    return ET.fromstring(read_text(project_dir / "nbproject" / "configurations.xml"))


def xml_property(root: ET.Element, key: str) -> str:
    for prop in root.iter("property"):
        if prop.get("key") == key:
            return prop.get("value", "")
    raise KeyError(key)


def project_target(root: ET.Element) -> str:
    target = root.find(".//targetDevice")
    if target is None or not (target.text or "").strip():
        raise ValueError("targetDevice not found in configurations.xml")
    value = target.text.strip()
    return value[3:] if value.startswith("PIC") else value


def parse_sources(project_dir: Path, root: ET.Element) -> list[Path]:
    source_folder = root.find(".//logicalFolder[@name='SourceFiles']")
    if source_folder is None:
        raise ValueError("SourceFiles folder not found in configurations.xml")
    sources: list[Path] = []
    for item in source_folder.iter("itemPath"):
        if not item.text:
            continue
        if not item.text.lower().endswith(".c"):
            continue
        sources.append((project_dir / Path(item.text)).resolve())
    return sources


def parse_include_dirs(project_dir: Path, root: ET.Element) -> list[Path]:
    base_dir = project_dir / "nbproject"
    raw = xml_property(root, "extra-include-directories")
    includes: list[Path] = []
    for item in raw.split(";"):
        item = item.strip()
        if not item:
            continue
        item = item.replace("..core", "..\\core")
        item = item.replace("..drivers", "..\\drivers")
        item = item.replace("..libraries", "..\\libraries")
        includes.append((base_dir / Path(item)).resolve())
    return includes


def parse_define_macros(root: ET.Element) -> dict[str, str]:
    raw = xml_property(root, "define-macros")
    macros: dict[str, str] = {}
    for item in raw.split(";"):
        if not item:
            continue
        name, _, value = item.partition("=")
        macros[name] = value
    return macros


def parse_project_defines(project_dir: Path) -> dict[str, str]:
    return parse_define_lines(read_text(project_dir / "project_config.h"))


def find_dfp(root: ET.Element) -> Path:
    packs = root.find(".//packs")
    if packs is None:
        raise ValueError("packs metadata not found in configurations.xml")
    pack = packs.find("pack")
    if pack is None:
        raise ValueError("pack metadata not found in configurations.xml")
    name = pack.get("name")
    vendor = pack.get("vendor")
    version = pack.get("version")
    if not name or not vendor or not version:
        raise ValueError("pack metadata incomplete in configurations.xml")

    candidates: list[Path] = []
    env_roots = [os.environ.get("MPLABX_PACKS_DIR"), os.environ.get("MPLABX_HOME")]
    for env_root in env_roots:
        if env_root:
            candidates.append(Path(env_root))

    candidates.extend(
        [
            Path(r"C:\Program Files\Microchip\MPLABX"),
            Path(r"C:\Program Files (x86)\Microchip\MPLABX"),
        ]
    )

    for base in candidates:
        packs_dir = base / "packs" if base.name.lower() != "packs" else base
        direct = packs_dir / vendor / name / version / "xc8"
        if direct.is_dir():
            return direct
        for match in base.glob(f"**/packs/{vendor}/{name}/{version}/xc8"):
            if match.is_dir():
                return match
        for match in packs_dir.glob(f"**/{vendor}/{name}/{version}/xc8"):
            if match.is_dir():
                return match

    raise FileNotFoundError(f"DFP pack not found for {vendor}/{name}/{version}")


def build_command(root: ET.Element, dfp: Path, sources: list[Path], includes: list[Path], macros: dict[str, str]) -> list[str]:
    cmd = [
        "xc8",
        f"-mcpu={project_target(root)}",
        f"-mdfp={dfp}",
        "-O0",
    ]
    cmd.extend(f"-I{path}" for path in includes)
    cmd.extend(f"-D{name}={value}" for name, value in macros.items())
    cmd.extend(str(path) for path in sources)
    return cmd


def summarize_output(text: str) -> dict[str, list[str]]:
    warnings = [line for line in text.splitlines() if "warning" in line.lower()]
    rom = [line for line in text.splitlines() if re.search(r"(program memory|program space|code size)", line, re.IGNORECASE)]
    ram = [line for line in text.splitlines() if re.search(r"(data memory|data space|sram)", line, re.IGNORECASE)]
    return {"warnings": warnings, "rom": rom, "ram": ram}


def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(description="Build an XC8 project from tracked MPLAB metadata.")
    parser.add_argument("project", type=Path, help="Path to a .X project directory")
    parser.add_argument("--out-dir", type=Path, default=None, help="Output directory for build artifacts")
    args = parser.parse_args(argv)

    project_dir = args.project.resolve()
    root = xml_root(project_dir)
    sources = parse_sources(project_dir, root)
    includes = parse_include_dirs(project_dir, root)
    macros = parse_define_macros(root)
    project_defines = parse_project_defines(project_dir)

    conflicts = sorted(name for name in CATEGORY_A if name in project_defines)
    if conflicts:
        print(f"CONFLICT project_config.h duplicates Category A macros: {', '.join(conflicts)}", file=sys.stderr)
        return 2

    if shutil.which("xc8") is None:
        print("xc8 not found on PATH", file=sys.stderr)
        return 2

    dfp = find_dfp(root)
    out_dir = args.out_dir or (Path(tempfile.gettempdir()) / "xc8-builds" / project_dir.name)
    out_dir.mkdir(parents=True, exist_ok=True)
    hex_path = out_dir / f"{project_dir.name}.hex"

    command = build_command(root, dfp, sources, includes, macros)
    command.extend(["-o", str(hex_path)])

    result = subprocess.run(
        subprocess.list2cmdline(command),
        cwd=project_dir,
        shell=True,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
    )
    output = result.stdout + result.stderr
    summary = summarize_output(output)

    print(f"PROJECT={project_dir}")
    print(f"COMMAND={subprocess.list2cmdline(command)}")
    print(f"RESULT={result.returncode}")
    print(f"HEX={hex_path}")
    print(f"SOURCES={', '.join(str(path) for path in sources)}")
    print(f"DEFINES={', '.join(f'{name}={value}' for name, value in macros.items())}")
    print(f"CATEGORY_A={', '.join(f'{name}={macros[name]}' for name in macros if name in CATEGORY_A)}")
    print(f"ROM={'; '.join(summary['rom']) if summary['rom'] else 'unknown'}")
    print(f"RAM={'; '.join(summary['ram']) if summary['ram'] else 'unknown'}")
    print(f"WARNINGS={'; '.join(summary['warnings']) if summary['warnings'] else 'none'}")

    if result.returncode != 0:
        print(output, file=sys.stderr)

    return result.returncode


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

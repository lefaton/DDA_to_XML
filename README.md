# DDA_Reader — Ducati DDA File Reader

A C++17 command-line tool that reads the proprietary binary data files produced by the **Ducati DDA** (Data Acquisition Device) and converts them to human-readable XML. Originally written to support statistics analysis and virtual dashboard overlays for onboard video footage.

Tested on a 2014 Ducati Streetfighter 848, using a DDA unit originally from a 1198 SBK (bought on eBay — the 1198 metadata is still embedded in the file headers).

---

## Background & Philosophy

The Ducati DDA is a small black box that plugs into the bike's diagnostic port and records telemetry during a ride: speed, RPM, throttle position, engine temperature, lap count, distance, and traction control activity. The files it produces are compact binary blobs with no official documentation.

This project is built around a single guiding principle: **the parsing logic should never need to be recompiled when a new DDA firmware version is released**. All format knowledge — header size, parameter order, bit widths, update rates, and value transforms — lives in a single XML definition file (`DDA_Definition.xml`). Adding support for a new DDA version is a one-line XML edit.

The C++ code is intentionally thin: it reads the definition, walks the binary stream according to those rules, and writes XML output. No magic numbers, no hardcoded offsets.

---

## How the DDA Binary Format Works

### File structure

```
[ Header (variable size) ][ Body (repeating frames) ]
```

The **first byte** of the file encodes the DDA firmware version. The header size varies by version (22 bytes for v1/v2, 296 for v3, 1070 for v4). Bytes 2–4 of the header always contain the ASCII signature `DDA`.

The **body** is a stream of binary frames recorded at **100 Hz** (one frame every 10 ms). However, not every parameter is updated every frame — each parameter has its own update rate defined by its `interval` attribute:

| Parameter   | Size    | Update rate | Transform         |
|-------------|---------|-------------|-------------------|
| speed       | 16 bits | 10 Hz       | raw ÷ 4           |
| rpm         | 16 bits | 50 Hz       | none              |
| temperature | 8 bits  | 1 Hz        | raw − 64          |
| throttle    | 8 bits  | 20 Hz       | none              |
| lap         | 8 bits  | 1 Hz        | 255 → 0 (no lap)  |
| distance    | 24 bits | 1 Hz        | none              |
| dtc (v3+)   | 8 bits  | 20 Hz       | none              |

When a parameter is not updated in a given frame, the **last known value is carried forward** into the XML output so every entry has a complete set of values.

### The frame-count calculation

A subtle but important detail: some frames consume **zero bytes** (when none of the parameters update on that tick). A naive `while (position < fileSize)` loop would exit too early on those frames, silently dropping them. The correct approach is to pre-compute `bytes_per_cycle` from the definition and derive `total_frames = (body_size / bytes_per_cycle) × frequency`. This is what the parser does.

### Value transforms

Each parameter can optionally define an arithmetic transform applied after reading the raw binary value:

| Operator | Effect                                     | Example                          |
|----------|--------------------------------------------|----------------------------------|
| `div`    | `raw / value`                              | speed: raw/4 → km/h units        |
| `sub`    | `raw - value`                              | temperature: raw−64 → °C offset  |
| `mul`    | `raw × value`                              | —                                |
| `add`    | `raw + value`                              | —                                |
| `dif`    | `0` if `raw == value`, else `raw`          | lap: 255 means "no lap" → 0      |

---

## XML Definition File

`DDA_Definition.xml` is the heart of the system. It describes every known DDA firmware version:

```xml
<DDA_Definitions>
  <DDA_Definition version="4" frequency="100" clock="0.01" headersize="1070">
    <EntryToRead>
      <Param name="speed"       bitsize="16" interval="0.10" operator="div" value="4"/>
      <Param name="rpm"         bitsize="16" interval="0.02"/>
      <Param name="temperature" bitsize="8"  interval="1.0"  operator="sub" value="64"/>
      <Param name="throttle"    bitsize="8"  interval="0.05"/>
      <Param name="lap"         bitsize="8"  interval="1.0"  operator="dif" value="255"/>
      <Param name="distance"    bitsize="24" interval="1.0"/>
      <Param name="dtc"         bitsize="8"  interval="0.05"/>
    </EntryToRead>
  </DDA_Definition>
</DDA_Definitions>
```

**To add a new DDA version**, add a new `<DDA_Definition>` block. No recompilation needed.

> **Tip for reverse engineering a new version:** Start with `distance` (your total bike mileage) — it's a 24-bit value that's easy to identify because it barely changes between rides and has a known approximate value you can compare against the real Ducati tool.

---

## XML Output Format

Each converted DDA file becomes an XML file with one `<entry>` per frame (100 entries per second):

```xml
<DDA_file version="4" frequency="100">
  <entry timing="0.00">
    <speed>87</speed>
    <rpm>6240</rpm>
    <temperature>92</temperature>
    <throttle>34</throttle>
    <lap>0</lap>
    <distance>12453</distance>
    <dtc>0</dtc>
  </entry>
  <entry timing="0.01">
    ...
  </entry>
</DDA_file>
```

The `timing` attribute gives the elapsed time in seconds with 2 decimal places. From XML you can easily convert to CSV, JSON, or import into any analysis tool.

---

## Code Architecture

```
DDA_Definition.xml        ← format knowledge (no recompile needed)
DDA_FileFormat.h/.cpp     ← loads the XML definition into structs
DDA_Operators.h           ← ApplyOperator() — single source of truth
DDA_Parser.h/.cpp         ← reads the binary, calls ApplyOperator, writes XML
main.cpp                  ← entry point
tests/                     ← Catch2 unit + integration tests
CMakeLists.txt            ← cross-platform build (Windows / macOS / Linux)
```

**`CDDA_FileFormat`** parses `DDA_Definition.xml` and populates an `SDDADefinition` struct containing the list of `SDDAParam` entries. It is the only component that touches the XML definition file.

**`CDDAParser`** reads the binary DDA file, drives the frame loop, reads the right number of bytes for each parameter at the right cadence, applies the operator transform via `ApplyOperator()`, and writes the output XML using pugixml.

**`DDA_Operators.h`** exposes a single `inline` function `ApplyOperator()` used by both the parser at runtime and the test suite. Having one canonical implementation prevents the tests from testing a different version of the logic than what ships.

---

## Build

### Prerequisites

- CMake 3.16+
- A C++17-capable compiler (GCC, Clang, MSVC)
- Internet access for the first build (Catch2 is fetched automatically)

### Build steps

```bash
git clone https://github.com/lefaton/DDA_to_XML.git
cd DDA_to_XML
mkdir build && cd build
cmake .. -DBUILD_TESTING=ON
cmake --build .
```

### Run

```bash
./DDA_Reader path/to/your/file.dda
```

The output XML is written next to the input file with a `.xml` extension.

---

## Tests

The test suite uses **Catch2 v3** (downloaded automatically by CMake via `FetchContent`) and is structured in three files:

### `tests/test_fileformat.cpp` — Definition loading

Verifies that `CDDA_FileFormat` correctly parses `DDA_Definition.xml`:

- Version 0 is rejected
-- Versions 1–4 load with the correct header size and frequency
- Each parameter has the right name, bit size, update interval, operator and value
- v3 and v4 include the `dtc` parameter
- Unknown versions return `false`

### `tests/test_operators.cpp` — Transform logic

Tests `ApplyOperator()` exhaustively for every operator:

- `nil` — value passes through unchanged
- `add`, `sub`, `mul`, `div` — basic arithmetic
- `div` by zero — returns raw value unchanged (no crash)
- `dif` — sentinel value (255 for `lap`) maps to 0; other values pass through

### `tests/test_parser.cpp` — Integration tests

Builds synthetic DDA binary blobs in memory, writes them to temp files, parses them, and inspects the resulting XML:

- Missing file → returns `false`
- Empty file → returns `false`
- Bad `DDA` signature → returns `false`
- Unknown DDA version → returns `false`
- Valid v1 file → correct XML values (`speed=100`, `rpm=8000`, `lap=0` after dif transform, etc.)
- **Regression test for the frame-count bug**: 3 seconds of v1 data (300 frames) must produce exactly 300 `<entry>` tags. This test catches the old `while (seekPos < fileSize)` bug that silently dropped frames when the last frame of a cycle consumed 0 bytes.

### Running the tests

```bash
cd build
ctest --output-on-failure
# or run the binary directly for verbose output:
./dda_tests -v
```

---

## Credits

- Original reverse engineering based on the work of **Andrew Allan** (2009), whose white paper and notes on the DDA format were invaluable. See [ducati.ms](http://www.ducati.ms/).
- XML parsing powered by [pugixml](http://pugixml.org) — Copyright (C) 2006–2015 Arseny Kapoulkine.

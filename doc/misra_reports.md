# Project MISRA C:2012 Static Analysis Report

| Report field | Value |
|:--|:--|
| Analysis date | 2026-09-22 |
| Project | micro_core |
| Scope | All production C sources under `src/` and included project headers |
| Translation units | 11 |
| Safety context | ISO 26262-6, ASIL-C project policy |
| Analyzer | cppcheck 2.19.0 with MISRA addon |
| Language/profile | C99, `unix64` |
| Result | **PASS — 0 unsuppressed findings** |

## Executive summary

The complete production source tree passes the project MISRA gate after direct
remediation and application of the reviewed, narrowly scoped deviations in
`tools/misra/suppressions.txt`.

| Category | Unsuppressed | Policy | Status |
|:--|--:|:--|:--|
| Mandatory | 0 | No violations or deviations permitted | PASS |
| Required | 0 | Must comply or have a documented deviation | PASS |
| Advisory | 0 | Must remediate or formally disposition | PASS |
| Unknown | 0 | Rule-category mapping required | PASS |

```text
MISRA: 0 Mandatory, 0 Required, 0 Advisory, 0 Unknown.
ASIL-C MISRA gate PASSED (no Mandatory/Required violations).
```

## Results by module

| Module | Production sources | Mandatory | Required | Advisory | Unknown | Status |
|:--|--:|--:|--:|--:|--:|:--|
| Core/time | 1 | 0 | 0 | 0 | 0 | PASS |
| Algorithms | 1 | 0 | 0 | 0 | 0 | PASS |
| DSA | 2 | 0 | 0 | 0 | 0 | PASS |
| Chain pattern | 1 | 0 | 0 | 0 | 0 | PASS |
| I/O communication | 5 | 0 | 0 | 0 | 0 | PASS |
| I/O message | 1 | 0 | 0 | 0 | 0 | PASS |
| **Project total** | **11** | **0** | **0** | **0** | **0** | **PASS** |

## Raw observation inventory

A diagnostic run without the project suppression list reported 173 raw
observations. Existing line-level inline annotations remained active. These are
not open gate findings: each is mapped to DEV-001 through DEV-010 in
`doc/safety/deviation_records.md`.

| Category | Raw observations |
|:--|--:|
| Mandatory | 0 |
| Required | 39 |
| Advisory | 134 |
| **Total** | **173** |

### Raw observations by rule

| Rule | Category | Count | Disposition |
|:--|:--|--:|:--|
| 5.5 | Required | 8 | DEV-001: constructor macro/type naming |
| 18.7 | Required | 8 | DEV-002: flexible caller-buffer storage |
| 5.6 | Required | 4 | DEV-003: local dispatch typedefs |
| 11.3 | Required | 15 | DEV-004: typed headers in caller storage |
| 11.9 | Required | 3 | DEV-006: analyzer interpretation of `NULL` |
| 21.1 | Required | 1 | DEV-007: POSIX feature-test macro |
| 8.7 | Advisory | 30 | DEV-008: externally consumed public APIs |
| 11.5 | Advisory | 27 | DEV-009: validated generic-storage boundaries |
| 15.5 | Advisory | 77 | DEV-010: resource-free guard clauses |

### Raw observations by source area

| Source area | Required | Advisory | Total |
|:--|--:|--:|--:|
| Public headers | 10 | 0 | 10 |
| Core/time | 0 | 1 | 1 |
| Algorithms | 0 | 0 | 0 |
| DSA | 11 | 65 | 76 |
| Chain pattern | 1 | 11 | 12 |
| I/O communication | 9 | 24 | 33 |
| I/O message | 8 | 33 | 41 |
| **Total** | **39** | **134** | **173** |

The two Rule 11.8 deviations in DEV-005 are line-scoped inline annotations and
therefore do not appear in this raw inventory.

## Verification performed

| Check | Result |
|:--|:--|
| CMake configure with compile database | PASS |
| Complete project build | PASS |
| Algorithm functional tests | PASS |
| Memlin functional tests | PASS |
| Sorted-array functional tests (20 cases) | PASS |
| DSA span functional tests | PASS |
| MISRA project gate | PASS |

## Commands to run and check MISRA

Configure, build, and run the gate from the repository root:

```sh
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build -j2
tools/misra/run_misra.sh build
```

The gate returns `0` on success, `1` when a Mandatory or Required finding
remains, and `2` for setup errors. Review its generated outputs with:

```sh
cat build/misra/summary.md
less build/misra/misra.txt
```

| Artifact | Purpose |
|:--|:--|
| `build/misra/summary.md` | Category totals and gate decision |
| `build/misra/misra.xml` | Machine-readable complete analyzer output |
| `build/misra/misra.txt` | Human-readable analyzer output |
| `tools/misra/suppressions.txt` | Scoped deviations applied by the gate |
| `doc/safety/deviation_records.md` | Rationale, impact, and mitigation |

If `build/compile_commands.json` is absent, the script falls back to scanning
`src/` with `include/`. The compile-database workflow is preferred because it
preserves the actual translation-unit flags.

## Qualification and limitations

This is a Parasoft-style report, not Parasoft output, a qualified-tool record,
or an ISO 26262 certification claim. The licensed MISRA rule text file was not
present, so cppcheck reported rule identifiers without copyrighted headlines.
Formal safety-manager approval of deviations and tool qualification remain
release activities.

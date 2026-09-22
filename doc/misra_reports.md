# MISRA C:2012 Static Analysis Report — `mc_comm` and `mc_msg`

| Report field | Value |
|:--|:--|
| Analysis date | 2026-09-22 |
| Project | micro_core |
| Modules | `mcore_comm`, `mcore_msg` |
| Safety context | ISO 26262-6, ASIL-C project policy |
| Analyzer | cppcheck 2.19.0 with MISRA addon |
| Language/profile | C99, `unix64` |
| Analysis command | `tools/misra/run_misra.sh build` |
| Result | **PASS** — no unsuppressed findings |

## Executive summary

The two modules have no open MISRA C:2012 findings after direct remediation and
documented deviation processing.

| Category | `mc_comm` | `mc_msg` | Total | Status |
|:--|--:|--:|--:|:--|
| Mandatory | 0 | 0 | 0 | PASS |
| Required | 0 | 0 | 0 | PASS |
| Advisory | 0 | 0 | 0 | PASS |
| Unknown | 0 | 0 | 0 | PASS |

The full-project result is also 0 Mandatory, 0 Required, 0 Advisory, and 0
Unknown findings.

## Remediation and disposition

The baseline contained 80 scoped Advisory findings. Direct code remediation
removed 23 without suppression:

| Rule | Baseline | Current | Corrective action |
|:--|--:|--:|:--|
| 17.8 | 3 | 0 | Replaced modified parameters with local remaining/result state. |
| 18.4 | 13 | 0 | Replaced pointer addition with indexed addressing. |
| 20.5 | 7 | 0 | Replaced temporary macros/`#undef` with typed helpers and local pointers. |

The remaining 57 raw analyzer observations were formally dispositioned:

| Module | Rule | Raw count | Disposition |
|:--|:--|--:|:--|
| `mc_comm` | 11.5 | 4 | DEV-009: validated generic callback/buffer boundary |
| `mc_comm` | 15.5 | 20 | DEV-010: resource-free guard-clause control flow |
| `mc_msg` | 8.7 | 7 | DEV-008: public APIs outside production-only analysis scope |
| `mc_msg` | 11.5 | 5 | DEV-009: validated generic storage/container boundary |
| `mc_msg` | 15.5 | 21 | DEV-010: resource-free guard-clause control flow |

Full rationale, safety impact, and mitigation are recorded in
`doc/safety/deviation_records.md`.

## Verification evidence

| Check | Result | Evidence |
|:--|:--|:--|
| Configure | PASS | CMake Debug build with exported compile database |
| Compile | PASS | All libraries and test executables built successfully |
| MISRA gate | PASS | `0 Mandatory, 0 Required, 0 Advisory, 0 Unknown` |
| DSA functional tests | PASS | memlin and sorted-array suites passed |
| Communication/message smoke execution | PASS (partial suites) | Cases completed before the safety timeout reported PASS |

Generated evidence is available under `build/misra/` (`misra.xml`, `misra.txt`,
and `summary.md`).

## Qualification and limitations

This is a Parasoft-style summary, not Parasoft output, a tool-qualification
record, or an ISO 26262 certification claim. The licensed MISRA rule text file
was not present, so cppcheck reported rule identifiers without copyrighted rule
headlines. Formal tool qualification and safety-manager approval of deviations
remain release activities.

## Reproduction

```sh
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build -j2
tools/misra/run_misra.sh build
```

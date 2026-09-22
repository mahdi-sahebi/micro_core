# Static Analysis Report (MISRA C:2012, ASIL-C)

_Generated with `tools/misra/run_misra.sh` using cppcheck and its MISRA addon._

The detailed whole-project results, module breakdown, raw observation inventory,
and execution commands are maintained in [`../misra_reports.md`](../misra_reports.md).

## Gate result

```text
11 files checked.
MISRA: 0 Mandatory, 0 Required, 0 Advisory, 0 Unknown.
ASIL-C MISRA gate PASSED (no Mandatory/Required violations).
```

| Category | Unsuppressed findings | Policy | Status |
|:--|--:|:--|:--|
| Mandatory | 0 | must be 0 | PASS |
| Required | 0 | must be 0 or formally deviated | PASS |
| Advisory | 0 | remediate or formally disposition | PASS |
| Unknown | 0 | category mapping required | PASS |

## Deviations

The analyzer applies the narrowly scoped entries in
`tools/misra/suppressions.txt`. Every entry maps to a rationale, safety impact,
and mitigation in `doc/safety/deviation_records.md`. Mandatory rules are not
deviated.

## Reproduction

```sh
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build -j2
tools/misra/run_misra.sh build
```

Generated evidence is written to `build/misra/`.

To inspect the gate decision directly:

```sh
cat build/misra/summary.md
```

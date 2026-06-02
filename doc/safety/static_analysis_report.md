# Static Analysis Report (MISRA C:2012, ASIL-C)

_Generated from the MISRA C:2012 gate (`tools/misra/run_misra.sh`, cppcheck with
the MISRA addon). Records the result of the static-analysis pass on this branch._

## What this report shows

This is the static-analysis evidence for the ISO 26262-6 ASIL-C safety case. The
gate enforces the project policy: **zero Mandatory and zero Required** MISRA
violations, with every permitted exception captured as a documented deviation in
`tools/misra/suppressions.txt`. Advisory findings are reported only and tracked
for later cleanup. Re-running `tools/misra/run_misra.sh` reproduces every number
below.

## Gate result

```
22 files checked.
MISRA: 0 Mandatory, 0 Required, 180 Advisory, 0 Unknown.
ASIL-C MISRA gate PASSED (no Mandatory/Required violations).
```

| Category  | Violations | Policy                              | Status |
|:--|--:|:--|:--|
| Mandatory | 0          | must be 0                           | PASS   |
| Required  | 0          | must be 0 (or documented deviation) | PASS   |
| Advisory  | 180        | reported only                       | INFO   |

All Mandatory and Required rules pass. The build is clean against the ASIL-C
policy.

## Advisory findings (180, non-blocking)

These do not block the gate. They are tracked for opportunistic cleanup.

| Rule | Count | Topic |
|:--|--:|:--|
| 15.5 | 84 | multiple return points (single-exit) |
| 8.7  | 31 | functions could have internal linkage |
| 11.5 | 28 | conversion from `void*` to object pointer |
| 18.4 | 19 | pointer arithmetic with `+`/`-` |
| 20.5 | 7  | use of `#undef` |
| 2.3  | 3  | unused type declarations |
| 17.8 | 3  | function parameter modified |
| 5.9  | 2  | identifier reuse with internal linkage |
| 15.4 | 1  | more than one break/goto in a loop |
| 2.5  | 1  | unused macro declaration |
| 2.7  | 1  | unused function parameter |

## How to reproduce

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
tools/misra/run_misra.sh build      # -> build/misra/summary.md
```

# Stack Usage Report (ISO 26262-6, worst-case stack depth)

_Generated from the stack-usage gate (`tools/stack_usage/run_stack_usage.sh`, GCC
`-fstack-usage`). Records the per-function worst-case stack frame size for every
function under `src/` on this branch._

## What this report shows

Worst-case stack usage is part of the ISO 26262-6 safety case: on resource-constrained
targets, unbounded or oversized stack frames risk overflow with no diagnosable failure
mode. GCC's `-fstack-usage` emits one `.su` file per translation unit with the exact
frame size the compiler allocated for each function, plus a `dynamic` qualifier when the
frame size is not a compile-time constant (e.g. a variable-length array). `test/` sources
are excluded - this report only covers the library code under `src/` that ships to a
target. Re-running `tools/stack_usage/run_stack_usage.sh` reproduces every number below.

## Gate result

```
90 functions analysed (src/ only).
Largest frame: wndpool_write - 112 bytes (src/io/communication/mc_window_pool.c:169)
Dynamic/unbounded frames: 3 (all dynamic,bounded - see below)
Frames over 512 bytes: 0
```

| Metric                    | Value | Policy                                | Status |
|:--|--:|:--|:--|
| Frames over 512 bytes     | 0     | flag for review                       | PASS   |
| Unbounded dynamic frames  | 0     | must be 0 (VLA/alloca with no bound)  | PASS   |
| Bounded dynamic frames    | 3     | reported only, size is constant-bound | INFO   |

No function under `src/` exceeds 512 bytes of stack, and no frame is genuinely
unbounded. The 3 `dynamic,bounded` frames below are GCC's way of saying it could not
constant-fold the frame size at the `.su`-emission stage, even though every allocation
is driven by a `sizeof`/constant argument - not a true VLA or `alloca`.

## Bounded-dynamic frames (non-blocking)

| Function | Bytes | Location |
|:--|--:|:--|
| `mc_msg_req_size` | 80 | src/io/message/mc_message.c:121 |
| `mc_msg_init`     | 80 | src/io/message/mc_message.c:144 |
| `mc_comm_init`    | 56 | src/io/communication/mc_communication.c:72 |

## Top frames by size

| Function | Bytes | Location |
|:--|--:|:--|
| `wndpool_write`          | 112 | src/io/communication/mc_window_pool.c:169 |
| `mc_comm_recv`           | 96  | src/io/communication/mc_communication.c:105 |
| `mc_comm_send`           | 96  | src/io/communication/mc_communication.c:135 |
| `mc_memlin_alloc`        | 80  | src/dsa/memory/memlin.c:234 |
| `mc_msg_req_size`        | 80  | src/io/message/mc_message.c:121 |
| `mc_msg_init`            | 80  | src/io/message/mc_message.c:144 |
| `mc_alg_lower_bound`     | 80  | src/alg/algorithm.c:6 |
| `mc_alg_nearest`         | 80  | src/alg/algorithm.c:46 |
| `mc_chain_run`           | 80  | src/pattern/mc_chain.c:56 |
| `mc_memlin_create`       | 64  | src/dsa/memory/memlin.c:205 |

## How to reproduce

```sh
tools/stack_usage/run_stack_usage.sh                 # -> build-stack-usage/stack-usage/summary.md
tools/stack_usage/run_stack_usage.sh <build_dir> <threshold_bytes>
```

Under the hood this configures a dedicated build with `-DMC_STACK_USAGE=ON` (adds
GCC's `-fstack-usage` to every compile), builds it, then parses the resulting `.su`
files with `tools/stack_usage/summarize.py`, filtered to `src/`. The full per-function
CSV is written alongside the summary at `<build_dir>/stack-usage/stack_usage.csv`.

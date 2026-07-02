# Stack Usage Report (ISO 26262-6, worst-case stack depth)

_Generated from the stack-usage gate (`tools/stack_usage/run_stack_usage.sh`, GCC
`-fstack-usage`). Records the per-function worst-case stack frame size for every
function under `src/`, broken down per module, on this branch._

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
90 functions analysed across 7 modules (src/ only).
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

## Per-module breakdown

| Module | Functions | Largest frame |
|:--|--:|:--|
| `src/alg`             | 3  | `mc_alg_lower_bound` - 80 bytes (src/alg/algorithm.c:6) |
| `src/core/time`       | 4  | `mc_now` - 48 bytes (src/core/time/time_linux.c:12) |
| `src/dsa`             | 13 | `mc_sarray_insert` - 48 bytes (src/dsa/sarray.c:128) |
| `src/dsa/memory`      | 24 | `mc_memlin_alloc` - 80 bytes (src/dsa/memory/memlin.c:234) |
| `src/io/communication`| 32 | `wndpool_write` - 112 bytes (src/io/communication/mc_window_pool.c:169) |
| `src/io/message`      | 9  | `mc_msg_req_size` - 80 bytes (src/io/message/mc_message.c:121) |
| `src/pattern`         | 5  | `mc_chain_run` - 80 bytes (src/pattern/mc_chain.c:56) |

### `src/alg` (3 functions)

| Function | Bytes | Location |
|:--|--:|:--|
| `mc_alg_lower_bound`  | 80 | src/alg/algorithm.c:6 |
| `mc_alg_nearest`      | 80 | src/alg/algorithm.c:46 |
| `mc_alg_crc16_ccitt`  | 8  | src/alg/algorithm.c:70 |

### `src/core/time` (4 functions)

| Function  | Bytes | Location |
|:--|--:|:--|
| `mc_now`   | 48 | src/core/time/time_linux.c:12 |
| `mc_now_u` | 48 | src/core/time/time_linux.c:20 |
| `mc_now_m` | 48 | src/core/time/time_linux.c:25 |
| `mc_now_s` | 48 | src/core/time/time_linux.c:30 |

### `src/dsa` (13 functions)

| Function | Bytes | Location |
|:--|--:|:--|
| `mc_sarray_insert`        | 48 | src/dsa/sarray.c:128 |
| `mc_sarray_find`          | 32 | src/dsa/sarray.c:109 |
| `mc_sarray_remove_at`     | 32 | src/dsa/sarray.c:148 |
| `mc_sarray_remove`        | 32 | src/dsa/sarray.c:163 |
| `mc_sarray_required_size` | 8  | src/dsa/sarray.c:30 |
| `mc_sarray_init`          | 8  | src/dsa/sarray.c:39 |
| `mc_sarray_clear`         | 8  | src/dsa/sarray.c:59 |
| `mc_sarray_get_count`     | 8  | src/dsa/sarray.c:70 |
| `mc_sarray_get_capacity`  | 8  | src/dsa/sarray.c:79 |
| `mc_sarray_get_data_size` | 8  | src/dsa/sarray.c:88 |
| `mc_sarray_get`           | 8  | src/dsa/sarray.c:97 |
| `mc_sarray_is_empty`      | 8  | src/dsa/sarray.c:188 |
| `mc_sarray_is_full`       | 8  | src/dsa/sarray.c:197 |

### `src/dsa/memory` (24 functions)

| Function | Bytes | Location |
|:--|--:|:--|
| `mc_memlin_alloc`        | 80 | src/dsa/memory/memlin.c:234 |
| `mc_memlin_create`       | 64 | src/dsa/memory/memlin.c:205 |
| `mc_memlin_is_full`      | 64 | src/dsa/memory/memlin.c:269 |
| `mc_memlin_clear`        | 48 | src/dsa/memory/memlin.c:250 |
| `mc_memlin_is_empty`     | 48 | src/dsa/memory/memlin.c:260 |
| `mc_memlin_get_capacity` | 48 | src/dsa/memory/memlin.c:278 |
| `mc_memlin_get_size`     | 48 | src/dsa/memory/memlin.c:287 |
| `mc_memlin_get_meta_size`| 32 | src/dsa/memory/memlin.c:296 |
| `mc_memlin_destroy`      | 8  | src/dsa/memory/memlin.c:224 |
| `init_08/16/32`          | 8  | src/dsa/memory/memlin.c:76-88 |
| `get_capacity_08/16/32`  | 8  | src/dsa/memory/memlin.c:94-104 |
| `get_size_08/16/32`      | 8  | src/dsa/memory/memlin.c:121-131 |
| `set_size_08/16/32`      | 8  | src/dsa/memory/memlin.c:148-158 |
| `get_data_08/16/32`      | 8  | src/dsa/memory/memlin.c:175-187 |

### `src/io/communication` (32 functions)

| Function | Bytes | Qualifier | Location |
|:--|--:|:--|:--|
| `wndpool_write`        | 112 | static          | src/io/communication/mc_window_pool.c:169 |
| `mc_comm_recv`         | 96  | static          | src/io/communication/mc_communication.c:105 |
| `mc_comm_send`         | 96  | static          | src/io/communication/mc_communication.c:135 |
| `mc_comm_init`         | 56  | dynamic,bounded | src/io/communication/mc_communication.c:72 |
| `wndpool_read`         | 48  | static          | src/io/communication/mc_window_pool.c:146 |
| `frame_recv`           | 48  | static          | src/io/communication/mc_frame.c:36 |
| `io_send`              | 48  | static          | src/io/communication/mc_io.c:25 |
| `wndpool_update_header`| 32  | static          | src/io/communication/mc_window_pool.c:117 |
| `frame_init`           | 32  | static          | src/io/communication/mc_frame.c:29 |
| `send_ack`             | 32  | static          | src/io/communication/mc_protocol.c:12 |
| `protocol_recv`        | 32  | static          | src/io/communication/mc_protocol.c:52 |
| `protocol_send_unacked`| 32  | static          | src/io/communication/mc_protocol.c:87 |
| `io_recv`              | 32  | static          | src/io/communication/mc_io.c:12 |
| `mc_comm_update`       | 32  | static          | src/io/communication/mc_communication.c:93 |
| `mc_comm_flush`        | 32  | static          | src/io/communication/mc_communication.c:170 |
| `wndpool_update`       | 16  | static          | src/io/communication/mc_window_pool.c:92 |
| `frame_send`           | 16  | static          | src/io/communication/mc_frame.c:59 |
| `mc_comm_req_size` (+13 more single-digit frames) | 8 | static | src/io/communication/*.c |

### `src/io/message` (9 functions)

| Function | Bytes | Qualifier | Location |
|:--|--:|:--|:--|
| `mc_msg_req_size`   | 80 | dynamic,bounded | src/io/message/mc_message.c:121 |
| `mc_msg_init`       | 80 | dynamic,bounded | src/io/message/mc_message.c:144 |
| `mc_msg_update`     | 64 | static          | src/io/message/mc_message.c:175 |
| `mc_msg_send`       | 64 | static          | src/io/message/mc_message.c:231 |
| `mc_msg_subscribe`  | 48 | static          | src/io/message/mc_message.c:204 |
| `mc_msg_unsubscribe`| 48 | static          | src/io/message/mc_message.c:217 |
| `mc_msg_signal`     | 32 | static          | src/io/message/mc_message.c:257 |
| `id_compare`        | 8  | static          | src/io/message/mc_message.c:38 |
| `mc_msg_flush`      | 8  | static          | src/io/message/mc_message.c:276 |

### `src/pattern` (5 functions)

| Function | Bytes | Location |
|:--|--:|:--|
| `mc_chain_run`           | 80 | src/pattern/mc_chain.c:56 |
| `mc_chain_get_alloc_size`| 8  | src/pattern/mc_chain.c:5 |
| `mc_chain_init`          | 8  | src/pattern/mc_chain.c:15 |
| `mc_chain_clear`         | 8  | src/pattern/mc_chain.c:27 |
| `mc_chain_push`          | 8  | src/pattern/mc_chain.c:37 |

## How to reproduce

```sh
tools/stack_usage/run_stack_usage.sh                 # -> build-stack-usage/stack-usage/summary.md
tools/stack_usage/run_stack_usage.sh <build_dir> <threshold_bytes>
```

Under the hood this configures a dedicated build with `-DMC_STACK_USAGE=ON` (adds
GCC's `-fstack-usage` to every compile), builds it, then parses the resulting `.su`
files with `tools/stack_usage/summarize.py`, filtered to `src/` and grouped per module.
The full per-function CSV is written alongside the summary at
`<build_dir>/stack-usage/stack_usage.csv`, and the generated Markdown
(`<build_dir>/stack-usage/summary.md`) contains the complete, ungrouped per-function
tables for every module - this document mirrors that output for the committed
snapshot.

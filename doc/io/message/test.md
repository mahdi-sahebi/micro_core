# Test Report — mc_msg (Message Layer)

Scope: verification & validation of mc_msg (ID-based message layer built on mc_comm) using the UDP-based harness (test_common.c, sender_test.c, receiver_test.c, runner).
Note: tests intentionally simulate real-world bad channel behaviour — delays, packet dropping, bit corruption, duplication, jitter, re-ordering, periodic and long-term disconnections.
Purpose: demonstrate deterministic, timeout-based APIs under stress and provide evidence aligned with ASIL-B safety design principles.
<br>


## Why UDP?

* UDP provides **no guarantees** — no ordering, no retransmission, no flow control.  
* This makes it ideal to simulate noisy hardware links (UART, RS485, proprietary radio and also UDP).  
* Additionally, **socket buffer size is deliberately set small** to increase the chance of loss and force the protocol to exercise retransmit and error handling paths.

<br>


## Table of Contents

<br>


## Test environment & tools

- Host: Ubuntu Linux  
- Build: GCC + CMake (Debug builds)  
- Test runner: `./build/test/io/message/test_message_udp`  
- Tools:
  - **Valgrind** (memcheck) — runtime memory correctness
  - **gcov** — code coverage (instructions below)
  - Harness knobs in `test_common.c`:
    - `cfg_set_loss_rate(rate)` — random packet loss/corruption
    - `cfg_set_repetitive_send(true)` — duplicate frames
    - `cfg_set_periodic_duration(ms)` — periodic link up/down
    - `cfg_set_iterations(n)` — number of test iterations

<br>


## Static analysis

*(TODO — to be filled later with cppcheck / clang-tidy / MISRA notes)*

<br>



## Test scenarios

All scenarios use the sender/receiver harness and the same basic message patterns (string, two large message sizes, tiny boolean, and a zero-length signal). Each scenario description below includes how inputs are set and what the test asserts.

Common API contract under test:
- `mc_msg_send(...)`: must transmit a message header + payload (or return a timeout/error).
- `mc_msg_signal(...)`: must send a zero-length header-only message.
- `mc_msg_update(...)`: must read headers, collect body bytes into the recv_pool, and dispatch callbacks when a full message is available.
All public APIs are heap-free, timeout-aware and must return deterministic error codes for invalid arguments.

#### 1. invalid_creation
- **Condition:** Call mc_msg_init() with invalid configs (NULL IO callbacks, zero window/pool sizes, too-small allocation buffer, or NULL buffer pointer).
- **Expected:** mc_msg_init() must return MC_ERR_INVALID_ARGUMENT or MC_ERR_BAD_ALLOC. No crash, no partial initialization.

#### 2. valid_creation
- **Condition:** Initialize mc_msg with a valid mc_msg_cfg and static buffer, after querying mc_msg_req_size().
- **Expected:** mc_msg_init() returns MC_SUCCESS with a non-null handle. All sub-objects (including embedded mc_comm) are placed correctly in the buffer.

#### 3. singly_direction (clean link)
- **Condition:** Link configured with 0% loss, no duplication. Pool size sufficient for all message types.
- **Expected:** Sending different payloads (9-byte string, 32×uint32 array, 189-byte buffer, 1-byte boolean, and a signal) — all received correctly, in order, and validated by callbacks. RecvFailedCounter == 0.

#### 4. singly_repetitive (duplicate frames)
- **Condition:** Harness set with cfg_set_repetitive_send(true) to duplicate on-wire frames.
- **Expected:** mc_msg (via mc_comm) suppresses duplicate delivery. Each unique message triggers its callback exactly once. Counters confirm duplicates were sent but filtered.

#### 5. singly_low_lossy (moderate loss + short disconnect)
- **Condition:** Loss rate set to ~20%, with intermittent short disconnects.
- **Expected:** Retransmissions succeed, partial reassemblies handled, and pool exhaustion triggers drop_message() as designed. Delivered messages pass all callbacks. Some failed send/recv counts are expected but bounded.

#### 6. singly_high_lossy (extreme loss + long disconnect)
- **Condition:** Loss rate ~98%, with extended downtime.
- **Expected:** System responds with timeouts (not hangs), maintains forward progress, and ensures no leaks/corruptions. Process terminates cleanly. Valgrind shows zero memory errors.
<br>
<br>



## Test Execution (Debug)

```bash
[MICRO CORE 1.0.0 - IO - MESSAGE]
[invalid_creation]
PASSED - 5(us)

[valid_creation]
PASSED - 4(us)

[singly_direction]
████████████████████ 100.0%
[IO] Completed{Recv: 3606, Send: 3080} - Failed{Recv: 0(0.00%), Send: 0(0.00%)} - Throughput: 3.44 KBps
PASSED - 4408970(us)

[singly_repetitive]
████████████████████ 100.0%
[IO] Completed{Recv: 289859, Send: 447219} - Failed{Recv: 0(0.00%), Send: 0(0.00%)} - Throughput: 0.22 KBps
PASSED - 60249809(us)

[singly_low_lossy]
████████████████████ 100.0%
[IO] Completed{Recv: 231436, Send: 304902} - Failed{Recv: 46431(20.06%), Send: 32257(10.58%)} - Throughput: 0.29 KBps
PASSED - 47092283(us)

[singly_high_lossy]
████████████████████ 100.0%
[IO] Completed{Recv: 22551, Send: 32919} - Failed{Recv: 22116(98.07%), Send: 21361(64.89%)} - Throughput: 0.37 KBps
PASSED - 21296544(us)
```

#### Results (Debug)

| Test case         | #Recv   | #Send   | Recv<br>failed  | Send<br>failed  | Throughput<br>(KBps) | Result |
| :--               | :--:    | :--:    | :--:            | :--:            | :--:                 | :--:   | 
| invalid_creation	| 0       | 0       | 0               | 0               | 0                    | PASSED |
| valid_creation	  | 0       | 0       | 0               | 0               | 0                    | PASSED |
| singly_direction  | 3,642	  | 3,124	  | 0 (0.00%)       | 0 (0.00%)	      | 2.59	               | PASSED |
| singly_repetitive | 355,950	| 596,980	| 0 (0.00%)       | 0 (0.00%)	      | 0.09	               | PASSED |
| singly_low_lossy  | 241,812	| 350,618	| 48,270 (19.96%) | 37,051 (10.57%) |	0.13                 | PASSED |
| singly_high_lossy | 25,918	| 37,771  | 25,390 (97.96%) | 24,495 (64.85%) |	0.23                 | PASSED |

#### Notes:
Under extreme loss the system degrades gracefully (expected). The pass condition is stable behavior + no crashes + deterministic error returns — which is met.


#### Failure rate

```
singly_direction  : ░ 0.00% 
singly_repetitive : ░ 0.00% 
singly_low_lossy  : ██████ 19.96% 
singly_high_lossy : █████████████████████ 97.96%
```

#### Throughput (Debug)

```
singly_direction  : ███████████ 2.59 KBps 
singly_repetitive : █ 0.09 KBps 
singly_low_lossy  : ███ 0.13 KBps 
singly_high_lossy : █ 0.23 KBps
```


## Detailed scenario walkthroughs — how internals are exercised

#### 1. Header read & small-pool handling

mc_msg_update() calls mc_comm_update() then attempts to read the message header into recv_pool.
read_message_header() can return early if only partial header is available; test harness jitter ensures partial reads occur and are handled.

#### 2. drop_message() path

When a message size exceeds recv_pool capacity, tests force the drop path: drop_message() will consume remaining bytes but return MC_ERR_NO_SPACE. singly_low_lossy exercises this path. The test verifies this returns cleanly (no memory corruption) and the receiver continues.

#### 3. is_message_stored() and reassembly

When header is present and pool can hold the message, is_message_stored() calls into mc_comm_recv() repeatedly until the expected payload bytes are stored. Tests with send_large_1 and send_large_2 validate correct reassembly and content verification.

#### 4. Subscription & dispatch

mc_msg_subscribe() usage is validated in valid_creation() and receiver init; on_receive() call path is exercised in all positive scenarios. Tests assert that id_node lookup and callback invocation are correct and occur only when the full message is available.

#### 5. Signal messages

mc_msg_signal() sends header-only messages (size=0) and on_signal_received checks that mc_buffer_is_empty() is true. This path is covered in all passing tests.



<br>
<br>

# Analysis

## Run-time analyzer (Valgrind)

Command:

```bash
valgrind --tool=memcheck --track-origins=yes ./build/test/io/message/test_message_udp 
==6527== Memcheck, a memory error detector
==6527== Copyright (C) 2002-2022, and GNU GPL'd, by Julian Seward et al.
==6527== Using Valgrind-3.22.0 and LibVEX; rerun with -h for copyright info
==6527== Command: ./build/test/io/message/test_message_udp
==6527== 
[MICRO CORE 1.0.0 - IO - MESSAGE]
[invalid_creation]
PASSED - 12716(us)

[valid_creation]
PASSED - 10722(us)

[singly_direction]
████████████████████ 100.0%			
[IO] Completed{Recv: 3642, Send: 3124} - Failed{Recv: 0(0.00%), Send: 0(0.00%)} - Throughput: 2.59 KBps
PASSED - 5811899(us)

[singly_repetitive]
████████████████████ 100.0%			
[IO] Completed{Recv: 355950, Send: 596980} - Failed{Recv: 0(0.00%), Send: 0(0.00%)} - Throughput: 0.09 KBps
PASSED - 149303345(us)

[singly_low_lossy]
████████████████████ 100.0%			
[IO] Completed{Recv: 241812, Send: 350618} - Failed{Recv: 48270(19.96%), Send: 37051(10.57%)} - Throughput: 0.13 KBps
PASSED - 98161683(us)

[singly_high_lossy]
████████████████████ 100.0%			
[IO] Completed{Recv: 25918, Send: 37771} - Failed{Recv: 25390(97.96%), Send: 24495(64.85%)} - Throughput: 0.23 KBps
PASSED - 31710959(us)

==6527== 
==6527== HEAP SUMMARY:
==6527==     in use at exit: 0 bytes in 0 blocks
==6527==   total heap usage: 3 allocs, 3 frees, 1,568 bytes allocated
==6527== 
==6527== All heap blocks were freed -- no leaks are possible
==6527== 
==6527== For lists of detected and suppressed errors, rerun with: -s
==6527== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)

```
- No memory leaks
- No invalid reads/writes
- Heap-free operation as designed


<br>
<br>

## Static analyzer




<br>
<br>

#### Bad Situations Simulated

| Bad situation           | API Config                           | What Simulated                             | Direction    |
| :--:                    | :--                                  | :--                                        | :--:         |
| Packet Loss             | `cfg_set_loss_rate()`                | Packet dropping                            | Send/Receive |
| Data Corruption         | `cfg_set_loss_rate()`                | Bit-flip                                   | Send/Receive |
| Duplication             | `cfg_set_repetitive_send(true)`      | Sending Duplicated Frames                  | Send/Receive |
| Re-ordering             | UDP + `usleep()` jitter              | Delay the ack to retransmit                | Send/Receive |
| Full Frame              | UDP + `usleep()` jitter              | Delayed acked to fill the pools            | Send/Receive |
| Periodic Disconnect     | `cfg_set_periodic_duration(ms)`      | Disable sending/receive + clear UDP buffer | Send/Receive |


<br>
<br>

## ASIL-B requirement mapping

| Safety requirement (ASIL-B)              | Test(s) covering it      | Evidence                       | Status |
| :--                                      | :--                      | :--                            | :--:   |
| Fault detection (corruption)             | low_lossy, high_lossy    | CRC rejection, counters        | ✓      |
| Robustness under packet loss/disconnect  | low_lossy, high_lossy    | Logs, throughput/failure stats | ✓      |
| Deterministic response (bounded timeout) | all (timeout-based APIs) | Timeout returns observed       | ✓      |
| No memory safety issues                  | all (Valgrind memcheck)  | Valgrind 0 errors, no leaks    | ✓      |
| Defensive init / safe failure            | invalid_creation         | Error return, no partial init  | ✓      |



<br>
<br>

## Code coverage

Regenerate the build files and re-run the test:
```bash
cmake ../ -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_FLAGS="-fprofile-arcs -ftest-coverage -O0 -g"
cmake --build ./test/io/message/
./test/io/message/test_message_udp
```

**`mc_message.c`**:
```bash
gcov ./build/src/io/message/CMakeFiles/mcore_msg.dir/mc_message.c.
File '/media/mahdi/common/repositories/micro_core/src/io/message/mc_message.c'
Lines executed:86.82% of 129
Creating 'mc_message.c.gcov'

Lines executed:86.82% of 129
```



<br>
<br>

## Performance benchmarks



<br>
<br>

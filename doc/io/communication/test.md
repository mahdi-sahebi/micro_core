# Test Report — mc_comm (Communication Core)

**Scope:** Verification & validation of `mc_comm` (sliding-window transport) using the UDP-based harness (`test_common.c`, `test_sender.c`, `test_receiver.c`, runner).  
**Note:** Tests intentionally simulate real-world *bad channel behaviour* — delays, packet dropping, data corruption, duplication, jitter, reordering, periodic and long-term disconnections.  
**Purpose:** Demonstrate deterministic, timeout-based APIs under stress conditions, aligned with **ASIL-B safety requirements**.

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
- Test runner: `./build/test/io/communication/test_comm_udp`  
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
  
#### 1. invalid_creation
- **What:** Initialize with NULL callbacks / invalid window sizes
- **Expected:** Fail safely (return error, no partial init)

#### 2. invalid_argument
- **What:** Pass invalid andnot-allowed arguments in the APIs
- **Expected:** No crash and entering in bad state, return the MC_ERR_INVALID_ARGUMENT error

#### 3. valid_creation
- **What:** Proper config with static buffer
- **Expected:** Success, valid handle created

#### 4. singly_direction
- **Condition:** No loss, no duplication, no disconnects, sending frame != receieving frame
- **Expected:** 100% success, in-order delivery, 0% failure

#### 5. singly_repetitive
- **Condition:** Duplicates frames enabled, sending frame != receieving frame
- **Expected:** No double delivery, duplicates suppressed

#### 6. singly_low_lossy
- **Condition:** ~20% packet loss(send/receive), periodic short disconnects, sending frame != receieving frame
- **Expected:** Retransmit + recovery, bounded timeouts

#### 7. singly_high_lossy
- **Condition:** ~98% packet loss(send/receive), periodic long disconnects, sending frame != receieving frame
- **Expected:** Graceful degradation, bounded timeouts, no crash/leak

#### 8. singly_timed_out
- **Condition:** Short time-out to perform incomplete for operations **send**, **receive**, and **flush**.
- **Expected:** no crash and stop operation in that state.


<br>
<br>

## Test Execution (Debug)

```
/test/io/communication/test_comm_udp 
[MICRO CORE 1.0.0 - IO - COMMUNICATION]
[invalid_creation]
PASSED - 3(us)

[invalid_argument]
PASSED - 3(us)

[valid_creation]
PASSED - 0(us)

[singly_direction]
████████████████████ 100.0%			
[IO] Completed{Recv: 847, Send: 783} - Failed{Recv: 0(0.00%), Send: 0(0.00%)} - Throughput: 981.27 KBps
PASSED - 927196(us)

[singly_repetitive]
████████████████████ 100.0%			
[IO] Completed{Recv: 846, Send: 784} - Failed{Recv: 0(0.00%), Send: 0(0.00%)} - Throughput: 994.46 KBps
PASSED - 915875(us)

[singly_low_lossy]
████████████████████ 100.0%			
[IO] Completed{Recv: 1721, Send: 1681} - Failed{Recv: 345(20.05%), Send: 351(20.88%)} - Throughput: 193.41 KBps
PASSED - 6359812(us)

[singly_high_lossy]
████████████████████ 100.0%			
[IO] Completed{Recv: 49380, Send: 77338} - Failed{Recv: 48531(98.28%), Send: 75768(97.97%)} - Throughput: 2.35 KBps
PASSED - 184952978(us)

[singly_timed_out]
PASSED - 832(us)
```

#### Results (Debug)

| Test case	        | Iterations | Recv	  |  Send | Recv<br>failed | Send<br>failed | Throughput<br>(KBps) | Result |
| :--               | :--:       | :--:   | :--:  | :--:           | :--:           | :--:                 | :--:   |
| invalid_creation	| 1          | 0      | 0     | 0              | 0              | 0                    | ✓      |
| valid_creation	  | 1          | 0      | 0     | 0              | 0              | 0                    | ✓      |
| singly_direction	| 200        | 929    | 874   | 0 (0.00%)      | 0 (0.00%)	    | 393.4                | ✓      |
| singly_repetitive	| 200        | 936    | 883   | 0 (0.00%)      | 0 (0.00%)	    | 442.9                | ✓      |
| singly_low_lossy	| 200        | 1771   | 1702  | 372 (21.0%)    | 321 (18.9%)  	| 117.5                | ✓      |
| singly_high_lossy	| 100        | 16770  | 26322 | 16438 (98%)    | 25769 (98%)  	| 6.1                  | ✓      |
| singly_timed_out	| 1          | 0      | 0     | 0              | 0             	| 0                    | ✓      |

#### Failure rate

```
singly_direction   | █ 0.0%
singly_repetitive  | █ 0.0%
singly_low_lossy   | ███████████ 21%
singly_high_lossy  | ██████████████████████████████ 98%
```

#### Throughput (Debug)

```
singly_direction   : ████████████████████ 393 KBps
singly_repetitive  : █████████████████████ 443 KBps
singly_low_lossy   : ███████ 117 KBps
singly_high_lossy  : █ 6 KBps
```


#### Network congestion

Observed:
- Retransmit floods the channel and expected congestion.

Suggested improvements:
- Throughput is maintained by selective retransmit instead of bursts
- Adaptive sending time separately for each frame





<br>
<br>

# Analysis

## Run-time analyzer (Valgrind)

Command:

```bash
valgrind --track-origins=yes --tool=memcheck ./build/test/io/communication/test_comm_udp

HEAP SUMMARY:
    in use at exit: 0 bytes in 0 blocks
  total heap usage: 3 allocs, 3 frees, 1,568 bytes allocated

All heap blocks were freed -- no leaks are possible
ERROR SUMMARY: 0 errors from 0 contexts
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
| Header Corruption       | `cfg_set_loss_rate()`                | Packet unlock                              | Receive      |
| Drop part of packet     | `cfg_set_loss_rate()`                | Incomplete frame                           | Receive      |


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
cmake --build ./test/io/communication/
./test/io/communication/test_comm_udp
```

**`communication.c`**:
```bash
gcov build/src/io/communication/CMakeFiles/mcore_comm.dir/mc_communication.c.
File '/media/mahdi/common/repositories/micro_core/src/io/communication/mc_communication.c'
Lines executed:100.00% of 77
Creating 'mc_communication.c.gcov'

Lines executed:100.00% of 77
```

**`mc_protocol.c`**:
```bash
gcov build/src/io/communication/CMakeFiles/mcore_comm.dir/mc_protocol.c.
File '/media/mahdi/common/repositories/micro_core/src/io/communication/mc_protocol.c'
Lines executed:100.00% of 51
Creating 'mc_protocol.c.gcov'

Lines executed:100.00% of 51
```

**`mc_io.c`**:
```bash
gcov build/src/io/communication/CMakeFiles/mcore_comm.dir/mc_io.c.
File '/media/mahdi/common/repositories/micro_core/src/io/communication/mc_io.c'
Lines executed:100.00% of 19
Creating 'mc_io.c.gcov'

Lines executed:100.00% of 19
```

**`mc_frame.c`**:
```bash
gcov build/src/io/communication/CMakeFiles/mcore_comm.dir/mc_frame.c.
File '/media/mahdi/common/repositories/micro_core/src/io/communication/mc_frame.c'
Lines executed:100.00% of 32
Creating 'mc_frame.c.gcov'

Lines executed:100.00% of 32
```

**`window_pool.c`**:
```bash
gcov build/src/io/communication/CMakeFiles/mcore_comm.dir/mc_window_pool.c.
File '/media/mahdi/common/repositories/micro_core/src/io/communication/mc_window_pool.c'
Lines executed:98.37% of 123
Creating 'mc_window_pool.c.gcov'

Lines executed:98.37% of 123
```

Totaly: **99.674%** code coverage achieved.
<br>
<br>

## Performance benchmarks



<br>
<br>

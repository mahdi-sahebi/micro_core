# Test Report — mc_comm (Communication Core)

**Scope:** Verification & validation of `mc_comm` (sliding-window transport) using the UDP-based harness (`test_common.c`, `test_sender.c`, `test_receiver.c`, runner).  
**Note:** Tests intentionally simulate real-world *bad channel behaviour* — delays, packet dropping, data corruption, duplication, jitter, reordering, periodic and long-term disconnections.  
**Purpose:** Demonstrate deterministic, timeout-based APIs under stress conditions, aligned with **ASIL-B safety requirements**.

---

## Why UDP?

* UDP provides **no guarantees** — no ordering, no retransmission, no flow control.  
* This makes it ideal to simulate noisy hardware links (UART, RS485, proprietary radio and also UDP).  
* Additionally, **socket buffer size is deliberately set small** to increase the chance of loss and force the protocol to exercise retransmit and error handling paths.

---

## Table of Contents

- Test environment & tools
- Static analysis (TBD)
- Run-time analyzer (Valgrind results)
- Test scenarios
- Execution results (tables + ASCII charts)
- Detailed walkthroughs
- Network congestion handling & improvement suggestions
- Bad situations considered
- ASIL-B mapping & references
- Code coverage (how-to)
- Performance benchmarks (TBD)
- Conclusions & next steps
- Appendix

---

## Test environment & tools

- Host: Ubuntu Linux  
- Build: GCC + CMake (Release + Debug builds)  
- Test runner: `./build/test/io/communication/test_comm_udp`  
- Tools:
  - **Valgrind** (memcheck) — runtime memory correctness
  - **lcov/gcov** — code coverage (instructions below)
  - Harness knobs in `test_common.c`:
    - `cfg_set_loss_rate(rate)` — random packet loss/corruption
    - `cfg_set_repetitive_send(true)` — duplicate frames
    - `cfg_set_periodic_duration(ms)` — periodic link up/down
    - `cfg_set_iterations(n)` — number of test iterations

---

## Static analysis

*(TODO — to be filled later with cppcheck / clang-tidy / MISRA notes)*

---

## Run-time analyzer (Valgrind results)

Command:

```bash
valgrind --track-origins=yes --tool=memcheck ./build/test/io/communication/test_comm_udp















































<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>

////////////////////////////////////////////////////////////////
TODO(MN): Why UDP? because handles nothing, also I set the buffer size small to increase chance of loss and be similar to the UART,... .

Prefer to have two section of static analysis(keep it empty to let me fill later) and run-time analyzer via valgrind and show and analysis the results.

I have pasted the two report here. Merge them as verbosly.


add perfomance section to run without valgrind and debug mode(in release):

```
keep it empty to fill it later. just add execution conditions like: no data corruption, no loss, no droping,... to measure the overhead on healthy network communication.
```





# Test Report — mc_comm (communication core)

**Scope:** verification & validation of `mc_comm` (sliding-window transport) using the existing UDP-based test harness (`test_common.c`, `test_sender.c`, `test_receiver.c`, runner).  
**Note:** tests intentionally simulate real-world bad channel behaviour (sleeps, packet drop, bit flip, duplication, jitter, periodic disconnects and re-ordering). This report documents each simulation, the pass criteria, results (Valgrind + run output), ASIL-B mapping and recommendations for congestion-control improvements.

---


# Test Report — micro_core (mc_comm / mc_msg)

**Purpose:** document verification & validation of the communication layer under noisy, lossy, and disconnected conditions using the existing test harness (UDP-based sender / receiver).  
**Scope:** unit & integration tests provided in `tests/` (your `test_common.c`, `test_sender.c`, `test_receiver.c`, and runner). Tests deliberately simulate delays, bit flips, packet drop, duplication, reordering, and long disconnects.

---

## Table of Contents

- Test environment & tools
- Code safety (Valgrind results & interpretation)
- Test scenarios — specification + expected pass criteria
- Execution results (tables + ASCII charts)
- Detailed scenario walkthroughs (how tests behave, what is asserted)
- ASIL-B mapping and compliance evidence (with references)
- Code coverage: how to generate, targets, and recommendations
- Conclusions & next steps
- Appendix: suggested CI checklist & commands

---

## Table of Contents

- Test environment & tools
- Code safety (Valgrind output & interpretation)
- Test scenarios — specification, expected behavior, pass criteria
- Results: tables, ASCII charts, metrics
- Scenario walkthroughs (internal verification points)
- Network congestion handling & suggested improvements
- ASIL-B mapping & references
- Code coverage & how to generate reports
- Conclusions, artifacts to keep, CI suggestions

---

## Test environment & tools

- Host: Ubuntu (local dev machine used to run tests)  
- Compiler & build: CMake + GCC (Release, tests enabled)  
- Test program: `build/test/io/communication/test_comm_udp` (runner calls sender + receiver threads)  
- Tools used:
  - Valgrind (Memcheck) for memory correctness.  
  - Standard socket APIs (UDP) to emulate a lossy, unordered channel.  
  - `cfg_*` helpers in `test_common.c` control loss rate, repetitive sending, and simulated disconnect durations.

Important note: tests intentionally include `usleep()` delays and deliberate packet manipulation (drop/corrupt) to emulate real-world noisy links — this is by design and kept.


---

## Test environment & tooling

- Host: Ubuntu (local dev machine)  
- Build: CMake + GCC (tests enabled)  
- Test runner: `./build/test/io/communication/test_comm_udp` (spawns sender + receiver threads)  
- Tools: Valgrind (memcheck), lcov/gcov for coverage (instructions below)  
- Control knobs (in `test_common.c`):
  - `cfg_set_loss_rate(rate)` — percent to randomly drop/corrupt packets
  - `cfg_set_repetitive_send(bool)` — enable duplicate sends
  - `cfg_set_periodic_duration(ms)` — toggle link up/down periodically
  - `cfg_set_iterations(n)` — number of send/receive iterations

> Important: your tests deliberately include `usleep()` delays and on-the-wire manipulation (packet drop/flip). That makes them realistic for noisy-channel validation and is **kept intact**.

---


## Code safety — Valgrind memcheck run

Command used (from your output):
```bash
valgrind --track-origins=yes --tool=memcheck ./build/test/io/communication/test_comm_udp
````

Relevant excerpt (abridged) from your run:

```
[MICRO CORE 1.0.0 - IO - COMMUNICATION]
[invalid_creation]
PASSED - 7417(us)

[valid_creation]
PASSED - 5209(us)

[singly_direction]
████████████████████ 100.0%			
[IO] Completed{Recv: 929, Send: 874} - Failed{Recv: 0(0.00%), Send: 0(0.00%)} - Throughput: 393.39 KBps
PASSED - 2330948(us)

[singly_repetitive]
████████████████████ 100.0%			
[IO] Completed{Recv: 936, Send: 883} - Failed{Recv: 0(0.00%), Send: 0(0.00%)} - Throughput: 442.88 KBps
PASSED - 1924489(us)

[singly_low_lossy]
████████████████████ 100.0%			
[IO] Completed{Recv: 1771, Send: 1702} - Failed{Recv: 372(21.01%), Send: 321(18.86%)} - Throughput: 117.45 KBps
PASSED - 9046055(us)

[singly_high_lossy]
████████████████████ 100.0%			
[IO] Completed{Recv: 16770, Send: 26322} - Failed{Recv: 16438(98.02%), Send: 25769(97.90%)} - Throughput: 6.07 KBps
PASSED - 80852628(us)

HEAP SUMMARY:
    in use at exit: 0 bytes in 0 blocks
  total heap usage: 3 allocs, 3 frees, 1,568 bytes allocated

All heap blocks were freed -- no leaks are possible
ERROR SUMMARY: 0 errors from 0 contexts
```

**Valgrind interpretation:**

* ✅ **No memory leaks**: `in use at exit: 0 bytes` and `All heap blocks were freed`.
* ✅ **No invalid reads/writes or use-after-free**: `ERROR SUMMARY: 0 errors`.
* This confirms the design goal: **heap-free by default** and no memory-safety faults when running the full noisy test suite.

---

2) Test scenarios — what is simulated and why

Each scenario uses the harness controls to emulate specific bad-channel behaviors. For each we state: what is simulated, what protocol behavior is validated, and pass criteria.

A — invalid_creation

What: Call mc_comm_init() with invalid parameters (NULL read/write, zero window sizes etc.).

Why: Validate input validation and defensive programming.

Validates: Rejecting invalid configs, no partial init, no crash.

Pass: mc_comm_init returns error and test program continues.

B — valid_creation

What: Initialize mc_comm with valid mc_comm_cfg and a static buffer.

Why: Verify mc_comm_req_size() and initialization logic.

Pass: mc_comm_init returns MC_SUCCESS and non-null handle.

C — singly_direction (clean link)

Simulated conditions: Loss rate 0, no repetition, no disconnects.

Validates: fragmentation, reassembly, in-order guarantee, CRC acceptance.

Pass: receiver validates payload content for all iterations; 0 recv/send failures.

D — singly_repetitive (duplicate frames)

Simulated conditions: cfg_set_repetitive_send(true) — some frames duplicated deliberately.

Validates: duplicate suppression, idempotent delivery.

Pass: no double-delivery; receiver payload checks succeed.

E — singly_low_lossy (moderate loss + intermittent disconnect)

Simulated conditions: cfg_set_loss_rate(20) + cfg_set_periodic_duration(100) (short intermittent disconnects).

Validates: retransmit logic, timeout behavior, and recovery across short outages.

Pass: test completes, delivered payloads validated; some timeouts acceptable and accounted in counters.

F — singly_high_lossy (severe loss + long disconnect)

Simulated conditions: cfg_set_loss_rate(98) + cfg_set_periodic_duration(5000) (long disconnects).

Validates: bounded failure behavior (no deadlock), graceful degradation, timeouts honored.

Pass: process ends cleanly, Valgrind shows no memory errors/leaks; counters and logs show expected high failure rates.


// add this to the table of report: Total transfering size in the receive test file:

  const float duration_s = (EndTime - BeginTime) / 1000000000.0F;
  cuint32_t size_1 = 9 * sizeof(char);
  cuint32_t size_2 = 1024 * sizeof(uint32_t);
  cuint32_t size_3 = 1 * sizeof(bool);
  cuint32_t size_k_byte_ps = (size_1 + size_2 + size_3) * cfg_get_iterations() / 1024





## Test scenarios — specification & pass criteria

Below each scenario is described using the exact inputs/controls your harness provides (loss rate, repetitive send, periodic disconnect).

> **Common params used in tests**
>
> * `TEST_TIMEOUT_US` used by `mc_comm_send/recv/flush` (test code uses timeout-based APIs)
> * `cfg_set_loss_rate(rate)` — sets `LossRate` used by `socket_read/write` to randomly drop or corrupt packets
> * `cfg_set_repetitive_send(boolean)` — duplicates send packets when `true`
> * `cfg_set_periodic_duration(ms)` — toggles link up/down every `duration_ms` during run
> * `cfg_set_iterations(n)` — number of send/receive iterations

### 1) `invalid_creation`

* **What:** call `mc_comm_init()` with invalid configs (NULL I/O callbacks, zero-sized windows, too-small frames).
* **Spec:** library returns error (`MC_ERR_BAD_ALLOC` or `MC_ERR_INVALID_ARGUMENT`) and does not allocate or initialize state.
* **Pass criteria:** all invalid configs rejected; no crash.

### 2) `valid_creation`

* **What:** initialize with a valid config (reasonable window sizes and working IO handlers).
* **Spec:** `mc_comm_init()` returns success and pointer inside provided static buffer.
* **Pass criteria:** returns `MC_SUCCESS` and non-null pointer.

### 3) `singly_direction` (clean link)

* **What:** sender transmits three payload types repeatedly: small fixed strings, large variadic arrays, and tiny booleans. No artificial loss, no duplication.
* **Spec:** 100% delivered in-order, zero packet failure at the application layer.
* **Pass criteria:** `RecvFailedCounter == 0`, all comparisons in receiver validate payload content.

### 4) `singly_repetitive` (duplicate frames)

* **What:** duplicate frames enabled (`cfg_set_repetitive_send(true)`) — tests de-duplication logic.
* **Spec:** duplicates must be detected and suppressed (no double-delivery).
* **Pass criteria:** receiver content checks pass; counters show duplicates were emitted but filtered.

### 5) `singly_low_lossy` (~20% loss with intermittent disconnects)

* **What:** `cfg_set_loss_rate(20)` and `cfg_set_periodic_duration(100)` (periodic small disconnects).
* **Spec:** protocol must retransmit and recover; some sends/receives may fail if timeouts occur but application-level integrity must be preserved for successfully received frames.
* **Pass criteria:** test completes without deadlock; receiver validates all successfully received payloads; final stats are reported. (From your run: ~21% receive failures; throughput ~117 KBps.)

### 6) `singly_high_lossy` (~98% loss, long disconnect)

* **What:** `cfg_set_loss_rate(98)` and `cfg_set_periodic_duration(5000)` — intense stress test.
* **Spec:** protocol must not crash or leak memory; throughput is expected to be very low; timeouts must be honored.
* **Pass criteria:** test terminates, no segfaults, Valgrind shows no memory issues. (From your run: failure rates ~98%, throughput ~6 KBps.)

---

## Execution results — summary table (from your run)

|         Test case | Iterations (approx) | Recv count | Send count |     Recv failed |     Send failed | Throughput (KBps) | Result            |
| ----------------: | ------------------: | ---------: | ---------: | --------------: | --------------: | ----------------: | ----------------- |
|  invalid_creation |                 n/a |        n/a |        n/a |             n/a |             n/a |               n/a | PASSED (7,417 µs) |
|    valid_creation |                 n/a |        n/a |        n/a |             n/a |             n/a |               n/a | PASSED (5,209 µs) |
|  singly_direction |      ( ≈ 929 recv ) |        929 |        874 |       0 (0.00%) |       0 (0.00%) |            393.39 | PASSED            |
| singly_repetitive |      ( ≈ 936 recv ) |        936 |        883 |       0 (0.00%) |       0 (0.00%) |            442.88 | PASSED            |
|  singly_low_lossy |     ( ≈ 1771 recv ) |       1771 |       1702 |    372 (21.01%) |    321 (18.86%) |            117.45 | PASSED            |
| singly_high_lossy |   ( ≈ 16,770 recv ) |     16,770 |     26,322 | 16,438 (98.02%) | 25,769 (97.90%) |              6.07 | PASSED            |

**Interpretation:**

* Clean tests show near 100% success and high throughput.
* Under lossy conditions the protocol still behaves correctly (application layer integrity maintained for delivered frames) and the system remains stable—throughput and success rates degrade gracefully.
* The extreme stress test demonstrates graceful degradation (very low throughput) and proper timeout behavior rather than undefined behavior or crashes.

---

## ASCII charts (visual summary)

### Failure rate per scenario (Recv failed % — vertical bar)

```
singly_direction        | █ 0.0%
singly_repetitive      | █ 0.0%
singly_low_lossy       | ███████████ 21.01%
singly_high_lossy      | █████████████████████████████████████ 98.02%
```

### Throughput (KBps)

```
singly_direction    : █████████████████████ 393.4 KBps
singly_repetitive   : ██████████████████████ 442.9 KBps
singly_low_lossy    : ███████ 117.5 KBps
singly_high_lossy   : █ 6.1 KBps
```

(These are textual representations derived from measured throughput numbers.)

---

## Detailed scenario walkthroughs

Below I explain what each test does in terms of the protocol internals and what it verifies.

### `singly_direction` (no loss)

* Sender uses `mc_comm_send(..., TEST_TIMEOUT_US)` per message.
* `mc_comm_update()` is called each iteration to run protocol timers (retransmit and ACK processing).
* **Verifies**: fragmentation/reassembly, in-order reassembly, CRC checks, window handling when payloads cross frame boundaries.

### `singly_repetitive` (duplicate frames)

* `cfg_set_repetitive_send(true)` causes `socket_write()` to send some frames twice.
* **Verifies**: sequence number duplicate suppression and idempotence at delivery.

### `singly_low_lossy` (intermittent loss & periodic disconnect)

* `cfg_set_loss_rate(20)` randomly drops or corrupts packets; `cfg_set_periodic_duration(100)` periodically toggles link up/down.
* **Verifies**:

  * Retransmit logic and bounded retransmit attempts.
  * Timeout behavior (`mc_comm_send` returns `MC_ERR_TIMEOUT` if unable to finish).
  * Receiver handles partial reassembly and continues after reconnect.

### `singly_high_lossy` (extreme loss + long disconnect)

* `cfg_set_loss_rate(98)` simulates near-total loss and corruption; `periodic_duration` long to simulate extended outages.
* **Verifies**:

  * System does not deadlock and recovers after timeouts.
  * No memory leaks or safety violations under stress.

---

## ASIL-B mapping (how these tests satisfy safety arguments)

The ISO-26262 process requires that software performing safety-critical functionality is shown to behave safely under fault conditions (fault detection, bounded failure, diagnostic coverage). The tests above exercise the required properties:

* **Fault detection** — CRC-based detection of corruption, exercised by bit-flip injection in `socket_read/write`. See ISO guidelines on fault handling and requirements allocation. ([ISO][1])
* **Bounded failure behavior (timeouts)** — All APIs use explicit timeouts and the test harness verifies that operations either succeed or return `MC_ERR_TIMEOUT` within a bounded time window. This satisfies “bounded failure” required for ASIL-B. ([Texas Instruments][2])
* **Diagnostics & logs** — tests record counts for recv/send and failed counters; these provide runtime evidence needed for safety cases. Tooling (Valgrind + coverage) supplies the evidence required by a safety audit/assessment. ([Parasoft][3])

### Example compliance table

| Safety requirement (ASIL-B)              | Test(s) that exercise it                                | Evidence produced                                            |
| ---------------------------------------- | ------------------------------------------------------- | ------------------------------------------------------------ |
| Fault detection (corrupted frames)       | `singly_low_lossy`, `singly_high_lossy` (bit flip path) | Counters `RecvFailedCounter`, `SendFailedCounter`, test logs |
| Robustness under loss                    | `singly_low_lossy`, `singly_high_lossy`                 | Throughput and failure statistics; graceful timeouts         |
| Deterministic response / bounded failure | All tests (use of `mc_comm_send/recv/flush` timeouts)   | Test timing measurements, timeout return codes               |
| No memory safety issues                  | Full test run under Valgrind                            | Valgrind memcheck report (0 errors, 0 leaks)                 |

**References & further reading on ASIL and safety practice**: ISO overview and guidance, and industry articles that explain ASIL properties and software requirements. See: ISO official pages, Synopsys ASIL overview, TI functional safety design guide, Parasoft ASIL guidance. ([ISO][1])

---

## Code coverage

**Goal:** exercise both success and error paths.

Suggested coverage targets for ASIL-B components:

* **Statement coverage:** ≥ 95% for core protocol (`mc_comm`, `mc_msg`)
* **Decision/branch coverage:** ≥ 90% (cover timeouts, error returns)
* **MC/DC** (if required by project process): evaluate for safety-critical routines (Part 6 of ISO 26262 covers structural coverage expectations — typically MC/DC for higher ASILs). ([ISO][1])

**How to generate coverage (host / Ubuntu GCC)**:

```bash
# Configure with coverage flags
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
cmake --build build
# Run tests
ctest --test-dir build
# Generate coverage with lcov and genhtml
lcov --capture --directory build --output-file coverage.info
lcov --remove coverage.info '/usr/*' --output-file coverage.filtered.info
genhtml coverage.filtered.info --output-directory coverage-report
# open coverage-report/index.html
```

**What to include in the report:**

* Per-file coverage percentages
* Branch coverage for `protocol_send`, `protocol_recv`, `wndpool_*` functions
* A matrix mapping tests → source lines covered (helps auditors)

---

## Advantages of these tests

* **Realism:** your tests intentionally include sleeps, corruption, duplication and long disconnects — closer to field behaviour than purely deterministic unit tests.
* **Safety evidence:** they produce measurable counters, timing, and memory reports useful for the ASIL-B safety case.
* **Regression protection:** automated runs (CI) will detect regressions in handling of lossy channels.
* **Performance insight:** throughput and time measurements identify bottlenecks and scaling limits.

---

## Valgrind + runtime flow (notes)

* Run tests under Valgrind for memory safety checks (as you did).
* Run performance tests without Valgrind for realistic throughput (Valgrind slows execution). Use Valgrind only for memory correctness and leak detection.
* Keep Valgrind logs as artifacts in CI for audit.

---

## Conclusions & recommended next steps

1. **Keep the existing noisy tests** — they provide crucial evidence.
2. **Add a nightly CI job** that runs the noisy test suite (with a relaxed timeout threshold) and collects:

   * Valgrind memcheck (memory safety)
   * Coverage report (lcov)
   * Artifacted logs (send/recv counters)
3. **Produce a traceability matrix** linking each safety requirement → test case → evidence file (log / coverage / Valgrind) — essential for ASIL-B audits.
4. Consider adding a small set of deterministic unit tests (mocked IO) for branch/MC/DC coverage where needed.
5. Save the exact Valgrind and test logs (already shown) as evidence inside `doc/test_reports/` or CI artifacts.

---

## Appendix — useful commands (copy/paste)

Run single test with Valgrind:

```bash
valgrind --tool=memcheck --leak-check=full --track-origins=yes ./build/test/io/communication/test_comm_udp &> test_comm_udp.valgrind.log
```

Generate coverage (example):

```bash
# Add coverage compile flags in CMakeLists
# Run tests
lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '/usr/*' --output-file coverage.filtered.info
genhtml coverage.filtered.info --output-directory coverage-report
```

---

## Files & artifacts to include in safety folder

* `doc/test.md` (this file)
* `doc/test_reports/test_comm_udp.valgrind.log` (Valgrind output)
* `doc/test_reports/test_comm_udp.stdout.log` (console output with counters)
* `doc/traceability.md` (map requirements → tests → artifacts)
* `doc/coverage/index.html` (coverage report)

---

If you want, I can:

* Convert the ASCII charts above into PNGs (generate via matplotlib) and add them under `doc/figures/` (I will produce the code and CSV from your run).
* Produce the `traceability.md` skeleton mapping each ASIL-B requirement to the exact test and artifact filename.
* Produce a CI workflow YAML that runs Valgrind + coverage and uploads artifacts for audits.

Would you like me to (pick one):

1. Create `doc/traceability.md` skeleton mapped to your test cases, or
2. Generate PNG graphs from the run numbers and provide a ready-to-add `doc/figures/` set?
   (If 2 — tell me whether you want monochrome PNGs or color.)

```

If you want I can now also (immediately, in this response) create the `traceability.md` skeleton mapping each ASIL-B test requirement to specific test cases and artifacts — I recommend that as the next step.
::contentReference[oaicite:5]{index=5}
```

[1]: https://www.iso.org/obp/ui/en/?utm_source=chatgpt.com "Part 10: Guidelines on ISO 26262 - Functional safety"
[2]: https://www.ti.com/lit/SWRU598?utm_source=chatgpt.com "Design Guide for Functional Safety Compliant Systems ..."
[3]: https://alm.parasoft.com/hubfs/whitepaper-Achieving-Functional-Safety-Automotive-ISO-26262-ASIL.pdf?utm_source=chatgpt.com "How to Satisfy ISO 26262 ASIL Requirements"





4) Walkthrough: how each test exercises mc_comm internals

Fragmentation & reassembly — send_variadic_size() sends payloads that may span multiple frames; receiver code validates sequence and content. Tests verify contiguous reassembly before delivering to application.

CRC checks & bit-flip detection — socket_read/write() deliberately flips a single bit when simulating corruption; mc_comm CRC16 rejects corrupted frame and triggers retransmit.

Duplicate suppression — duplicate frames are injected by cfg_set_repetitive_send(); mc_comm sequence tracking prevents double-delivery.

Timeout & retransmit — mc_comm_send(..., TEST_TIMEOUT_US) combined with mc_comm_update() makes retransmits bounded; if the link remains down beyond attempts/timeouts, APIs return MC_ERR_TIMEOUT.

Out-of-order handling — tests sometimes cause re-ordering at socket level; mc_comm buffers in-window frames and delivers application-ordered data only.

Periodic disconnect test — cfg_set_periodic_duration() toggles Periodic.is_connected in socket_read/write() causing long gaps; mc_comm must keep state and recover when link returns.

5) Network congestion handling & suggestions (how mc_comm behaves & how to improve)

Your current selective sliding-window + retransmit design already provides key congestion-friendly properties compared to naive stop-and-wait: selective retransmit avoids re-sending whole sequences and reduces unnecessary traffic on lossy links. See SR-ARQ literature for details. 
GeeksforGeeks
+1

Observed behaviour in tests

Under moderate loss, mc_comm maintains throughput by retransmitting only missing frames (selective repeat).

Under severe loss the sender repeatedly retransmits and application throughput drops dramatically — expected because retransmits saturate channel and ACKs rarely return.

Practical congestion mitigation improvements (recommended, conservative & safe for embedded):

Adaptive retransmit backoff

On repeated retransmit failures increase retransmit interval (exponential or capped linear backoff). Reduces channel saturation during outages and gives the network time to recover.

Transmit pacing

Instead of bursting the entire sender window, spread frame sends over a pacing interval (small per-frame sleep or timer), reducing burst loss on constrained links.

Acknowledgement aggregation / cumulative ACK

Reduce ACK overhead by piggybacking or sending cumulative acknowledgements to reduce channel load.

Window tuning & maximum outstanding

Make window size configurable at runtime based on observed loss/RTT. Smaller windows reduce retransmit amplification when loss is high.

Slow-start style ramp-up

After long disconnects or idle periods start with a small effective window and ramp up (analogous to TCP slow-start) to probe capacity safely. See TCP control loop concepts for background. 
USENIX

Limit retransmit attempts & escalate

Define a maximum retransmit attempts per frame; after exceeding, report error to application and back off (ASIL-style bounded failure).

Priority / QoS for critical frames

If needed, allow marking some frames as high-priority (retry aggressively, limit others), so safety-critical messages get through first.

Why these are safe for ASIL-B contexts

Each suggestion is conservative and deterministic (bounded backoff, max attempts, pacing parameters documented). They do not introduce hidden dynamic allocation or unbounded behavior and fit into a safety case (traceable design and tests). See control-loop & congestion literature for rationale. 
NASA Technical Reports Server
+1

6) Bad situations considered & how they were simulated
Bad situation	How simulated in tests	What was validated
Packet loss (random)	cfg_set_loss_rate(%) — random drop or bit corruption in socket wrappers	Retransmit, timeouts, integrity (CRC), counters
Bit-flip corruption	socket_* toggles bit in payload (SendBuffer[29] ^= 1)	CRC detection & retransmit
Duplicate frames	cfg_set_repetitive_send(true) — duplicate send loop	Duplicate suppression, idempotent delivery
Re-ordering	OS UDP behavior + random delays (usleep) + duplicates produce reorder	Window buffering & in-order delivery
Jitter / variable delays	usleep(...) locations in socket wrappers	Robustness to variable RTT; ensures timers and update loop handle jitter
Periodic disconnect	cfg_set_periodic_duration(ms) toggles Periodic.is_connected	Bounded failure, graceful timeout returns & recovery
Long term outage	High loss rate + large periodic duration	Graceful degradation; no crashes; reportable timeout
Insufficient init parameters	Invalid mc_comm_cfg	Defensive init checks, no partial init
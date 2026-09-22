# MISRA C:2012 Deviation Register

This register documents every suppression used by the micro_core MISRA gate.
Suppressions are scoped to the smallest practical set of files. Mandatory rules
are never deviated. Reapproval is required if a listed interface, storage model,
or control-flow assumption changes.

## Active deviations

### DEV-001 — Rule 5.5 (Required): constructor macro and type names

- Scope: result, buffer, I/O, communication, message, and chain public headers.
- Rationale: Same-named function-like macros construct typed result aggregates.
- Safety impact: Arguments are evaluated once in compound-literal initializers.
- Mitigation: Scoped header suppressions and compile/test verification.

### DEV-002 — Rule 18.7 (Required): flexible array storage

- Scope: caller-buffer containers in DSA, pattern, communication, and message.
- Rationale: Variable-capacity objects place storage after a fixed header without
  dynamic allocation.
- Safety impact: Access remains bounded by validated capacity and required-size calculations.
- Mitigation: Initialization validates storage size; boundary tests cover access.

### DEV-003 — Rule 5.6 (Required): local dispatch callback typedef

- Scope: `src/dsa/memory/memlin.c`.
- Rationale: Word-width dispatch tables use equivalent function signatures.
- Safety impact: Types are local and select only width-specific accessors.
- Mitigation: Fixed-size tables indexed by a validated enumeration.

### DEV-004 — Rule 11.3 (Required): raw storage to typed header

- Scope: DSA, pattern, communication, and message buffer initialization.
- Rationale: The no-heap API constructs private headers inside `mc_buffer`.
- Safety impact: Size is checked before conversion; later access is typed.
- Mitigation: Required-size APIs, initialization checks, scoped suppressions.

### DEV-005 — Rule 11.8 (Required): controlled const removal

- Scope: communication send adapter and window-pool storage accessor.
- Rationale: Legacy `mc_buffer` exposes writable storage where send only reads;
  the pool accessor serves read/write contexts over owned storage.
- Safety impact: Caller send data is not modified; pool storage is object-owned.
- Mitigation: Two line-scoped inline suppressions only.

### DEV-006 — Rule 11.9 (Required): analyzer handling of `NULL`

- Scope: buffer header, memlin, and message implementation.
- Rationale: cppcheck can misclassify standard `NULL` in these contexts.
- Safety impact: Source uses `NULL`, never literal zero, for null pointers.
- Mitigation: Suppression limited to known false-positive contexts.

### DEV-007 — Rule 21.1 (Required): POSIX feature-test macro

- Scope: `src/io/communication/mc_communication.c`.
- Rationale: `_DEFAULT_SOURCE` exposes required POSIX declarations under C99.
- Safety impact: It changes declaration visibility only.
- Mitigation: Guarded definition before system headers and scoped suppression.

### DEV-008 — Rule 8.7 (Advisory): public APIs reported as TU-local

- Scope: core time, DSA, chain, algorithm, and message public definitions.
- Rationale: Production-only analysis excludes library consumers and tests, so
  cppcheck cannot observe their external references.
- Safety impact: Internal linkage would break the documented library API.
- Mitigation: Suppressions are limited to files containing declared public APIs;
  headers and link-time tests validate external linkage.

### DEV-009 — Rule 11.5 (Advisory): generic storage conversion

- Scope: memlin, communication, and message generic-storage boundaries.
- Rationale: Generic callbacks and caller-owned byte storage cross boundaries as
  `void *` and are converted to validated private types.
- Safety impact: Conversion follows size or protocol validation.
- Mitigation: Scoped suppressions, required-size checks, functional tests.

### DEV-010 — Rule 15.5 (Advisory): guard-clause returns

- Scope: DSA, chain, communication, and message implementations.
- Rationale: Early returns make validation, timeout, capacity, and packet
  rejection explicit. Affected functions acquire no resources needing cleanup.
- Safety impact: No cleanup is bypassed; results preserve precise failure causes.
- Mitigation: File-scoped suppressions only for resource-free modules and tests.

## Review status

These deviations are technically justified and active in the automated gate.
Formal safety-manager approval and tool qualification remain release activities;
this register does not by itself constitute ISO 26262 certification.

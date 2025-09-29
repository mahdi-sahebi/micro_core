# MICRO CORE

micro_core is a lightweight and safety-oriented communication and processing library written in pure C99.
It is tailored for low-resource embedded systems that require portability, high reliability, and compliance with safety standards such as MISRA C:2012 and ISO 26262 ASIL-B.

Unlike many embedded libraries, micro_core has zero dependencies and does not rely on dynamic memory allocation, making it suitable for environments where determinism, predictability, and efficiency are critical.
This means the same codebase can be deployed on resource-constrained microcontrollers, bare-metal systems, or standard host platforms without modification.


## Features

- **Safety-Compliant:** Developed with MISRA C:2012 coding rules and designed with ISO 26262 ASIL-B safety requirements in mind, making it a strong fit for automotive, medical, and industrial applications.
- **Portability:** Written in C99 with no third-party dependencies, ensuring it can run across a broad range of platforms — from small microcontrollers to embedded systems.
- **Heap-Free:** All memory is statically allocated, eliminating fragmentation risks and unpredictable runtime behavior caused by dynamic allocation. All APIs work on user-given buffer for taking better control.
- **Deterministic:** Designed for real-time and safety-critical systems, guaranteeing consistent behavior even under constrained conditions.
- **Low Memory Footprint:** Optimized for minimal code size and RAM usage.
- **Modular:** The library is structured into independent modules that can be combined as needed. Each module has clear responsibilities and documentation, allowing users to only integrate what they need.
- **Unit Testing:** Provides unit tests, compliance tests, and benchmarks to ensure correctness, reliability, and maintainability. Tests are platform-independent and can be executed both on embedded hardware and host PCs.
- **CMake Build System:** Modern, portable, and configurable build system with options for enabling/disabling tests, benchmarks, and compliance checks.


Developers of embedded systems often struggle with:

Libraries that are too heavyweight for constrained devices
Hidden dynamic allocations that lead to unpredictable crashes
Lack of MISRA or ISO 26262 compliance, which is required in safety-critical industries
Difficulty in running portable tests across microcontrollers and host platforms

micro_core addresses these problems directly by focusing on clarity, safety, and strict resource control.
This makes it an excellent foundation for communication stacks, message-based systems, and other embedded infrastructure in domains where quality matters more than speed of hacking.


## Changelog
1.0.0 (Initial Release)
- **mc_comm:** Raw data communication over selective sliding window flow
- **mc_msg:** Message ID-based protocol over mc_comm


## Structure
- [Documentation](./doc/doc.md)
- [Build](./doc/build)
- [IO](./doc/io/io.md)
  - [Communication](./doc/io/communication/communication.md)
    - [API](./doc/io/communication/api.md)
    - [Test](./doc/io/communication/test.md)
  - [Message](./doc/io/message/message.md)
    - [API](./doc/io/message/api.md)
    - [Test](./doc/io/message/test.md)


## Contributors
- [Mahdi Nejad Sahebi](mailto:m.nejadsahebi@live.co.uk): Maintainer and designer  
- **Ehsan Moradi:** Mentor of the initial version — thanks for teaching low-level concepts  


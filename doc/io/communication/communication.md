# mc_comm (Communication)

`mc_comm` is the communication backbone of **micro_core**.  
It implements a **selective sliding window protocol** that guarantees reliable data transfer even over **highly lossy and noisy channels** such as RF links, serial lines, or unstable UDP connections.  

The design prioritizes **safety, predictability, and low resource usage**, making it suitable for embedded systems where reliability is critical and hardware resources are limited.



## Advantages

- **Reliability in High Noisy-Lossy Environments:** Tolerates packet loss, corruption, duplication, and reordering. 
  Guarantees that higher-level software always receives data **in correct order** and **with integrity verified**.  

- **Safety-Oriented:** Developed with strict coding standards and **static-only allocation**.  
  No hidden dynamic memory use, ensuring deterministic and predictable execution.

- **Timeout-Based APIs:** Designed with **non-blocking** and **timeout-driven** functions, allowing applications to maintain determinism in **real-time environments**.  
  Prevents indefinite blocking on slow or unstable links, making it safer for reactive embedded systems.  

- **Resource-Constrained Friendly:** Optimized for **low memory footprint** and **small code size**, deployable on 8-bit and 16-bit MCUs while still scaling to 32-bit systems and Linux hosts.  

- **General I/O Interface:** Independent of underlying transport: can wrap **serial, RF, UDP, SPI, UART, CAN-tunneled data**, or any raw byte stream.  
  This allows a single communication layer across multiple platforms without rewriting protocols.  



## Requirements and Implementation

- **Selective Sliding Window Protocol** with controlled retransmission and acknowledgment  

- **In-Order Packet Delivery**, even if lower layers reorder frames  

- **CRC16 Error Detection** to discard corrupted packets (bit flips, partial errors)

- **Variable Frame Sizes:** Supports different maximum frame sizes for sending and receiving

- **User-Provided Buffers:** Reports required buffer size so the application can allocate and supply its own memory

- **Duplicate Frame Handling**, filtering out retransmission duplicates automatically  

- **Configurable Window Size** to balance throughput vs. memory footprint  

- **Timeout-Based Non-Blocking APIs** for deterministic response times and real-time operation  

- **Deterministic Timing**, no heap use or runtime memory growth — essential for real-time embedded systems  



## Applications

- **Medical Devices**  
  Ensures safe delivery of control and monitoring data where corruption or loss cannot be tolerated  

- **Industrial Control**  
  Stable communication in heavy **electromagnetic noise** or harsh environments  

- **Robotics & Drones**  
  Efficient over **low-bandwidth RF links** where packet drops, reordering, and duplication are common 

- **Automotive Systems**  
  Reliable command and telemetry over UART or RF diagnostics links in noisy environments  




## Safety Properties

- Detects and rejects corrupted data via **CRC16 checksums**  
- Ensures **predictable execution** with static allocation only  
- Operates with **bounded CPU cycles per frame** (constant-time operations)  
- Uses **explicit acknowledgments** and **timeouts** to avoid blocking states or ambiguous communication

Together, these properties make `mc_comm` a robust foundation for **safety-critical embedded communication** in environments where data integrity, determinism, and responsiveness are essential.  



### Related Links
- [APIs](./api.md)
- [Test](./test.md)

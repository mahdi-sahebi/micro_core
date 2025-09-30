# API REFERENCE

All APIs are heap-free, timeout-aware, and guarantee controlled behavior under any input or system state — ensuring crash handling.


<br>
<br>

# Configuration Structures

### `mc_comm_wnd`

Defines a sliding window for send/receive buffering.

```c
typedef struct {
    uint16_t size;      // Size of each frame/window in bytes
    uint8_t  capacity;  // Number of windows (segments)
} mc_comm_wnd;

#define mc_comm_wnd(SIZE, CAPACITY)   // Helper macro for creation
```


<br>
<br>

---

### `mc_comm_cfg`

Defines the configuration for initializing a communication instance.

```c
typedef struct {
    mc_io       io;     // User-supplied I/O read/write handlers
    mc_comm_wnd recv;   // Receive window configuration
    mc_comm_wnd send;   // Send window configuration
} mc_comm_cfg;

#define mc_comm_cfg(IO, RECV_WND, SEND_WND)   // Helper macro for creation
```


<br>
<br>

# API Functions

### `mc_u32 mc_comm_req_size(mc_comm_cfg config)`

#### Description:
Calculates the required buffer size (in bytes) to hold a communication instance with the given configuration.

#### Returns:
- **`value`:** required memory size in bytes
- **`error`:**
  - **`MC_SUCCESS`:** Configuration valid
  - **`MC_ERR_INVALID_ARGUMENT`:** Null or invalid parameters
  - **`MC_ERR_BAD_ALLOC`:** Window size too small for minimum frame



<br>
<br>

--- 

### `mc_ptr mc_comm_init(mc_buffer alloc_buffer, mc_comm_cfg config)`

#### Description:
Initializes a communication instance inside a statically allocated buffer.

#### Returns:
- **data:** pointer to mc_comm instance
- **`error`:**
  - **`MC_SUCCESS`:** Initialization OK
  - **`MC_ERR_BAD_ALLOC`:** Provided buffer too small
  - **`MC_ERR_INVALID_ARGUMENT`:** Invalid I/O or window configuration



<br>
<br>

--- 

### `mc_err mc_comm_update(mc_comm* this)`

#### Description:** 
Core update routine.
Handles receiving, retransmitting unacknowledged packets, and flushing pending data.
Should be called regularly in the main loop, a timer, or a dedicated thread.

#### Returns:
- **`MC_SUCCESS`:** Update successful
- **`MC_ERR_INVALID_ARGUMENT`:** Null pointer



<br>
<br>

--- 

### `mc_u32 mc_comm_recv(mc_comm* this, void* dst_data, uint32_t size, uint32_t timeout_us)`
#### Description:
Receives exactly size bytes into dst_data.

### Behavior:
Waits until full size is received or timeout expires.
Data is guaranteed in-order and corruption-checked.

#### Returns:
- **`value`:** number of bytes actually received
- **`error`:**
  - **`MC_SUCCESS`:** Full data received
  - **`MC_ERR_TIMEOUT`:** Timeout expired before complete receive
  - **`MC_ERR_INVALID_ARGUMENT`:** Null arguments



<br>
<br>

--- 

### `mc_u32 mc_comm_send(mc_comm* this, cvoid* src_data, uint32_t size, uint32_t timeout_us)`

#### Description:
Sends size bytes from src_data.

#### Behavior:
Splits large payloads into frames (according to send window).
Retransmits on loss or corruption until ACK received or timeout.

#### Returns:
- **`value`:** number of bytes actually sent
- **`error`:**
  - **`MC_SUCCESS`:** Full data sent
  - **`MC_ERR_TIMEOUT`:** Timeout expired before completion
  - **`MC_ERR_INVALID_ARGUMENT`:** Null arguments



<br>
<br>

--- 

### `mc_bool mc_comm_flush(mc_comm* this, uint32_t timeout_us)`

#### Description:
Flushes all pending send and receive buffers.

#### Behavior:
Ensures no unacknowledged packets remain.
Typically called at shutdown or end of a transaction.

#### Returns:
- **`value`:** true if flushed successfully
- **`error`:**
  - **`MC_SUCCESS`:** All buffers flushed
  - **`MC_ERR_TIMEOUT`:** Timeout expired before completion
  - **`MC_ERR_INVALID_ARGUMENT`:** Null pointer


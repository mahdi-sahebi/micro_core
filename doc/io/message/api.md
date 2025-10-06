# API REFERENCE

All APIs are **timeout-aware** and guarantee controlled behavior under any input or system state — ensuring **crash handling**.


<br>
<br>

# Configuration Structures

### `mc_msg_cfg`

Defines the configuration for initializing a message communication instance.

```c
typedef uint16_t mc_msg_id;   // Message ID for each one
typedef void (*mc_msg_cb_receive)(mc_msg_id, mc_buffer);  // Callback of a complete message is received

typedef struct {
    mc_io       io;           // User-supplied I/O read/write handlers
    mc_comm_wnd recv;         // Receive window configuration
    mc_comm_wnd send;         // Send window configuration
    uint32_t    pool_size;    // Buffer pool size for storing pending messages
    uint16_t    ids_capacity; // Max number of subscribed IDs
} mc_msg_cfg;

#define mc_msg_cfg(IO, RECV_WND, SEND_WND, POOL_SIZE, IDS_CAPACITY)   // Helper macro for creation
```


<br>
<br>

# API Functions

### `mc_u32 mc_msg_req_size(mc_msg_cfg config)`

#### Description:
Calculates the required buffer size (in bytes) to hold a message instance with the given configuration.

#### Returns:
- **`value`**: required memory size in bytes
- **`error`**:
  - **`MC_SUCCESS`**: Configuration valid
  - **`MC_ERR_INVALID_ARGUMENT`**: Null or invalid parameters
  - **`MC_ERR_BAD_ALLOC`**: Pool or window too small



<br>
<br>

--- 

### `mc_ptr mc_msg_init(mc_buffer alloc_buffer, mc_msg_cfg config)`

#### Description:
Initializes a message instance inside a **statically allocated buffer**.

#### Returns:
- **`data`**: pointer to `mc_msg` instance
- **`error`**:
  - **`MC_SUCCESS`**: Initialization OK
  - **`MC_ERR_BAD_ALLOC`**: Provided buffer too small
  - **`MC_ERR_INVALID_ARGUMENT`**: Invalid I/O or configuration


<br>
<br>

--- 

### `mc_err mc_msg_update(mc_msg* this)`

#### Description: Core update routine.
  Handles delivery of received messages and internal housekeeping.
  Should be called in the main loop, a timer, or a dedicated thread.

#### Returns:
- **`MC_SUCCESS`**: Update successful
- **`MC_ERR_INVALID_ARGUMENT`**: Null pointer


<br>
<br>

--- 

### `mc_err mc_msg_subscribe(mc_msg* this, mc_msg_id id, mc_msg_cb_receive on_receive)`

#### Description: Subscribes to a specific message ID.
  Whenever a message with this ID is received, the provided callback is triggered.

#### Returns:

- **`MC_SUCCESS`**: Subscription added
- **`MC_ERR_OUT_OF_RANGE`**: Subscription table full
- **`MC_ERR_INVALID_ARGUMENT`**: Null pointer or invalid callback


<br>
<br>

--- 

### `mc_err mc_msg_unsubscribe(mc_msg* this, mc_msg_id id)`

#### Description: 
Removes an existing subscription for the given ID.

#### Returns:
- **`MC_SUCCESS`**: Subscription removed
- **`MC_ERR_OUT_OF_RANGE`**: ID not found
- **`MC_ERR_INVALID_ARGUMENT`**: Null pointer


<br>
<br>

--- 

### `mc_u32 mc_msg_send(mc_msg* this, mc_buffer buffer, mc_msg_id id, uint32_t timeout_us)`

#### Description:
Sends a message with attached data and a specific ID.
  Ensures in-order delivery and retransmission on loss/corruption.

#### Returns:
- **`value`**: number of bytes actually sent
- **`error`**:
  - **`MC_SUCCESS`**: Full message sent
  - **`MC_ERR_TIMEOUT`**: Timeout expired before completion
  - **`MC_ERR_INVALID_ARGUMENT`**: Null pointer or bad arguments


<br>
<br>

--- 

### `mc_u32 mc_msg_signal(mc_msg* this, mc_msg_id id, uint32_t timeout_us)`

#### Description: 
Sends a **signal-only message** (header + ID, no payload).
  Useful for events, triggers, or lightweight synchronization.

#### Returns:
- **`value`**: number of bytes actually sent
- **`error`**:
  - **`MC_SUCCESS`**: Signal sent
  - **`MC_ERR_TIMEOUT`**: Timeout expired before completion
  - **`MC_ERR_INVALID_ARGUMENT`**: Null pointer


<br>
<br>

--- 

### `mc_bool mc_msg_flush(mc_msg* this, uint32_t timeout_us)`

#### Description: Flushes all pending send/receive messages.
  Typically called at shutdown or end of a transaction.

#### Returns:
- **`value`**: `true` if flushed successfully
- **`error`**:
  - **`MC_SUCCESS`**: All buffers flushed
  - **`MC_ERR_TIMEOUT`**: Timeout expired before completion
  - **`MC_ERR_INVALID_ARGUMENT`**: Null pointer


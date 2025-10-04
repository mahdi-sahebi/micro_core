### mc_msg (Message Layer)

`mc_msg` builds on top of `mc_comm` to provide a **message-oriented communication layer**.  
It abstracts raw frame handling into **ID-based messages**, enabling clean separation of logic for higher-level protocols.  



## Features

- **Message ID-Oriented:** Each message carries an **ID** field so applications can easily distinguish between different message types without parsing raw data.  

- **Observer Design Pattern:** Applications register **callbacks** for specific message IDs.  
  When a message with that ID is received, the corresponding callback is automatically invoked.  

- **Protocol Simplification:** Developers implement simple handlers for each message ID instead of manually managing.  

- **Safe and Deterministic:** Inherits the safety, CRC validation, and deterministic timing of `mc_comm`, but with an easier-to-use interface for structured message passing.  



## Advantages

- **Clear Separation of Concerns:** Application code focuses on message logic, while `mc_msg` ensures safe delivery and dispatch.  

- **Scalable:** Adding new message types is as simple as registering a new callback for a unique ID.  

- **Low Overhead:** No dynamic allocation; all callbacks and buffers are statically registered or provided by the application.  



## Use Cases

- **Command/Response Protocols:** Simple mapping of IDs to commands and responses without writing a custom dispatcher.  

- **Event-Driven Architectures:**  Embedded systems can trigger actions immediately when specific message IDs arrive, without polling.  



### Related Links
- [APIs](./api.md)
- [Test](./test.md)

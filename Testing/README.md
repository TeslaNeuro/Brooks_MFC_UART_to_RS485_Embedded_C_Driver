## 🧪 Testing

This directory is dedicated to testing key features of the `Brooks_MFC_UART_to_RS485_Embedded_C_Driver`. It includes test scripts, resources, and documentation that assist in verifying and validating the driver's functionality throughout the development process.

### 📄 `test.py` – Your Starting Point

If you're new to working with Brooks Mass Flow Controllers (MFCs) over UART to RS485 communication, begin with the `test.py` script. It provides a simple, interactive environment for sending and receiving serial commands.

This script is designed to help you:
- Understand the **S-Protocol** (a HART-based protocol) used by Brooks devices
- Learn how to structure and send raw byte commands over UART
- See immediate responses from the device for rapid feedback

### 🔍 Key Features

- 🔌 Minimal setup: Connect and start testing with just a few lines
- 🧵 Low-level UART communication: Gain insight into protocol-level interactions
- 🛠️ Easy customization: Modify commands and responses for deeper experimentation
- 📈 Scalable approach: Once you're confident, extend the tool or integrate your own driver logic

### 🚀 Learning by Doing

This testing setup avoids unnecessary abstraction layers to give you a clear view of what’s happening under the hood:
- **Raw byte-based communication** for complete transparency
- **Hands-on interaction** with the protocol
- **Extendable scripts** to test new features or custom implementations

Whether you're debugging, learning the protocol, or prototyping your own tools—this folder is your go-to resource.

---

> ⚠️ **Note:** Ensure you have the required serial libraries installed (e.g., `pyserial`) and the correct permissions to access your serial ports.

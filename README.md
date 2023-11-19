# GL-Task-3

GL С/C++ BaseCamp 3rd task.

Requirements:
- Use Linux for implementation. (or virtual machine with Linux)
- Use GCC for compilation.
- Use C/C++ for implementation.
- Console application. No UI is required.
- Use Make file for build/link/clean instructions.
- Things to google: "RAW socket"

4. Implement your own TCP-like protocol on top of UDP protocol.
- Application should use RAW sockets for implementation.
- Protocol should have it's own defined header that will be used for each packet.
- Broken packets should be re-sent.
- Regular packets should be acknowledged. 
  (Since your protocol will be based on existing protocol - it means that you must not break base protocol.
  Make changes only in your protocol header + payload)
- Implement 2 applications that communicate using your new protocol
- Packet order and checksum should be checked on both sides.
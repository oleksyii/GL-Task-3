# GL-Task-3

## Hello!
To run the app, download or clone the repoand in terminal run "make". Then type "<b>sudo ./client</b>". After that open another terminal and run  "<b>sudo ./server</b>". Don't forget the <b>sudo</b> since apps use raw sockets.

## Requirements

GL С/C++ BaseCamp 3rd task.

- Use Linux for implementation. (or virtual machine with Linux)
- Use GCC for compilation.
- Use C/C++ for implementation.
- Console application. No UI is required.
- Use Make file for build/link/clean instructions.
- Things to google: "RAW socket"

1. Implement your own TCP-like protocol on top of UDP protocol.
- Application should use RAW sockets for implementation.
- Protocol should have it's own defined header that will be used for each packet.
- Broken packets should be re-sent.
- Regular packets should be acknowledged. 
  (Since your protocol will be based on existing protocol - it means that you must not break base protocol.
  Make changes only in your protocol header + payload)
- Implement 2 applications that communicate using your new protocol
- Packet order and checksum should be checked on both sides.
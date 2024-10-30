![banner](https://i.imgur.com/jN0GBtS.png)

# EasyIPC-example

The accompanying example VS solution featuring a Client and Server project.  
This repo expects you to have vcpkg installed.  

Simply build both projects and open the folder it output the binaries to.  
Open two terminals in that folder, e.g. by holding shift and right clicking -> Open in Terminal.  

In one terminal run the server with `./Server.exe` and in the other terminal run the client with `./Client.exe`  

Pressing G in the client terminal will emit a message to the server.  
Pressing SPACE in the server terminal will emit the current dummy value to all clients.  
Note: Since this example uses GetAsyncKeyState, if you open multiple clients and press G it  
would trigger for all clients hence you will see duplicate messages.
But this has more to do with GetAsyncKeyState.

Here is a small gif showing one server terminal emitting events to many clients:

![example-with-many-clients](https://i.imgur.com/SV4wRQd.gif)

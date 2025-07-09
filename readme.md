# A simple cli chatstream

## What does this mean?
It has two components, clients and a server.
You can run the server to start a chat stream,
then anyone can use the client to connect to a server
pick a username and then send messages.
The server will then send the user any missing message they dont have

## Why?
I wanted to try to implement a server using C async with epoll, 
instead of multithreading and since a chat server might handle
many clients at the same time it seemed like a good fit.

It is also a project with a reletively small scope compared to a fully
functioning chatapp, and can perhaps be a infrastructure for something
like that in the future.


## Build for linux
`sh build_clang.sh`
Then simply use `cd build && make` to recompile
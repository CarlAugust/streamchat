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


## Build for wsl ubuntu (linux)
`sh build_clang.sh`
Then simply use `cd build && make` to recompile

## Client user manual and functionallity of the app
Currently its a little bit weird as the cli is rather limiting.
After running ./client in build/src/client then you can close the 
program by first exiting with CTRL C and then writing something random
for fgets to close. Might fix later.

/msg "message", without the quotes to send a message to all users
/chgusrn "username" to change your username, will simply be client_fd by default
Maybe more more to come

If you dont use any of these the server will simply send back an error and these
instructions will be prompted.
Exercise-Networking
=============

In today’s exercise you will practice using POSIX sockets to write programs that communicate over a network. Networking is a large and important area of computer science that we cannot possible cover entirely in this course, but you will have experience with one type of networking that plays a particularly important role in the internet: TCP/IP.

While POSIX sockets are not particularly complex or difficult to learn, the API is a bit unusual and cumbersome to use. Instead of using sockets directly, you will have the option of using a collection of simple wrappers around socket operations that is included with the starter code for today’s exercise.

## Starter Code

Download the starter code from the repository. Before moving on, read through the contents of server.c and client.c as a refresher on the basics of TCP/IP programming. There are more details in comments, but you can read about some of the important details below.

## Program Structure

The starter code includes C code for two different programs: a server and a client. You’ll need to run both of these programs, starting with the server. The server starts up and begins listening for connections. The client connects to the server, and the two programs exchange short messages over sockets.

## Client

Open the file `client.c` to see the main code for the client program. This program takes two command-line parameters: the name of a machine to connect to, and a port number.

After dealing with command-line arguments, the client calls `socket_connect`. This function is a wrapper around POSIX sockets that is defined in socket.h, which you can find in your provided starter code. The function takes a string name of a server and a port number, then attempts to open a TCP connection to that server and port. You should review the code and comments with this function in `socket.h` to see how the POSIX sockets interface actually works. Today’s exercise will not require you to call any of the POSIX socket functions directly; you can use the wrappers in socket.h instead.

After connecting to the server and checking for errors, the client sets up `FILE*` streams for the new socket. These streams make it a bit easier to deal with string input and output, since they work with useful utilities like `fprintf` and `getline`. The client creates two file streams: one for input and one for output.

Each file stream is connected with a duplicate of the socket file descriptor. Each duplicate accesses the same underlying socket, but their open/close states are independent. This way, when we call `fclose` to close the file streams later we do not close the same file descriptor more than once.

Once streams have been set up, the client waits for the server to send a message and displays it. This is done using the `fgets` function.

Next, the client sends a message to the server using `fprintf`. Calling `fprintf` only places this message in the file stream’s buffer, so we have to call `fflush` to send the complete message over the network. It is possible to set up streams to not use buffers, but that can be inefficient for some cases, and controlling buffers has some platform-dependent behavior so calling `fflush` is often easiest. If you forget to call `fflush` here, the server will be waiting for a message that was never sent.

Finally, the client closes the file streams with `fclose` and closes the `socket_fd` with `close`.

## Server

The server’s messaging code is similar to the client’s, although it sends a message and then waits for an incoming message—the reverse of the client’s messaging behavior. However, the server’s setup code is necessarily different.

First, the server creates a server socket using the `server_socket_open` function, defined in `socket.h`. You should review the documentation for this function in `socket.h` before moving on.

Once a server socket is set up, we begin listening for connections on the socket by calling the `listen` function. The second parameter to `listen` says how many incoming connections can be queued at once; additional connections are just dropped immediately.

Once the server socket is listening, calling `server_socket_accept` will accept an incoming connection. If there are no incoming connections yet, this call blocks the server until one arrives. The returned value is a file descriptor for the socket we can use to talk to the connected client. After this point you could accept another connection with the server socket and also talk to the client, but the example server just deals with one client at a time.

After establishing a connection, the server behaves much like the client. It exchanges messages with the client (in the reverse order), closes streams and socket file descriptors, and then exits.

## Testing the Starter Code

Once you’ve reviewed the starter code, run gcc to build the server and client:
```
gcc server.c -o server
gcc client.c -o client
```
Then, start the server in one terminal window with this command:
```
$ ./server
Server listening on port 35843
```
You will almost certainly see a different port number in your case. Now that we know the port, start a client in another terminal window.
```
$ ./client localhost 35843
```
This tells the client to connect to the server named "localhost", which is always the name for the current machine. Make sure you pass in the port number that was printed out by the server during startup. You should see messages exchanged between the two programs, then they will exit.

# Exercises

Try to work through each exercise during class. If you do not finish them all, make sure you complete them before the next class. You do not need to turn in your work for any of the exercises.

## Exercise 1: Echo Server

One of the first client–server programs most CS students write is an echo server. This system simply reads input from the user in the client program (using `fgets` or `getline`) and sends it to the server. The server then sends this exact message back. The client waits for this response and displays it. This process continues until the client sends the message quit, which tells the server to close the connection.

Here is an example run of the client talking to an echo server. Messages sent by the server are displayed with Server: as a prefix:
```
$ ./client localhost 1234
Hello
Server: Hello
Testing
Server: Testing
This is a longer message
Server: This is a longer message
quit
```

## Exercise 2: Capitalization Server

Now that you have a program that sends messages from the client to the server and back, you can do something more interesting on the server end. Instead of sending back the original message, have the server send back a capitalized version of the message.

While you will need to change the server program for this exercise, the client program is unchanged. Here is a sample run of the client when talking to a capitalization server:
```
$ ./client localhost 5678
Hello
Server: HELLO
Hey, stop shouting.
Server: HEY, STOP SHOUTING.
How rude
Server: HOW RUDE
123456789
Server: 123456789
quit
```
Hint: You can convert a character to its capitalized version with the `toupper` function.

## Exercise 3: Handling Multiple Clients

Most real servers deal with multiple clients at one time, but the server programs you have written so far only talk to one client. In this exercise, you will build on your capitalization server to interact with more than one client. As with the previous exercise, you will not need to change your client code.

The first step in supporting multiple clients is to handle them in sequence; modify your capitalization server to run `accept` in a loop. The server should accept a connection and interact with the one and only one client. When that client sends a quit message, the server closes the connection to the client. Instead of exiting, the server should loop back and accept a new connection. Verify that you are able to connect to your server, interact with it, quit, and then connect with another client. You will need to press `ctrl+C` to stop your server when you are done testing.

This isn’t quite what we’re aiming for; our server handles multiple clients, but still only interacts with one client at a time. You can verify this by trying to connect to the server with a client while another client is still connected to the server. To handle multiple clients simultaneously, we need to introduce concurrency to your server program.

The easiest way to deal with multiple clients at the same time is to accept connections in a loop, but pass those connections off to new threads that interact with a single client. Your main function should still have a loop that calls `server_socket_accept` to connect with a new client.

However, instead of interacting with a client inside this loop before calling `server_socket_accept` again, you should create a thread and send it the file descriptor corresponding to the newly-connected client. This thread will receive and send messages with the client, and should exit when the client sends a quit message. The main thread will continue to accept connections until you press `ctrl+C`.

## Acknowledgement ##
This exercise is based on an original exercise created by Charlie Curtsinger of Grinnell College.

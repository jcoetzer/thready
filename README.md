# thready
Thread server test

The program should run with (at least) three threads, and there should be three threads with open TCP
sockets, one listening on port 3000, another on port 3001, and a final thread on port 3002.

Upon all threads being created and now listening on ports, each thread should open a TCP connection
with the next thread “up” from it until it gets to the last thread, which should open up a TCP connection
with the first (ie in the above example, the thread on port 3000 should open a TCP connection with the
thread on 3001, the thread on port 3001 should open a TCP connection with the thread on 3002, and the
thread on 3002 should open a TCP connection with the thread on 3000).

Upon all the TCP connections being open, the first thread should read this requirements document
(NetCoreInterviewProblem.pdf) from disk (you may place this file wherever on your machine, just
please document the required path relative to the binary’s execution for our testing) into memory, print
to stdout that it is sending the file, and then send the file over the TCP connection it has open to the
next thread. It should be noted that this file is deliberately very much larger than the Maximum
Transmission Unit for TCP. Upon the next thread receiving the file, it should print to stdout that it has
received the file. It should then wait exactly 1 second, and then do exactly as the first thread did in that
it should print that it is sending the file to stdout, and send it to the next thread. The last thread should
then do the same, except sending the file to the first thread. The threads should repeatedly pass this file
around until the program is manually terminated.

It should be noted that this program may be required to run for an arbitrary amount of time, so be sure
to free memory when it is no longer needed (ie after reading the message from the socket and passing it
on, it is probably very advisable to free the memory used to temporarily hold the message).

Please limit library usage to “standard” and avoid third party libraries, ie any part of the standard posix
library such as pthreads.h socket.h, netinet/in.h, is naturally totally fair game, but third partly libraries
(such as all the boost software libraries ie netlib for C++ ) are off limits. It is likely the program you
produce will be build-able with just a simple

gcc -o your_programs_binary your_program.c

but if this is not the case, please limit your build to gnu make, and provide a makefile and
documentation.

TCP Server

    using create(), Create TCP socket.
    using bind(), Bind the socket to server address.
    using listen(), put the server socket in a passive mode, where it waits for the client to approach the server to make a connection
    using accept(), At this point, connection is established between client and server, and they are ready to transfer data.
    Go back to Step 3.

TCP Client

    Create TCP socket.
    connect newly created client socket to server.

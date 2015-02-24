# udp-file-transfer
###Overview
This repository contains a simple UDP-based file transfer client/server program. The client program (`deliver`) transmits any given file (text, audio, image, you name it) to the server program (`server`) using UDP (user Datagram Protocol). Since UDP does not offer reliable service, error control is done using Stop-and-Wait ARQ with a 1-second timeout on the client side. The client segments the file into fragments that are at most 1000 bytes long and sends these individual fragments to the server. 
###Build Instructions
To build the client and server executables, run the following command in the main directory:
```sh
$ make
```
This will create the `deliver` executable in the main directory and the `server` executable in the `/server` directory.
###Server
To the run the server program, run the following command in the `/server` directory:
```sh
$ ./server <server_port_number>
```
The server does not produce any output, and keeps listening for incoming packets until it is killed. The file that the server creates has the same name as the file given to the client. The server can handle multiple clients simultaneously provided that none of the clients have the same working directory and that all incoming files have unique names.
###Client
To run the client program, run the following command in the main directory:
```sh
$ ./deliver <server_IP_address> <server_port_number> <client_listen_port> <file_name>
```
The client does not produce any output. Make sure that a `server` process is running on a computer with the given IP address and on the given port before starting the client.

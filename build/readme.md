## TFTP Protocol - Progroup 2019
This is a small project that implements the Simple TFTP protocol following the description of [RFC 1350](https://tools.ietf.org/html/rfc1350) 

## Details implementation:

It is built in C++ language using POO 


The project follows a basic structure to organize its files containing 2 main files "server, client" also has a directory "libs" which contains the implementation of the protocol as well as help functions. 

### 1 FILE List

 - server_main
 - client_main
 - **[libs]** Folder
	 - helpers (helpers class)
	 - header (header class)
	 - server (server class) **[protocol implementation]**
	 - client (client class) **[protocol implementation]**

### 2 Build
In order to compile and run the programs you must follow these steps, CMAKE is used to perform this process which will generate a package of debian that can be installed to then run the server or client.

 - To be located in the directory build `cd build`
 - Set up CMAKE `cmake ..`
 - build and compile packages and executables `cmake --build .`
 - generate the .deb to be installed `cpack`


### 3 execute 
once installed the debian packete the executables are in the directory "home/TFTP_PG/server" and "home/TFTP_PG/client" each one with the corresponding executables must consider that the files to be transferred must be in these directories according to the case. 

#### Run server
only execute TFTP_PG_SERVER in the folder "/home/TFTP_PG/server/"

#### Run client

execute TFTP_PG_CLIENT in the folder "/home/TFTP_PG/client/" 

- to execute correctly you must pass the commands
<host> <GET/POST> <FILENAME>
- <HOST> Describe the host.
- <GET/POST> Method to use, GET is used to get a file of server folder, POST to send file to server folder
- <filename> File's name.
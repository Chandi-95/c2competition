> **_DISCLAIMER: This tool is for educational, competition, and training purposes only. I am in no way responsible for any abuse of this tool._**

A basic C2 (command and control) server I designed for use in cybersecurity competitions. Its primary role is to serve as a way to keep track of and communicate with remote target machines.

> _Note: This tool is currently in development._

## Structure

### Server

The server holds tasks/output data, keeps track of infected machines, and serves as a front for interaction with those machines.

### Client

The client sits on a target machine, calls back to the server and handles instructions sent from the server.

Currently, the only client program present is written in C++ and designed to run on Windows machines.

### Transport

The transport protocol refers to how the server and client communicate.

Currently, the server and client make use of TCP socket programming as the method of communication, though communication is unencrypted.

## How to use

The server program is built using Python, so to start the server type:

```python
python3 server.py
```

At the moment, the server has a command line menu as the primary method of interaction, with numbers 0-9 representing choices. 

```
Welcome,
Please choose the menu you want to start:
1. Connections
0. Exit
```

Entering '1' gives the "Connections" menu:

```
Choose an option below:
3. List active connections
4. Create a connection
5. Rename a connection
6. Interact with a connection
9. Back
0. Exit
```

A common method of use is to start the server, create a connection by entering a name, deploy and start the client program on the target machine, wait for a completed connection, and then navigate to interacting with the connection, entering the name of the connection before a prompt shows up.

## Future Plans/Ideas

- Encrypted communication
- Communication through other network protocols
- Client programs for other OS's
- A dropper program for automatically deploying and starting the client on targets
- Web dashboard as a front end

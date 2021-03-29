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
	int iResult = getaddrinfo("129.21.253.188", DEFAULT_PORT, &hints, &result);

	if (iResult != 0) {
		printf("getaddrinfo failed with error %d\n", iResult);
		WSACleanup();
		return 1;
	}

	SOCKET clientSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	connect(clientSocket, result->ai_addr, (int)result->ai_addrlen);
	freeaddrinfo(result);
	
	while(true) {
		printf("Ready to recieve\n");
		char recvmessage[DEFAULT_BUFFLEN];
		ZeroMemory(recvmessage, DEFAULT_BUFFLEN);
		recv(clientSocket, recvmessage, DEFAULT_BUFFLEN, 0);

		if (strcmp(recvmessage, "quit") == 0) {
			send(clientSocket, "\xff", DEFAULT_BUFFLEN, 0);
			break;
		}
		else {
			printf("Recieved message %s\n", recvmessage);
			send(clientSocket, "Hello Server!", strlen("Hello Server!"), 0);
			send(clientSocket, "\xff", strlen("\xff"), 0);
		}
		//Sleep(5000);
	}
	

	closesocket(clientSocket);
	WSACleanup();
	return 0;

}

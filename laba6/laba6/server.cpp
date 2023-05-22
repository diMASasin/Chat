#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <vector>
#include <sstream>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

string IPToString(u_long ip)
{
    string ip_str="";
    int temp = 0;
    for (int i = 0; i < 8; i++)
	{
        if (i % 2 == 0)
        {
            temp += ip & 15;
            ip = ip >> 4;
        }
        else
        {
            stringstream ss;
            temp += (ip & 15) * 16;
            ip = ip >> 4;
            ss << temp;
            if(ip_str == "")
            	ip_str = ss.str();
            else
            	ip_str = ip_str + "." + ss.str();
            temp = 0;
        }
    }
    return ip_str;
}

int main(void)
{
	//Key constants
	const int PORT_NUM = 1;				// Enter Open working server port
	const short BUFF_SIZE = 1024;			// Maximum size of buffer for exchange info between server and client

	// Key variables for all program
	int erStat;								// Keeps socket errors status

	// WinSock initialization
	WSADATA wsData;

	erStat = WSAStartup(MAKEWORD(2, 2), &wsData);

	if (erStat != 0) 
	{
		cout << "Error WinSock version initializaion #";
		cout << WSAGetLastError();
		return 1;
	}
	else
	{
		cout << "WinSock initialization is OK" << endl;
	}

	// Server socket initialization
	SOCKET ServSock = socket(AF_INET, SOCK_STREAM, 0);

	if (ServSock == INVALID_SOCKET) 
	{
		cout << "Error initialization socket # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else
	{
		cout << "Server socket initialization is OK" << endl;
	}

	// Server socket binding
	sockaddr_in servInfo;
	ZeroMemory(&servInfo, sizeof(servInfo));	// Initializing servInfo structure

	servInfo.sin_family = AF_INET;
	servInfo.sin_addr.s_addr = INADDR_ANY;
	servInfo.sin_port = htons(PORT_NUM);

	erStat = bind(ServSock, (sockaddr*)&servInfo, sizeof(servInfo));

	if (erStat != 0) 
	{
		cout << "Error Socket binding to server info. Error # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else
	{
		cout << "Binding socket to Server info is OK" << endl;
	}

	//Starting to listen to any Clients
	erStat = listen(ServSock, SOMAXCONN);

	if (erStat != 0) 
	{
		cout << "Can't start to listen to. Error # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else 
	{
		cout << "Listening..." << endl;
	}

	//Client socket creation and acception in case of connection
	sockaddr_in clientInfo;
	ZeroMemory(&clientInfo, sizeof(clientInfo));	// Initializing clientInfo structure

	int clientInfo_size = sizeof(clientInfo);

	SOCKET ClientConn;


	ClientConn = accept(ServSock, (sockaddr*)&clientInfo, &clientInfo_size);

	if (ClientConn == INVALID_SOCKET) 
	{
		cout << "Client detected, but can't connect to a client. Error # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		closesocket(ClientConn);
		WSACleanup();
		return 1;
	}
	else 
	{
		cout << "Connection to a client established successfully" << endl;
		cout << "Client connected with IP address " << IPToString(clientInfo.sin_addr.s_addr) << endl;
	}

	//Exchange text data between Server and Client. Disconnection if a client send "xxx"

	vector <char> servBuff(BUFF_SIZE), clientBuff(BUFF_SIZE);							// Creation of buffers for sending and receiving data
	short packet_size = 0;												// The size of sending / receiving packet in bytes

	while (true) {
		packet_size = recv(ClientConn, servBuff.data(), servBuff.size(), 0);					// Receiving packet from client. Program is waiting (system pause) until receive
		cout << "Client's message: " << servBuff.data() << endl;

		cout << "Your (host) message: ";
		fgets(clientBuff.data(), clientBuff.size(), stdin);

		// Check whether server would like to stop chatting 
		if (clientBuff[0] == 'x' && clientBuff[1] == 'x' && clientBuff[2] == 'x') {
			shutdown(ClientConn, SD_BOTH);
			closesocket(ServSock);
			closesocket(ClientConn);
			WSACleanup();
			return 0;
		}

		packet_size = send(ClientConn, clientBuff.data(), clientBuff.size(), 0);

		if (packet_size == SOCKET_ERROR) {
			cout << "Can't send message to Client. Error # " << WSAGetLastError() << endl;
			closesocket(ServSock);
			closesocket(ClientConn);
			WSACleanup();
			return 1;
		}
	}
	closesocket(ServSock);
	closesocket(ClientConn);
	WSACleanup();

	return 0;
}
#ifndef _TIGA_NETWORK_H_
#define _TIGA_NETWORK_H_

#include <ws2tcpip.h>
#include <process.h>

enum NetMsgType
{
	netMsgHello,	/// the first message sent by new client
	netMsgAssignID,	/// server sends it to the new client	
	netMsgChat,		/// common chat
};

struct NetMsgHeader
{
	uint16 type;	
	uint16 length;
	uint32 timestamp;
};

#pragma once

const int DEFAULT_BUFLEN=32;
const char DEFAULT_PORT[]="2000";

class Network
{
	WSADATA wsaData;
	struct addrinfo *result,hints;
public:
	Network()
		:result(NULL)
	{
		// Initialize Winsock
		int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
		if (iResult != 0) {
			printf("WSAStartup failed: %d\n", iResult);
			//return 1;
		}
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;
		
		// Resolve the server address and port
		iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
		if ( iResult != 0 ) {
			printf("getaddrinfo failed: %d\n", iResult);
			//WSACleanup();
			//return 1;
		}
	}
	SOCKET createSocket()
	{
		 return socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	}

	~Network()
	{
		freeaddrinfo(result);
		WSACleanup();
	}
};

class Connection
{
protected:
	char dataBuffer[DEFAULT_BUFLEN];
	SOCKET listenSocket,clientSocket;
	//HANDLE thread;
	//unsigned threadID;
	bool server;
	bool reconnect;
	bool waitConnection;
	bool connected;
	std::vector<char> buffer;
	Network &network;
	std::string IP;
	int port;
public:
	Connection(Network &net,const char *nIP,int nPort,bool mode,int bufferSize)
		:network(net),listenSocket(INVALID_SOCKET),clientSocket(INVALID_SOCKET)/*,thread(NULL),threadID(-1)*/,server(mode),buffer(bufferSize),port(nPort)
	{
		if(nIP)
			IP=nIP;
		reconnect=true;
		connected=false;
	}
	~Connection()
	{
		close();
	}
	void close()
	{
		if(listenSocket!=INVALID_SOCKET)
		{
			closesocket(listenSocket);
			listenSocket=INVALID_SOCKET;
		}
		if(clientSocket!=INVALID_SOCKET)
		{
			closesocket(clientSocket);
			clientSocket=INVALID_SOCKET;
		}
		connected=false;
	}
	int doServer()
	{
		int iResult=0;
		printf("General net: Creating a server\n");
		// Create a SOCKET for connecting to server
		//listenSocket = socket(network.result->ai_family, network.result->ai_socktype, network.result->ai_protocol);
		listenSocket = network.createSocket();
		if (listenSocket == INVALID_SOCKET) {
			printf("-socket failed: %ld\n", WSAGetLastError());		
			return 0;
		}
		printf("General net: Binding a socket\n");
		sockaddr_in addr; // переменная для TCP сокета
		addr.sin_family = AF_INET;      // Семейство адресов - Internet
		addr.sin_port = htons (port);   // Назначаем 5001 порт сокету
		addr.sin_addr.s_addr = htonl (INADDR_ANY);   // Без конкретного адреса
		if (bind(listenSocket, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR)
		{
			printf("-bind failed: %d\n", WSAGetLastError());			
			close();			
			return -1;
		}
		printf("General net: Listening\n");
		do
		{
			iResult = listen(listenSocket, SOMAXCONN);
			/*if (iResult == SOCKET_ERROR) {
				printf("listen failed: %d\n", WSAGetLastError());
				closesocket(ListenSocket);
				WSACleanup();
				return 1;
			}*/
		}while(iResult==SOCKET_ERROR);
		// Accept a client socket
		printf("General net: Accepting the connection\n");		
		clientSocket = accept(listenSocket, NULL, NULL);
		if (clientSocket == INVALID_SOCKET) {
			printf("-accept failed: %d\n", WSAGetLastError());
			close();
			return 0;
		}
		else
			printf("-accepted\n");
		connected=true;
		int res=onConnect();	// maybe here i should sometimes deny incoming connection?
		return 1;
	}
	bool valid()
	{
		return clientSocket!=INVALID_SOCKET;
	}
	int doClient()
	{
		printf("General net: creating client\n");
		clientSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
		if ( clientSocket == INVALID_SOCKET ) 
		{
			printf( "General net: Error at socket(): %ld\n", WSAGetLastError() );
			return -1;
		}		
		sockaddr_in clientService;
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr(IP.c_str() );
		clientService.sin_port = htons( port );

		int res=SOCKET_ERROR;
		printf("General net: waiting for a server\n");
		while(res==SOCKET_ERROR)
		{
			if ((res= connect( clientSocket, (SOCKADDR*) &clientService, sizeof(clientService) )) == SOCKET_ERROR) 
			{
				//printf( "doNet error: Failed to connect.\n" );
				//return -1;
			}
		}
		printf("-connected\n");
		connected=true;
		onConnect();		
		return 1;
	}
	int doRecieve()
	{
		int iResult=0;
		do {
			iResult = recv(clientSocket, &buffer.front(), buffer.size(), 0);
			if (iResult > 0) 
			{		
				onRecieve(&buffer.front(),buffer.size());
			
			}
			else if (iResult == 0)
				printf("Connection closing...\n");
			else  
			{
				printf("recv failed: %d\n", WSAGetLastError());           
				break;
			}
		} while (iResult > 0);
		
		return iResult;
	}
	void run()
	{
		do
		{
			int res=0;
			// 1. create connection		
			res=server?doServer():doClient();
			if(res==0)
				continue;
			else if(res==-1)
				break;		
			// 2. listen for data
			doRecieve();
			close();
		}while(reconnect);
		//if(!thread)
		//	thread = (HANDLE)_beginthreadex( NULL, 0, (DoNet*)(void*)&doNet, this, 0, &threadID );		
		close();
	}	
	virtual int send(const char *buffer,int size)
	{
		int res=0;
		if(connected)
			res=::send( clientSocket, buffer, size, 0 );
		else
		{
			printf("General net: ERROR sending to invalid connection\n");
		}
		return res;
	}
	virtual int onRecieve(const char *buffer,int size)
	{
		return 1;
	}
	virtual int onConnect()
	{
		return 1;
	}
};

#endif

#pragma once

#include <stdint.h>
#include <iostream>
#include <string>
#include <cstring>
#include <iomanip>
#ifdef _WIN32
#include <winsock2.h>
#include <WS2tcpip.h>
typedef int socklen_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

class clienttcp
{
private:
#ifdef _WIN32
	SOCKET clientSocket;
#else
	int clientSocket;
#endif
	sockaddr_in serverAddress;
public:	
	clienttcp() : clientSocket(INVALID_SOCKET)
	{
		memset(&serverAddress, 0, sizeof(serverAddress)); 
	}

	bool Initialize()
	{
#ifdef _WIN32
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			std::cerr << "Failed to initialize Winsock." << std::endl;
			return false;
		}
#endif
		return true;
	}

	bool createSocket()
	{
		clientSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (clientSocket < 0)
		{
			std::cerr << "Failed to create client socket" << std::endl;
			return false;
		}
		return true;
	}

	bool Connect(const std::string& serverIP, int port)
	{
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(port);
		if (inet_pton(AF_INET, serverIP.c_str(), &(serverAddress.sin_addr)) < 0)
		{
			std::cerr << "Failed to convert IP address." << std::endl;
			return false;
		}
		if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
		{
			std::cerr << "Failed to connect server Socket." << std::endl;
			return false;
		}
		return true;
	}

	bool Send(const std::string& data)
	{
		int bytesSent = send(clientSocket, data.c_str(), static_cast<int>(data.length()), 0);
		if (bytesSent <0)
		{
			std::cerr << "Failed to send data" << std::endl;
			return false;
		}
		return true;
	}

	std::string Receive(int bufferSize = 4096)
	{
		char buffer[4096];
		memset(buffer, 0, sizeof(buffer));
		int bytesRead = recv(clientSocket, buffer, bufferSize, 0);
		if (bytesRead < 0)
		{
			std::cerr << "Failed to receive data from the server." << std::endl;
			return "";
		}
		return std::string(buffer);
	}

	void initNet()
	{
		clienttcp client;
		if (client.Initialize())
		{
			std::cout << "Successfully initialized network" << std::endl;
			if (client.createSocket())
			{
				std::cout << "Successfully created socket" << std::endl;
				if (client.Connect("127.0.0.1", 80))
				{
					std::cout << "Successfully connected to port 80" << std::endl;
				}
			}
		}
	}

	void cleanUP()
	{
#ifdef _WIN32
		closesocket(clientSocket);
		WSACleanup();
#else
		close(clientSocket);
#endif
	}
};
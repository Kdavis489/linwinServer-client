#pragma once

#include <stdint.h>
#include <iostream>
#include <string>
#include <iomanip>
#ifdef _WIN32
#include <winsock2.h>
#include <WS2tcpip.h>
typedef int socklen_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

class servertcp
{
private:
	SOCKET serverSocket;
	SOCKET clientSocket;
	WSADATA wsaData;
	sockaddr_in serverAddress;
	sockaddr_in clientAddress;
/*sub socklen_t*/	int clientAddressSize;

public:
	servertcp() : serverSocket(INVALID_SOCKET), clientSocket(INVALID_SOCKET), clientAddressSize(sizeof(clientAddress)) 
	{ 
		memset(&clientAddress, 0, sizeof(clientAddress)); 
	}

	bool Initialize()
	{
#ifdef _WIN32
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
		serverSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (serverSocket < 0)
		{
			std::cerr << "Failed to create server socket" << std::endl;
			return false;
		}
		return true;
	}

	bool Bind(int port)
	{
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_addr.s_addr = INADDR_ANY;
		serverAddress.sin_port = htons(port);
		if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
		{
			std::cerr << "Failed to bind server Socket." << std::endl;
			return false;
		}
		return true;
	}

	bool Listen(int backlog = SOMAXCONN)
	{
		if (listen(serverSocket, backlog) < 0)
		{
			std::cerr << "Faile to listen for client connections." << std::endl;
			return false;
		}
		return true;
	}
	bool Accept()
	{
		clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressSize);
		if (clientSocket < 0)
		{
			std::cerr << "Failed to accept client connection." << std::endl;
			return false;
		}
		return true;
	}

	bool Send(const std::string& data)
	{
		int bytesSent = send(clientSocket, data.c_str(), static_cast<int>(data.length()), 0);
		if (bytesSent < 0)
		{
			std::cerr << "Failed to send data to the client." << std::endl;
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
			std::cerr << "Failed to receivedata from the client." << std::endl;
			return "";
		}
		return std::string(buffer);
	}
	void initNet()
	{
		servertcp server;
		if (server.Initialize())
		{
			std::cout << "successfully initialized network" << std::endl;
			if (server.createSocket())
			{
				std::cout << "created Socket successfully" << std::endl;
				if (server.Bind(80))
				{
					if (server.Listen())
					{
						std::cout << "Successfully listening on port 80" << std::endl;
						if (server.Accept())
						{
							std::cout << "Successfully accepted client" << std::endl;
						}
					}
				}
			}
		}
	}
	void cleanUP()
	{
#ifdef _WIN32
		closesocket(clientSocket);
		closesocket(serverSocket);
		WSACleanup();
#else
		close(clientSocket);
		close(serverSocket);
#endif
	}
};
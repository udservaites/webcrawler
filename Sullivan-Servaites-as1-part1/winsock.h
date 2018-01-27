/*
Kelsea Sullivan
Kathryn Rebecca Servaites

Assignment 1 Part 1
CPS 570 Spring 2018
01-23-2018

winsock.h
This class contains all the functionality to connect to a website and to receive the data from it. 
*/

#pragma once
#pragma comment (lib, "Ws2_32.lib")  // link to winsock lib
#define _WINSOCK_DEPRECATED_NO_WARNINGS  // for inet_addr(), gethostbyname()

#include <winsock2.h>   // the .h file defines all windows socket functions 

#include "common.h" 


#define BUF_SIZE 1000000 // array size
#define TIMEOUT 20000 // 20 seconds


class Winsock
{
public:
	static int initialize()   // call Winsock::intialize() in main, to intialize winsock only once
	{
		WSADATA wsaData;
		int iResult;

		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);   // defined in winsock2.h
		if (iResult != 0) {
			printf("WSAStartup failed: %d\n", iResult);
			WSACleanup();
			return 1;  // 1 means failed
		}
		return 0;   // 0 means no error (i.e., successful)
	}

	static void cleanUp() // call Winsock::cleanUp() in main only once
	{
		WSACleanup();
	}

	int createTCPSocket(void)
	{
		sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sock == INVALID_SOCKET) {
			printf("socket() error %d\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}
		return 0;
	}

	// hostName (e.g., "www.google.com"),  2-byte port (e.g., 80)
	int connectToServer(string hostName, short port)
	{
		struct sockaddr_in server; // structure for connecting to server
		struct hostent *remote;    // structure used in DNS lookups: convert host name into IP address

		if ((remote = gethostbyname(hostName.c_str())) == NULL)
		{
			printf("Invalid host name string: not FQDN\n");
			return 1;  // 1 means failed
		}
		else // take the first IP address and copy into sin_addr
		{
			memcpy((char *)&(server.sin_addr), remote->h_addr, remote->h_length);
		}

		// setup the port # and protocol type
		server.sin_family = AF_INET;  // IPv4
		server.sin_port = htons(port);// host-to-network flips the byte order

									  // connect to the server 
		if (connect(sock, (struct sockaddr*) &server, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
		{
			printf("Connection error: %d\n", WSAGetLastError());
			return 1;
		}
		printf("Successfully connected to %s (%s) on port %d\n", hostName.c_str(), inet_ntoa(server.sin_addr),
			htons(server.sin_port));
		return 0;
	}

	// dot-separated hostIP (e.g., "132.11.22.2"), 2-byte port(e.g., 80)
	int connectToServerIP(string hostIP, short port)
	{
		struct sockaddr_in server; // structure for connecting to server

		DWORD IP = inet_addr(hostIP.c_str());
		if (IP == INADDR_NONE)
		{
			printf("Invalid IP string: nor IP address\n");
			return 1;  // 1 means failed						
		}
		else
		{
			server.sin_addr.S_un.S_addr = IP; // if a valid IP, directly drop its binary version into sin_addr
		}

		// setup the port # and protocol type
		server.sin_family = AF_INET;  // IPv4
		server.sin_port = htons(port);// host-to-network flips the byte order

		if (connect(sock, (struct sockaddr*) &server, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
		{
			printf("Connection error: %d\n", WSAGetLastError());
			return 1;
		}
		printf("Successfully connected to %s (%s) on port %d\n", hostIP.c_str(), inet_ntoa(server.sin_addr),
			htons(server.sin_port));

		return 0;
	}


	// define your sendRequest(...) function, to send a HEAD or GET request
	int sendRequest(string sendstring)
	{
		int size = sendstring.length(); // length of string, in terms of bytes
		if (send(sock, sendstring.c_str(), size, 0) == SOCKET_ERROR)
			return false; // your function returns false
		else
			return true; // otherwise successfully send a GET request
	}

	// define your receive(...) function, to receive the reply from the server
	bool recvRequest(string & recv_string) //recv_string is modified after function is called
	{
		char buf[BUF_SIZE];
		FD_SET Reader; // for select() function call
		FD_ZERO(&Reader);
		FD_SET(sock, &Reader); // add your socket to the set Reader
							   // set timeout, used for select()
		struct timeval timeout;
		timeout.tv_sec = TIMEOUT; // must include <time.h>
		timeout.tv_usec = 0;
		recv_string = ""; // initialized as an empty string, used to save all received data
		int bytes = 0; // count how many bytes received via each recv()
		do
		{
			if (select(0, &Reader, NULL, NULL, &timeout) > 0) // if have data
			{
				if ((bytes = recv(sock, buf, BUF_SIZE - 1, 0)) == SOCKET_ERROR)
				{
					printf("failed with %d on recv\n", WSAGetLastError());
					return false;
				}
				else if (bytes > 0)
				{
					buf[bytes] = 0; // NULL terminate buffer
					recv_string += buf; // append to the string recv_string
				}
				// quit loop if it hits the maximum size, i.e., 2 MB for pages, 16 KB for robots
			}
			else
			{
				// timed out on select()
				return false;
			}
		} while (bytes > 0); // end of do-while
		return true;
	}

	void closeSocket(void)
	{
		closesocket(sock);
	}



private:
	SOCKET sock;

	// define other private variables if needed


};
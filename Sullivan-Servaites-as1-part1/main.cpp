/*
Kelsea Sullivan
Kathryn Rebecca Servaites

Assignment 1 Part 1
CPS 570 Spring 2018
01-23-2018

main.cpp
This file will connect to a single website and download the contents, printing them to the screen and to a text file. 
*/

#pragma once
#include "common.h"
#include "winsock.h"
#include "urlparser.h"
#include "fstream"
#include <windows.h>



#define BUFFERSIZE 65000000
#define MAX_NUM_ERRORS -10

using namespace std;

enum errors
{
	incorrect_num_args = MAX_NUM_ERRORS,
	incorrect_num_threads,
	file_open_err,
	recv_request_err,
	send_request_err,
};

void return_error(int error)
{
	cout << "Enter any key to continue ...\n";
	getchar();
	exit(error);
}

int main(int argc, char **argv)
{
	// Check number of arguments
	if (argc != 3)
	{
		cout << "Incorrect number of arguments. \n";
		cout << "Expecting 2 arguments: 1 website_url \n";
		cout << "Example: asgn1.exe 1 http://www.symantec.com/verisign/ssl-certificates \n\n";
		//return 1;
		return_error(errors::incorrect_num_args);
	}

	// Check number of threads given
	if (atoi(argv[1]) != 1)
	{
		cout << "Number of threads not equal 1. Please set 2nd argument equal to 1.\n\n";
		//return 1;
		return_error(errors::incorrect_num_threads);
	}
	//cout << "Got past the command line" << endl;



	// Set up Winsock
	Winsock::initialize();	// initialize 
	Winsock ws;

	// Loop over each URL in file
	string url = argv[2];
	// Print URL
	//cout << "URL: " << url << "\n";
	//outfile << "URL: " << URL_Array << "\n";

	// Parse URL
	URLParser parser(url);
	ofstream outfile;
	outfile.open("asgn1_output.txt");

	// Create send request
	string sendstring = "GET /" + parser.getPath() +
		" HTTP/1.0\nUser-agent: UDCScrawler/1.0\nHost: " +
		parser.getHost() + "\nConnection: close" + "\n\n";
	// pay attention to required white space
	int size = sendstring.length();

	// Setup socket
	ws.createTCPSocket();

	// If can connect...
	if (ws.connectToServer(parser.getHost(), parser.getPort()) == 0)
	{
		// Send request
		if (ws.sendRequest(sendstring))
		{
			// Send was successful, now need to receive
			string * recvstring = new string();

			if (ws.recvRequest(*recvstring))
			{
				cout <<  *recvstring;
				outfile << *recvstring;
			}
			else
			{
				cout << "Receive Request Failed." << endl;
				return_error(errors::recv_request_err);
			}

			delete recvstring;
		}
		else
		{
			cout << "Send() error - %d\n", WSAGetLastError();
			return_error(errors::send_request_err);
		}
	}
	else
	{
		cout << "Cannot connect to server. Assuming invalid domain & skipping over to next URL.\n";
		//return_error( errors::connect_server_err );
	}

	// Close socket
	ws.closeSocket();

	// Wrap up
	Winsock::cleanUp();

	printf("Enter any key to continue ...\n");
	getchar();
	return 0;   // 0 means successful
}
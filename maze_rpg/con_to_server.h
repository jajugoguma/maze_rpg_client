#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
//   #include <unistd.h>
#include <errno.h>
#include <string>
//    #include <netdb.h>
#include <sys/types.h>
#include <winsock2.h>
//    #include <netinet/in.h>
//    #include <sys/socket.h>
#include <vector>
#include <windows.h>
#include <conio.h>

#pragma comment(lib, "ws2_32")

#define IP   "203.249.22.41"
#define PORT 8090

#define MAXDATASIZE 1024 // max number of bytes we can get at once 

using namespace std;

const int cols = 79, rows = 39;
const int h_cols = cols / 2, h_rows = rows / 2;

struct test_str
{
	char name[MAXDATASIZE];
};

class Connection {
public:
	HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos;

	SOCKET _socket;
	struct test_str ts;
	int numbytes;
	char buf[MAXDATASIZE];
	struct sockaddr_in their_addr; // connector's address information 
	vector<vector<string> *> str_vector; 

	WSADATA wsa;

	Connection();
	~Connection();

	int swap(int i);
	void con_to_server();
	void read_data(vector<string> datas);
	string send_data(vector<string>);
	void discon_from_server();

	void con_fail();
};
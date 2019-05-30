#include "con_to_server.h"

Connection::Connection() {
	this->con_to_server();
}

Connection::~Connection() {
	this->discon_from_server();
}

int Connection::swap(int i) {
	int byte0 = i & 0xff;
	int byte1 = (i >> 8) & 0xff;
	int byte2 = (i >> 16) & 0xff;
	int byte3 = (i >> 24) & 0xff;

	return (byte0 << 24) | (byte1 << 16) | (byte2 << 8) | byte3;
};

void Connection::con_to_server() {

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		con_fail();
		fprintf(stderr, ""/*"[ERROR0] : <<  \n"*/);
		exit(1);
	}


	if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		con_fail();
		fprintf(stderr, ""/*"[%d] Socket_Create() \n"*/, WSAGetLastError());
		exit(1);
	}

	their_addr.sin_family = AF_INET;    // host byte order 
	their_addr.sin_port = htons(PORT);  // short, network byte order 
	their_addr.sin_addr.s_addr = inet_addr(IP); //*((struct in_addr *)he->h_addr);
	memset(&(their_addr.sin_zero), '\0', 8);  // zero the rest of the struct 

	if (connect(_socket, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == SOCKET_ERROR) {
		con_fail();
		fprintf(stderr, ""/*" Socket_Connect() Error : [%d]\n"*/, WSAGetLastError());
		exit(1);
	}

	//printf("Connection success");
}

void Connection::read_data(vector<string> datas) {
	for (vector<string> *tmp : str_vector)
		delete(tmp);
	str_vector.clear();
	buf[0] = '\0';

	string senddata = "_,";

	for (int i = 0; i < datas.size(); i++) {
		senddata += datas[i] + ",";
	}
	senddata += "\n";

	char *bytes = new char[senddata.length()];

	strncpy(bytes, senddata.c_str(), senddata.length());

	//send data
	if (send(_socket, (const char *)bytes, strlen(bytes), 0) == SOCKET_ERROR) {
		fprintf(stderr, " Socket_Send() Error : [%d]\n", WSAGetLastError());
		exit(1);
	}
	
	//receive data
	if ((numbytes = recv(_socket, buf, MAXDATASIZE - 1, 0)) == SOCKET_ERROR) {
		fprintf(stderr, " Socket_Recv() Error : [%d]\n", WSAGetLastError());
		exit(1);
	}

	if (datas[0] == "SELCHAR" && buf[0] != '\0') {
		int i = 0, j = 0;
		
		str_vector.push_back(new vector<string>);

		char *token = strtok(buf, ",");
		while (token) {
			if (j == 13) {
				i++;
				j = 0;
				str_vector.push_back(new vector<string>);
			}

			str_vector[i]->push_back(token);
			token = strtok(NULL, ",");
			j++;
		}
	}
	else if (datas[0] == "ITEM" && buf[0] != '\0') {
		int i = 0, j = 0;

		str_vector.push_back(new vector<string>);

		char *token = strtok(buf, ",");
		while (token) {
			if (j == 6) {
				i++;
				j = 0;
				str_vector.push_back(new vector<string>);
			}

			str_vector[i]->push_back(token);
			token = strtok(NULL, ",");
			j++;
		}
	}
	else if (datas[0] == "LOADITEM" && buf[0] != '\0') {
		int i = 0, j = 0;

		str_vector.push_back(new vector<string>);

		char *token = strtok(buf, ",");
		while (token) {
			if (j == 2) {
				i++;
				j = 0;
				str_vector.push_back(new vector<string>);
			}

			str_vector[i]->push_back(token);
			token = strtok(NULL, ",");
			j++;
		}
	}
}

string Connection::send_data(vector<string> datas) {
	str_vector.clear();

	string senddata = "_,";

	for (int i = 0; i < datas.size(); i++) {
		senddata += datas[i] + ",";
	}
	senddata += "\n";

	char *bytes = new char[senddata.length()];

	strncpy(bytes, senddata.c_str(), senddata.length());

	//send data
	if (send(_socket, (const char *)bytes, strlen(bytes), 0) == SOCKET_ERROR) {
		fprintf(stderr, " Socket_Send() Error : [%d]\n", WSAGetLastError());
		exit(1);
	}

	//get result
	if ((numbytes = recv(_socket, buf, MAXDATASIZE - 1, 0)) == SOCKET_ERROR) {
		fprintf(stderr, " Socket_Recv() Error : [%d]\n", WSAGetLastError());
		exit(1);
	}

	string result(buf);

	return result;
}

void Connection::discon_from_server() {
	vector<string> data;
	data.push_back("ENDCON");
	read_data(data);
	closesocket(_socket);
}

void Connection::con_fail() {
	string title = "Fail to connect server";
	pos.X = (cols - title.length()) / 2;
	pos.Y = h_rows;
	SetConsoleCursorPosition(hConsoleOutput, pos);
	cout << title << "\n";
	while (!_kbhit());
}
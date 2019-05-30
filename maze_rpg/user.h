#pragma once
#include <iostream>
#include <string>
#include "character.h"

const int MAX_CHARS = 3;

using namespace std;

class USER {
	string id;
	string passwd;
	
	int created_char = 0;
public:
	CHARACTER *character[MAX_CHARS] = { NULL, NULL, NULL };
	
	USER(string id, string passwd);
	~USER();

	bool create_character(string name);
	bool create_character(vector<string> data);

	string* get_chars();

	int get_created_char();

	string get_id();

	void clear_chars();
};

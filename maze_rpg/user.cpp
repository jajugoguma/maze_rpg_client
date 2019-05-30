#include "user.h"

USER::USER(string id, string passwd) {
	this->id = id;
	this->passwd = passwd;
}
USER::~USER() {}

bool USER::create_character(string name) {
	if (created_char < MAX_CHARS) {
		character[created_char++] = new CHARACTER(this->id, name);
		return true;
	}
	else
		return false;
}
bool USER::create_character(vector<string> data) {
	if (created_char < MAX_CHARS) {
		character[created_char++] = new CHARACTER(this->id, data);
		return true;
	}
	else
		return false;
}

string* USER::get_chars() {
	string *sa = new string[MAX_CHARS];
	for (int i = 0; i < MAX_CHARS; i++) {
		if (i < created_char) {
			sa[i] = character[i]->get_name();
		}
		else
			sa[i] = "NO Character";
	}

	return sa;
}

int USER::get_created_char() {
	return created_char;
}

string USER::get_id() {
	return this->id;
}

void USER::clear_chars() {
	for (int i = 0; i < num_of_char; i++) {
		if (character[i] != NULL) {
			delete(character[i]);
			character[i] = NULL;
		}
	}
	created_char = 0;
}
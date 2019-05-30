#pragma once
#include <iostream>
#include <String>

using namespace std;

class Item {
private:
	int itme_no;
	string name;
	int class_no;
	int effect;
	string explain;

public:
	Item(int, string, int, int, string);
	~Item();
	int get_item_no();
	string get_name();
	int get_class_no();
	int get_effect();
	string get_explain();

	bool operator==(const Item &item) const {
		return (this->itme_no == item.itme_no);
	}
};
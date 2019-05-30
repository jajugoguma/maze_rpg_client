#pragma once
#include <iostream>
#include <String>
#include <vector>

using namespace std;

class Item {
private:
	int itme_no;
	string name;
	int effect;
	string explain;
	int duration;
	int cost;

public:
	Item(vector<string>);
	~Item();
	int get_item_no();
	string get_name();
	int get_effect();
	string get_explain();
	int get_duration();
	int get_cost();

	bool operator==(const Item &item) const {
		return (this->itme_no == item.itme_no);
	}
};
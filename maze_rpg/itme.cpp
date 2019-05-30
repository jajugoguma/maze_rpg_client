#include "item.h"

Item::Item(vector<string> data) {
	this->itme_no = atoi(data[0].c_str());
	this->name = data[1];
	this->effect = atoi(data[2].c_str());
	this->explain = data[3];
	this->duration = atoi(data[4].c_str());
	this->cost = atoi(data[5].c_str());
}

Item::~Item() {}

int Item::get_item_no() {
	return this->itme_no;
}

string Item::get_name() {
	return this->name;
}

int Item::get_effect() {
	return this->effect;
}

string Item::get_explain() {
	return this->explain;
}

int Item::get_duration() {
	return this->duration;
}

int Item::get_cost() {
	return this->cost;
}
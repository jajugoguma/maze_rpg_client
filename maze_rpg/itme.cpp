#include "item.h"

Item::Item(int itme_no, string name, int class_no, int effect, string explain) {
	this->itme_no = itme_no;
	this->name = name;
	this->class_no = class_no;
	this->effect = effect;
	this->explain = explain;
}

Item::~Item() {}

int Item::get_item_no() {
	return this->itme_no;
}

string Item::get_name() {
	return this->name;
}

int Item::get_class_no() {
	return this->class_no;
}

int Item::get_effect() {
	return this->effect;
}

string Item::get_explain() {
	return this->explain;
}
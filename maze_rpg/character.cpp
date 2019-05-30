#include "character.h"

CHARACTER::CHARACTER(string user_id, string name) {
	this->user_id = user_id;
	this->name = name;
	this->level = 1;
	this->exp = 0;
	this->status[0] = 5;
	this->status[1] = 5;
	this->status[2] = 5;
	this->max_hp = status[1] * 10;
	this->cur_hp = this->max_hp;
	this->atk = status[0] * 0.7;
	this->def = status[0] * 0.2 + status[1] * 0.4;
	this->state_num = 1;
	this->ap = 0;
	this->money = 500;
}

CHARACTER::CHARACTER(string user_id, vector<string> data) {
	this->user_id = user_id;
	this->name = data[0];
	this->level = atoi(data[1].c_str());
	this->def = atof(data[2].c_str());
	this->atk = atof(data[3].c_str());
	this->max_hp = atof(data[4].c_str());
	this->cur_hp = atof(data[5].c_str());
	this->exp = atoi(data[6].c_str());
	this->state_num = atoi(data[7].c_str());
	this->status[0] = atoi(data[8].c_str());
	this->status[1] = atoi(data[9].c_str());
	this->status[2] = atoi(data[10].c_str());
	this->ap = atoi(data[11].c_str());
	this->money = atoi(data[12].c_str());
}

CHARACTER::~CHARACTER() {}

string CHARACTER::get_name() {
	return name;
}

tuple<string, int, double, double, int> CHARACTER::get_infos() {
	return make_tuple(this->name, this->level, this->max_hp, this->cur_hp, this->exp);
}

void CHARACTER::add_cur_hp(double n) {
	this->cur_hp = this->cur_hp + n < this->max_hp ? this->cur_hp + n : this->max_hp;
}

void CHARACTER::set_cur_hp(double n) {
	this->cur_hp = n;
}

double CHARACTER::get_cur_hp() {
	return this->cur_hp;
}

double CHARACTER::get_max_hp() {
	return this->max_hp;
}

double CHARACTER::get_atk() {
	return this->atk;
}

int CHARACTER::get_luck() {
	return this->status[2];
}	

void CHARACTER::add_to_inven(Item item) {
	int i;
	for (i = 0; i < inventory.size(); i++) {
		if (inventory[i] == item) {
			inventory_cnt[i]++;
			return;
		}
	}

	inventory.push_back(item);
	inventory_cnt.push_back(1);

}
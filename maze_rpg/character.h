#pragma once
#include <iostream>	
#include <string>
#include <tuple>
#include <vector>
#include "item.h"

static int num_of_char = 0;

using namespace std;

class CHARACTER {
public:
	string user_id;
	string name;
	int level;
	int state_num;
	int status[3]; //first : stregth, second : constitution, third : luck
	double max_hp; //con * 10
	double cur_hp;
	int exp;
	double atk; //attack : str * 0.7
	double def; //defense : str * 0.2 + con * 0.4
	int ap;
	int money;

	vector<Item> inventory;
	vector<int> inventory_cnt;

	CHARACTER(string user_id, string name);
	CHARACTER(string user_id, vector<string> data);
	~CHARACTER();

	string get_name();
	tuple<string, int, double, double, int> get_infos();
	void add_cur_hp(double n);
	void set_cur_hp(double n);
	double get_cur_hp();
	double get_max_hp();
	double get_atk();
	int get_luck();
	void add_to_inven(Item item);
	void load_inven(Item item, int amount);
};
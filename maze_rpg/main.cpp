#define	_CRT_SECURE_NO_WARNINGS

#include "user.h"
#include "maze_generators.h"
#include "con_to_server.h"
#include <windows.h>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <algorithm>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

#include "Mmsystem.h"
#include "Digitalv.h"
MCI_OPEN_PARMS m_mciOpenParms;
MCI_PLAY_PARMS m_mciPlayParms;
DWORD m_dwDeviceID;
MCI_OPEN_PARMS mciOpen;
MCI_PLAY_PARMS mciPlay;

#define UNDERLINE "\033[4m"
#define DEUNDERLINE "\033[0m"
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define KEY_SLASH 47
#define KEY_A 97
#define KEY_R 114
#define KEY_I 105
#define KEY_S 115

using namespace std;

typedef struct Maze_pos {
	int col;
	int row;
} Maze_pos;

HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);

const int INIT_MENUS = 5;
const int COLUMNS = 79, ROWS = 39;
const int H_COLUMNS = COLUMNS / 2, H_ROWS = ROWS / 2;
const int MAZE_SIZE = 10;

USER *log_in_user;
CHARACTER *selected_char;
int num_users = 0;
bool cursor_on;
COORD pos;
vector< vector<int> > maze;
vector<Item> items;
Maze_pos start;
Maze_pos dest;
Maze_pos current;

string messages[20] = { " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " " };
string backuped_msgs[20] = { " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " " };
int top;
int backuped_top;

int dwID;

Item *using_item = NULL;
int remain_turn = -1;
int *i_target = NULL;
double *d_target = NULL;


Connection *conn;

//To cursor x and y
void set_cursor(int x, int y);
//Title print
void print_welcome();
//Main menu print and choose
void initial_menu();
//Clear only under title
void clear_in_main();
//Log in function
bool log_in();
//Sign up function
void sign_up();
//show ranking
void ranking();
//Show credit
void credit();
//Press enter to continue
void enter_to_continue();
//Cursor visiblity Switch, true : visible , false : invisible
void cur_visi_swit(bool flag);
//when a character is selected then true.
bool choose_character();

void create_char();

void delete_char();

boolean delete_acc();
//Dispose of input error when input wrong type.
void input_error();

void play();

void print_main_UI();

void print_msg(string msg);

void clear_msgs();

void item_store();

void show_inven(int state = 0);

void use_item(int, int);
//Fixed cursor row (Ignore press enter when input empty string)
void t_fix_cur_row(int x, int y);

void t_maze_gen();

void t_save();

void set_font_size();

void start_stage();

void print_maze();

void input_com_maze();

void input_com_fight();

void update_ui_maze();

bool fight_enemy();

bool spawn_enemy();

void level_up();

string inputs(int x, int y);

string input_in_game();

//void disable_echo();
//
//void enable_echo();

void bgm_on();

void bgm_off();

void t_keystrok_sound();

void get_item_infos();

void status();

void save_msgs();

void restore_msgs();

BOOL CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// Handle the CTRL+C signal. 
	case CTRL_C_EVENT:
		break;
	case CTRL_CLOSE_EVENT: // CTRL+CLOSE: confirm! that the user wants to exit. 
		delete(conn);
		break;
	case CTRL_BREAK_EVENT:
		break;
	case CTRL_LOGOFF_EVENT:
		break;
	case CTRL_SHUTDOWN_EVENT:
		//delete(conn);
		break;
	default:
		return FALSE; //TRUE로하면 에러가나고 FALSE하면 걍 잘꺼지고잘됨.
	}
}

int main() {
	
	//thread t(t_keystrok_sound);
	//프로그램 강제 종료시 핸들러 설정
	BOOL fSuccess = SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
	if (!fSuccess)
	{
		printf("Could not set control handler");
		return 1;
	}

	//cout << fixed;
	//cout.precision(2);

	cur_visi_swit(false);
	set_font_size();
	system("mode con cols=79 lines=39 | title MAZE RPG");

	//Sleep(30000);
	bgm_on();

	conn = new Connection();
	get_item_infos();
	print_welcome();
	initial_menu();

	

	//t.join();
	bgm_off();
	delete(conn);
	return 0;
}

void bgm_on() {
	mciOpen.lpstrElementName = "audio/Apprehensive_at_Best.mp3"; // 파일 경로 입력
	mciOpen.lpstrDeviceType = "mpegvideo";
	mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE, (DWORD)(LPVOID)& mciOpen);

	dwID = mciOpen.wDeviceID;

	mciSendCommand(dwID, MCI_PLAY, MCI_DGV_PLAY_REPEAT, (DWORD)(LPVOID)& m_mciPlayParms); //play and repeat

	//MCI_NOTIFY : 기본
	//MCI_DGV_PLAY_REPEAT : 반복재생
}

void bgm_off() {
	mciSendCommand(dwID, MCI_CLOSE, 0, (DWORD)(LPVOID)NULL); //BGM 종료
}

void get_item_infos() {


	vector<string> data;
	data.push_back("ITEM");
	conn->read_data(data);

	for (int i = 0; i < conn->str_vector.size(); i++) {
		Item tmp(*conn->str_vector[i]);
		items.push_back(tmp);
	}
}

void t_keystrok_sound() {
	while (TRUE) {
		//rewind(stdin);
		while (!_kbhit());
		PlaySound("audio/keystrok.wav", GetModuleHandle(NULL), SND_FILENAME | SND_ASYNC | SND_NODEFAULT); //한번 재생
	}
}

void t_fix_cur_row(int x, int y) {
	CONSOLE_SCREEN_BUFFER_INFO p, pre_p;
	GetConsoleScreenBufferInfo(hConsoleOutput, &pre_p);
	pre_p.dwCursorPosition.X = x;
	pre_p.dwCursorPosition.Y = y;

	while (cursor_on) {
		GetConsoleScreenBufferInfo(hConsoleOutput, &p);
		if (p.dwCursorPosition.Y != y)
			set_cursor(pre_p.dwCursorPosition.X, y);

		pre_p.dwCursorPosition.X = p.dwCursorPosition.X;
	}
}

void t_maze_gen() {
	srand((unsigned int)time(NULL));
	maze = rand_maze(MAZE_SIZE, MAZE_SIZE);
	//start Right or Left
	if (rand() % 2 != 0) {
		//start Right
		if (rand() % 2 != 0) {
			start.col = MAZE_SIZE - 1;
			start.row = rand() % MAZE_SIZE;
		}
		//start Left
		else {
			start.col = 0;
			start.row = rand() % MAZE_SIZE;
		}
	}
	//start Top ot Bottom
	else {
		//start Top
		if (rand() % 2 != 0) {
			start.col = rand() % MAZE_SIZE;
			start.row = 0;
		}
		//start Bottom
		else {
			start.col = rand() % MAZE_SIZE;
			start.row = MAZE_SIZE - 1;
		}
	}
	srand((unsigned int)time(NULL));
	//destiantion Right or Left
	if (rand() % 2 != 0) {
		//destiantion Right
		if (rand() % 2 != 0) {
			dest.col = MAZE_SIZE - 1;
			dest.row = rand() % MAZE_SIZE;
		}
		//destiantion Left
		else {
			dest.col = 0;
			dest.row = rand() % MAZE_SIZE;
		}
	}
	//destiantion Top ot Bottom
	else {
		//destiantion Top
		if (rand() % 2 != 0) {
			dest.col = rand() % MAZE_SIZE;
			dest.row = 0;
		}
		//destiantion Bottom
		else {
			dest.col = rand() % MAZE_SIZE;
			dest.row = MAZE_SIZE - 1;
		}
	}
}

void t_save() {
	vector<string> data;
	data.push_back("SAVE");
	data.push_back(selected_char->name);
	data.push_back(to_string(selected_char->level));
	data.push_back(to_string(selected_char->def));
	data.push_back(to_string(selected_char->atk));
	data.push_back(to_string(selected_char->max_hp));
	data.push_back(to_string(selected_char->cur_hp));
	data.push_back(to_string(selected_char->exp));
	data.push_back(to_string(selected_char->state_num));
	data.push_back(to_string(selected_char->status[0]));
	data.push_back(to_string(selected_char->status[1]));
	data.push_back(to_string(selected_char->status[2]));
	data.push_back(to_string(selected_char->ap));
	data.push_back(to_string(selected_char->money));

	for (int i = 0; i < selected_char->inventory.size(); i++) {
		data.push_back(to_string(selected_char->inventory[i].get_item_no()));
		data.push_back(to_string(selected_char->inventory_cnt[i]));
	}

	string result = conn->send_data(data);
	print_msg(result);
}

void set_font_size() {
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = 0;                 // Width of each character in the font
	cfi.dwFontSize.Y = 18;                  // Height
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;
	std::wcscpy(cfi.FaceName, L"Consolas"); // Choose your font
	SetCurrentConsoleFontEx(hConsoleOutput, FALSE, &cfi);
}

void cur_visi_swit(bool flag) {
	CONSOLE_CURSOR_INFO cursorInfo;
	cursorInfo.dwSize = 1;
	cursorInfo.bVisible = flag;
	cursor_on = flag;
	SetConsoleCursorInfo(hConsoleOutput, &cursorInfo);
}

void enter_to_continue() {
	if (cin.rdbuf()->in_avail())
		cin.ignore();
	cin.ignore();
}

void set_cursor(int x, int y) {
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(hConsoleOutput, pos);
}

void input_error() {
	if (cin.fail()) {
		cin.clear();
		cin.ignore(256, '\n');
	}
}

//void disable_echo() {
//	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
//	DWORD mode = 0;
//	GetConsoleMode(hStdin, &mode);
//	SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));
//
//	SetConsoleMode(hStdin, mode);
//}
//
//void enable_echo() {
//	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
//	DWORD mode = 0;
//
//	SetConsoleMode(hStdin, mode);
//}


void clear_in_main() {

	int end = H_ROWS - 3;
	for (int i = 0; i < end; i++) {
		set_cursor(0, H_ROWS + i + 1);
		for (int j = 0; j < COLUMNS; j++)
			cout << " ";
	}
}

string inputs(int x, int y) {
	string input;
	set_cursor(x, y);
	cur_visi_swit(true);
	thread t(t_fix_cur_row, x, y);
	cin >> input;
	input_error();
	cur_visi_swit(false);
	t.join();

	return input;
}

void print_welcome() {
	string title = "MAZE RPG";

	set_cursor((COLUMNS - title.length()) / 2, H_ROWS);

	for (unsigned int i = 0; i < title.length(); i++) {
		cout << title.at(i);
		Sleep(200);
	}
	system("color 08");	Sleep(500);
	system("color 0F");	Sleep(500);
	system("color 08");	Sleep(500);
	system("color 0F");	Sleep(500);
	cout << endl;

}

void initial_menu() {
	int i = 0;
	int input = -1;
	string menus[INIT_MENUS] = { "[1] LOG IN", "[2] SIGN UP","[3] Rank", "[9] CREDIT", "[OTHER] EXIT" };
	while (true) {
		string title = "MAZE RPG";
		set_cursor((COLUMNS - title.length()) / 2, H_ROWS);
		cout << title;

		clear_in_main();

		for (i = 0; i < INIT_MENUS; i++) {
			set_cursor((COLUMNS - menus[i].length()) / 2, H_ROWS + 2 + i);
			cout << menus[i];
			Sleep(50);
		}

		string menu_input = ">>   <<";
		set_cursor((COLUMNS - menu_input.length()) / 2, H_ROWS + i + 3);
		cout << UNDERLINE << menu_input;
		cout << DEUNDERLINE;
		input = atoi(inputs(H_COLUMNS, H_ROWS + i + 3).c_str());

		switch (input) {
		case 1:
			if (log_in()) {
				if (choose_character()) {
					play();
				}
			}
			break;
		case 2:
			sign_up();
			break;
		case 3:
			ranking();
			break;
		case 9:
			credit();
			break;
		default:
			return;
		}
	}
}

bool log_in() {
	int i;
	string id, passwd;
	string menus[2] = { "ID :           ", "PW :           " };

	while (true) {
		clear_in_main();
		for (i = 0; i < 2; i++) {
			set_cursor((COLUMNS - menus[i].length()) / 2, H_ROWS + 2 + i);
			cout << menus[i];
			Sleep(50);
		}

		id = inputs((COLUMNS - menus[0].length()) / 2 + 5, H_ROWS + 2);
		passwd = inputs((COLUMNS - menus[1].length()) / 2 + 5, H_ROWS + 3);

		vector<string> data;
		data.push_back("LOGIN");
		data.push_back(id);
		data.push_back(passwd);

		string result = conn->send_data(data);

		clear_in_main();
		set_cursor((COLUMNS - result.length()) / 2, H_ROWS + 2);
		cout << result;

		enter_to_continue();

		if (result != "Log In Complete")
			return false;

		log_in_user = new USER(id, passwd);
		return true;
	}
}

void sign_up() {
	int i, j = -1;
	string id, passwd;
	string menus[2] = { "ID :           ", "PW :           " };


	clear_in_main();
	for (i = 0; i < 2; i++) {
		set_cursor((COLUMNS - menus[i].length()) / 2, H_ROWS + 2 + i);
		cout << menus[i];
		Sleep(50);
	}

	id = inputs((COLUMNS - menus[0].length()) / 2 + 5, H_ROWS + 2);
	passwd = inputs((COLUMNS - menus[1].length()) / 2 + 5, H_ROWS + 3);

	vector<string> data;
	data.push_back("NEWACC");
	data.push_back(id);
	data.push_back(passwd);

	string result = conn->send_data(data);

	clear_in_main();
	set_cursor((COLUMNS - result.length()) / 2, H_ROWS + 2);
	cout << result;
	enter_to_continue();
}

void ranking() {
	string ranks[] = { "1st", "2nd", "3rd", "4th", "5th", "6th", "7th", "8th", "9th", "10th" };
	vector<string> data;
	data.push_back("RANK");
	conn->read_data(data);
	clear_in_main();

	string line = "RANKING";
	set_cursor((COLUMNS - line.length()) / 2, H_ROWS + 2);
	cout << line;

	for (int i = 0; i < 10; i++) {
		string line = ranks[i] + ". " + conn->str_vector[0]->at(i);
		set_cursor((COLUMNS - line.length()) / 2, H_ROWS + 4 + i);
		cout << line;
		Sleep(500);
	}
	enter_to_continue();
}

void credit() {
	system("cls");
	const int num_of_lines = 4;
	string s[num_of_lines] = { "made by DY", "For Database Programming Project", "Thanks for watching this", "Github ismaelbelghiti/maze-generators-cpp" };
	int x[num_of_lines];
	for (int i = 0; i < num_of_lines; i++)
		x[i] = (COLUMNS - s[i].length()) / 2;
	for (int i = ROWS - 1; i >= 0; i--) {
		for (int j = 0; j < num_of_lines; j++) {
			if (i + j < ROWS - 1) {
				set_cursor(x[j], i + j);
				cout << s[j];
			}
		}
		Sleep(200);
		system("cls");
	}

	Sleep(300);

	string title = "MAZE RPG";
	x[0] = (COLUMNS - title.length()) / 2;
	for (int i = ROWS - 1; i >= ROWS / 2; i--) {
		set_cursor(x[0], i);
		cout << title;
		Sleep(200);
		if (i != ROWS / 2) system("cls");
	}

	Sleep(300);
}

bool choose_character() {
	int i = 0, input;
	string tmp;

	while (true) {
		clear_in_main();
		log_in_user->clear_chars();

		vector<string> data;
		data.push_back("SELCHAR");
		data.push_back(log_in_user->get_id());
		conn->read_data(data);

		for (int i = 0; i < conn->str_vector.size(); i++) {
			log_in_user->create_character((*conn->str_vector[i]));
		}

		string *chars = log_in_user->get_chars();
		for (i = 0; i < MAX_CHARS; i++) {
			tmp = to_string(i + 1);
			tmp = "[" + tmp + "] " + chars[i];
			set_cursor((COLUMNS - tmp.length()) / 2, H_ROWS + i + 2);
			cout << tmp;
			Sleep(50);
		}
		tmp = "[4] New";
		set_cursor((COLUMNS - tmp.length()) / 2, H_ROWS + 2 + (++i));
		cout << tmp;

		tmp = "[5] Delete Character";
		set_cursor((COLUMNS - tmp.length()) / 2, H_ROWS + 2 + (++i));
		cout << tmp;

		tmp = "[6] Delete Account";
		set_cursor((COLUMNS - tmp.length()) / 2, H_ROWS + 3 + (++i));
		cout << tmp;

		tmp = "[OTHER] Log out";
		set_cursor((COLUMNS - tmp.length()) / 2, H_ROWS + 3 + (++i));
		cout << tmp;

		string menu_input = ">>   <<";
		set_cursor((COLUMNS - menu_input.length()) / 2, H_ROWS + (++i) + 3);
		cout << UNDERLINE << menu_input;
		cout << DEUNDERLINE;
		input = atoi(inputs(H_COLUMNS, H_ROWS + i + 3).c_str());


		switch (input) {
		case 1:
		case 2:
		case 3:
			if (chars[input - 1] == "NO Character") {
				clear_in_main();
				string s = "Cannot choose this";
				set_cursor((COLUMNS - s.length()) / 2, H_ROWS + 2);
				cout << s;
				enter_to_continue();
			}
			else {
				clear_in_main();
				selected_char = log_in_user->character[input - 1];

				vector<string> data;
				data.push_back("LOADITEM");
				data.push_back(selected_char->name);
				conn->read_data(data);

				for (int i = 0; i < conn->str_vector.size(); i++) {
					for (Item tmp : items) {
						if (atoi(conn->str_vector[i]->at(0).c_str()) == tmp.get_item_no()) {
							selected_char->load_inven(tmp, atoi(conn->str_vector[i]->at(1).c_str()));
							break;
						}
					}
				}

				string s = "Play";
				set_cursor((COLUMNS - s.length()) / 2, H_ROWS + 2);
				cout << s;
				enter_to_continue();
				return true;
			}
			break;
		case 4:
			create_char();
			break;
		case 5:
			delete_char();
			break;
		case 6:
			if (delete_acc())
				return false;
			break;
		default:
			return false;
		}

	}
}

void create_char() {
	int limit = log_in_user->get_created_char();
	string name, s;
	string menus = "NAME :           ";

	if (limit == MAX_CHARS) {
		clear_in_main();
		s = "Cannot create more character";
		set_cursor((COLUMNS - s.length()) / 2, H_ROWS + 2);
		cout << s;
		enter_to_continue();
		return;
	}

	clear_in_main();

	set_cursor((COLUMNS - menus.length()) / 2, H_ROWS + 2);
	cout << menus;
	Sleep(50);

	name = inputs((COLUMNS - menus.length()) / 2 + 7, H_ROWS + 2);

	vector<string> data;
	data.push_back("CRTCHAR");
	data.push_back(log_in_user->get_id());
	data.push_back(name);
	string result = conn->send_data(data);

	clear_in_main();
	set_cursor((COLUMNS - result.length()) / 2, H_ROWS + 2);
	cout << result;
	enter_to_continue();

}

void delete_char() {
	int limit = log_in_user->get_created_char();
	string name, s;
	string menus = "NAME :           ";

	if (limit == 0) {
		clear_in_main();
		s = "No Character exist";
		set_cursor((COLUMNS - s.length()) / 2, H_ROWS + 2);
		cout << s;
		enter_to_continue();
		return;
	}

	clear_in_main();

	string chars = "";

	for (int j = 0; j < limit; j++)
		chars += "[" + to_string(j + 1) + "] " + log_in_user->character[j]->get_name() + "  ";
	set_cursor((COLUMNS - chars.length()) / 2, H_ROWS + 2);
	cout << chars;

	set_cursor((COLUMNS - menus.length()) / 2, H_ROWS + 4);
	cout << menus;
	Sleep(50);

	name = inputs((COLUMNS - menus.length()) / 2 + 7, H_ROWS + 4);

	if (name.length() < 2 && atoi(name.c_str()) != 0) {
		int char_num = atoi(name.c_str());
		name = log_in_user->character[char_num - 1]->get_name();
	}

	vector<string> data;
	data.push_back("DELCHAR");
	data.push_back(name);
	string result = conn->send_data(data);

	clear_in_main();
	set_cursor((COLUMNS - result.length()) / 2, H_ROWS + 2);
	cout << result;
	enter_to_continue();
}

boolean delete_acc() {
	clear_in_main();

	string show1 = "Delete all characters and datas";

	set_cursor((COLUMNS - show1.length()) / 2, H_ROWS + 2);
	cout << show1;

	string show2 = "Are you sure?";

	set_cursor((COLUMNS - show2.length()) / 2, H_ROWS + 3);
	cout << show2;
	
	string menus = "[Y / N]";

	set_cursor((COLUMNS - menus.length()) / 2, H_ROWS + 4);
	cout << menus;
	Sleep(50);

	string input = inputs(COLUMNS / 2, H_ROWS + 5);

	while (true) {
		if (input == "Y" || input == "y") {
			vector<string> data;
			data.push_back("DELACC");
			data.push_back(log_in_user->get_id());
			string result = conn->send_data(data);

			clear_in_main();
			set_cursor((COLUMNS - result.length()) / 2, H_ROWS + 2);
			cout << result;
			enter_to_continue();

			return true;
		}
		
		string result = "Canceled";

		clear_in_main();
		set_cursor((COLUMNS - result.length()) / 2, H_ROWS + 2);
		cout << result;
		enter_to_continue();
		
		return false;
	}
}

string input_in_game() {
	string input;
	string menu_input = ">>       ";
	set_cursor((COLUMNS - menu_input.length()) / 2, 10);
	cout << menu_input.substr(0, 2) << UNDERLINE << menu_input.substr(2);
	cout << DEUNDERLINE << "                 ";

	input = inputs((COLUMNS - menu_input.length()) / 2 + 3, 10);

	return input;
}

void play() {
	string input;
	print_main_UI();
	while (true) {
		input = input_in_game();

		if (input == "/help") {
			print_msg(" ");
			print_msg("/help : View commands");
			print_msg("/exit : Log out");
			print_msg("/save : Save all conditions");
			print_msg("inven : show inventory");
			print_msg("store : Enter item store");
			print_msg("status : Show and Set status");
			print_msg("start : Enter next maze");
			print_msg("##### Commands #####");
			print_msg(" ");
		}
		else if (input == "start") {
			//thread t(t_maze_gen);
			print_msg("Lading maze...");
			t_maze_gen();
			//t.join();
			print_msg("Enter the maze");
			PlaySound("audio/door_open.wav", GetModuleHandle(NULL), SND_FILENAME | SND_SYNC | SND_NODEFAULT); //한번 재생
			start_stage();
		}
		else if (input == "store") {
			item_store();
		}
		else if (input == "status") {
			status();
		}
		else if (input == "inven") {
			show_inven();
		}
		else if (input == "/save") {
			thread t(t_save);
			print_msg("Saving...");
			t.join();
			//print_msg("Done!");
		}
		else if (input == "/exit") {
			print_msg("Logged out in 5 seconds.");
			clear_msgs();
			delete(log_in_user);
			Sleep(1000);
			system("cls");
			return;
		}
		else
			print_msg("'" + input + "' is invaild command");
	}
}

void print_main_UI() {
	system("cls");

	update_ui_maze();

	string menu_input = ">>       ";
	set_cursor((COLUMNS - menu_input.length()) / 2, 10);
	cout << menu_input.substr(0, 2) << UNDERLINE << menu_input.substr(2);
	cout << DEUNDERLINE;

	string msg[] = { "Welcome to the MAZE RPG",
					"Explore the maze",
					"Defeating the enemy",
					"Find the exit",
					" ",
					"You can get help by typing /help." };

	for (int i = 0; i < 6; i++) {
		Sleep(300);
		print_msg(msg[i]);
	}
}

void print_msg(string msg) {
	string clear = "                                                                               ";
	int index = top;
	messages[top] = msg;
	top = top + 1 >= 20 ? 0 : top + 1;
	for (int i = 0; i < 20; i++) {
		set_cursor(0, 11 + i);
		cout << clear;
		set_cursor((COLUMNS - messages[index].length()) / 2, 11 + i);
		cout << messages[index];
		index = index - 1 < 0 ? 19 : index - 1;
	}
}
void clear_msgs() {
	for (int i = 0; i < 20; i++)
		print_msg("");
}

void item_store() {
	save_msgs();
	int input = -1;

	clear_msgs();

	while (true) {
		print_msg("Balance : " + to_string(selected_char->money));
		print_msg("");

		print_msg("[OTHER] EXIT");
		print_msg("");
		for (int i = items.size() - 1; i >= 0; i--) {
			Item tmp = items[i];
			print_msg(tmp.get_explain());
			string info = "[" + to_string(tmp.get_item_no()) + "] " + tmp.get_name() + "    " + to_string(tmp.get_cost()) + " gold";
			print_msg(info);
			print_msg("");
		}

		PlaySound("audio/take_a_look.wav", GetModuleHandle(NULL), SND_FILENAME | SND_ASYNC | SND_NODEFAULT); //한번 재생
		if (rand() % 10 != 0)
			print_msg("Take a look.");
		else
			print_msg("떼껄룩.");
		print_msg("##### store #####");

		input = atoi(input_in_game().c_str());

		clear_msgs();
		if (input >= 1 && input <= items.size()) {
			if (selected_char->money >= items[input - 1].get_cost()) {
				selected_char->add_to_inven(items[input - 1]);
				print_msg("Buy " + items[input - 1].get_name() + " at " + to_string(items[input - 1].get_cost()) + " gold");
				print_msg("");
				selected_char->money -= items[input - 1].get_cost();
			}
			else {
				print_msg("Not enough gold");
				print_msg("");
			}
		}
		else {
			print_msg("Bye");
			print_msg("");
			Sleep(1000);
			restore_msgs();
			break;
		}
	}
}

void show_inven(int state) {
	while (true) {
		if (selected_char->inventory.empty()) {
			print_msg("");
			print_msg("Inventory is empty");
			return;
		}
		clear_msgs();
		print_msg("[OTHER] EXIT");
		print_msg("");
		for (int i = selected_char->inventory.size() - 1; i >= 0; i--) {
			print_msg("[" + to_string(i + 1) + "] " + selected_char->inventory[i].get_name() + " | QTY : " + to_string(selected_char->inventory_cnt[i]));
		}
		print_msg("");
		print_msg("##### Inventory #####");
		print_msg("");

		int input = atoi(input_in_game().c_str());

		if (input >= 1 && input <= selected_char->inventory.size()) {
			use_item(input - 1, state);
		}
		else
			break;
	}
	clear_msgs();
	print_msg("EXIT");
	print_msg("");
}

void use_item(int item_no, int state) {
	Item *use = &selected_char->inventory[item_no];
	int *use_cnt = &selected_char->inventory_cnt[item_no];

	if (state == 0) {
		if (use->get_name().find("HP") == string::npos) {
			print_msg("This item can not be used now");
		}
	}
	else if (remain_turn > 0 && use->get_name().find("HP") == string::npos) {
		print_msg("Can not use item now");
	}
	else {
		print_msg("Use " + use->get_name());
		(*use_cnt)--;

		for (int i = 0; i < items.size(); i++) {
			if (items[i].get_item_no() == use->get_item_no()) {
				using_item = &items[i];
				remain_turn = using_item->get_duration();

				if (using_item->get_name().find("HP") != string::npos)
					d_target = &selected_char->cur_hp;
				else if (using_item->get_name().find("ATK") != string::npos)
					d_target = &selected_char->atk;
				else if (using_item->get_name().find("DEF") != string::npos)
					d_target = &selected_char->def;
				else if (using_item->get_name().find("LUCK") != string::npos)
					i_target = &selected_char->status[2];

				if (d_target != NULL) {
					(*d_target) += using_item->get_effect();
					if (d_target == &selected_char->cur_hp && selected_char->cur_hp > selected_char->max_hp)
						selected_char->cur_hp = selected_char->max_hp;
				}
				else if (i_target != NULL) {
					(*i_target) += using_item->get_effect();
				}

				update_ui_maze();
				break;
			}
		}

		if ((*use_cnt) <= 0) {
			selected_char->inventory.erase(selected_char->inventory.begin() + item_no);
			selected_char->inventory_cnt.erase(selected_char->inventory_cnt.begin() + item_no);
		}
	}
	Sleep(1000);
}

void start_stage() {
	clear_msgs();
	print_msg("Enter The MAZE");

	current = { start.col, start.row };
	string input;
	int c = 0;

	print_maze();


	while (current.col != dest.col || current.row != dest.row) {
		while (_kbhit()) {
			boolean is_act = true;
			if (remain_turn >= 0)
				remain_turn--;

			switch ((c = _getch())) {
			case KEY_UP:
				if (current.row > 0 && (maze[current.row - 1][current.col] & 2) != 0) {
					current.row--;
					print_msg("Go up");
					print_maze();
					PlaySound("audio/footsteps.wav", GetModuleHandle(NULL), SND_FILENAME | SND_ASYNC | SND_NODEFAULT); //한번 재생
					if (c != 0 && !fight_enemy())
						return;
				}
				else {
					print_msg("I can't go that way");
					c = 0;
				}
				break;
			case KEY_DOWN:
				if (current.row < 9 && (maze[current.row][current.col] & 2) != 0) {
					current.row++;
					print_msg("Go down");
					print_maze();
					PlaySound("audio/footsteps.wav", GetModuleHandle(NULL), SND_FILENAME | SND_ASYNC | SND_NODEFAULT); //한번 재생
					if (c != 0 && !fight_enemy())
						return;
				}
				else {
					print_msg("I can't go that way");
					c = 0;
				}
				break;
			case KEY_LEFT:
				if (current.col > 0 && (maze[current.row][current.col - 1] & 1) != 0) {
					current.col--;
					print_msg("Go left");
					print_maze();
					PlaySound("audio/footsteps.wav", GetModuleHandle(NULL), SND_FILENAME | SND_ASYNC | SND_NODEFAULT); //한번 재생
					if (c != 0 && !fight_enemy())
						return;
				}
				else {
					print_msg("I can't go that way");
					c = 0;
				}
				break;
			case KEY_RIGHT:
				if (current.col < 9 && (maze[current.row][current.col] & 1) != 0) {
					current.col++;
					print_msg("Go right");
					print_maze();
					PlaySound("audio/footsteps.wav", GetModuleHandle(NULL), SND_FILENAME | SND_ASYNC | SND_NODEFAULT); //한번 재생
					if (c != 0 && !fight_enemy())
						return;
				}
				else {
					print_msg("I can't go that way");
					c = 0;
				}
				break;
			case KEY_I:
				show_inven();
				is_act = false;
				break;
			case KEY_S:
				status();
				is_act = false;
				break;
			case KEY_SLASH:
				input_com_maze();
				is_act = false;
				c = 0;
				break;
				/*default:
					print_msg("I can't understand");
					break;*/
			}

			if (is_act) {
				if (remain_turn == 0) {
					if (d_target != NULL)
						(*d_target) -= using_item->get_effect();
					if (i_target != NULL)
						(*i_target) -= using_item->get_effect();

					print_msg("The effect of the " + using_item->get_name() + "disappears.");

					using_item = NULL;
					int remain_turn = -1;
					int *d_target = NULL;
					int *i_target = NULL;
				}
			}
			//print_msg(to_string(c));
		}
	}


	string empty = "                              ";

	for (int i = 0; i < 10; i++) {
		set_cursor(H_COLUMNS - 15, i);
		cout << empty;
	}

	selected_char->state_num++;

	thread t(t_save);
	update_ui_maze();
	print_msg("");
	print_msg("!! Stage clear !!");
	print_msg("Escape the maze");
	print_msg("");
	t.join();
}

void input_com_maze() {
	string input = input_in_game();

	if (input == "heal") {
		//selected_char->add_cur_hp(10.5);
		update_ui_maze();
	}
	else if (input == "kick") {
		//selected_char->add_cur_hp(-10.6);
		update_ui_maze();
	}
	else if (input == "help") {
		print_msg(" ");
		print_msg("/help : View commands");
		print_msg("I : show inventory");
		print_msg("S : Show and Set status");
		print_msg("Arrow key : move");
		print_msg("##### Commands #####");
		print_msg(" ");
	}

	string menu_input = ">>       ";
	set_cursor((COLUMNS - menu_input.length()) / 2, 10);
	cout << menu_input.substr(0, 2) << UNDERLINE << menu_input.substr(2);
	cout << DEUNDERLINE << "                 ";
}

void input_com_fight() {
	string input = input_in_game();

	if (input == "help") {
		print_msg(" ");
		print_msg("/help : View commands");
		print_msg("I : show inventory");
		print_msg("R : Run away");
		print_msg("A : Attack");
		print_msg("##### Commands #####");
		print_msg(" ");
	}

	string menu_input = ">>       ";
	set_cursor((COLUMNS - menu_input.length()) / 2, 10);
	cout << menu_input.substr(0, 2) << UNDERLINE << menu_input.substr(2);
	cout << DEUNDERLINE << "                 ";
}

void update_ui_maze() {
	string erase = "          ";

	set_cursor(3, ROWS - 5);
	cout << selected_char->name;

	set_cursor(3, ROWS - 4);
	cout << "Lv. " << selected_char->level;

	set_cursor(3, ROWS - 3);
	cout << "Stage : " << selected_char->state_num;


	set_cursor(15, ROWS - 5);
	cout << "HP";
	set_cursor(15, ROWS - 4);
	cout << erase;
	set_cursor(15, ROWS - 4);
	cout << selected_char->cur_hp << " / " << selected_char->max_hp;

	set_cursor(15, ROWS - 3);
	cout << "EXP";
	set_cursor(15, ROWS - 2);
	cout << erase;
	set_cursor(15, ROWS - 2);
	cout << selected_char->exp << " / " << selected_char->level * 10;
}

void print_maze() {
	string empty = "                              ";

	for (int i = 0; i < 10; i++) {
		set_cursor(H_COLUMNS - 15, i);
		cout << empty;
	}

	set_cursor(H_COLUMNS, 5);
	cout << "<>";
	//up
	if (current.row == 0 || (maze[current.row - 1][current.col] & 2) == 0) {
		set_cursor(H_COLUMNS, 4);
		cout << "■";
	}
	else {
		set_cursor(H_COLUMNS, 4);
		cout << "□";
	}
	//left
	if (current.col == 0 || (maze[current.row][current.col - 1] & 1) == 0) {
		set_cursor(H_COLUMNS - 2, 5);
		cout << "■";
	}
	else {
		set_cursor(H_COLUMNS - 2, 5);
		cout << "□";
	}
	//down
	if (current.row == MAZE_SIZE - 1 || (maze[current.row][current.col] & 2) == 0) {
		set_cursor(H_COLUMNS, 6);
		cout << "■";
	}
	else {
		set_cursor(H_COLUMNS, 6);
		cout << "□";
	}
	//right
	if (current.col == MAZE_SIZE - 1 || (maze[current.row][current.col] & 1) == 0) {
		set_cursor(H_COLUMNS + 2, 5);
		cout << "■";
	}
	else {
		set_cursor(H_COLUMNS + 2, 5);
		cout << "□";
	}
}

bool fight_enemy() {
	save_msgs();

	if (spawn_enemy()) {
		PlaySound("audio/monster.wav", GetModuleHandle(NULL), SND_FILENAME | SND_ASYNC | SND_LOOP); // 반복재생
		print_msg("Enemy has appeared");
		double e_atk = selected_char->state_num * 2.5;
		double e_hp = selected_char->state_num * 10 + 40;
		while (e_hp > 0) {
			srand((unsigned)time(NULL));
			int c = 0;
			boolean is_act = true;

			if (remain_turn >= 0)
				remain_turn--;

			switch ((c = _getch())) {
			case KEY_A:
				if (rand() % 100 < selected_char->get_luck()) {
					e_hp = (e_hp - selected_char->get_atk() * 2) <= 0 ? 0 : (e_hp - selected_char->get_atk() * 2);
					print_msg("Critical Hit! (" + to_string(e_hp) + ")");
					print_msg("");
				}
				else {
					e_hp = (e_hp - selected_char->get_atk()) <= 0 ? 0 : (e_hp - selected_char->get_atk());
					print_msg("Attack! (" + to_string(e_hp) + ")");
					print_msg("");
				}
				PlaySound("audio/attack.wav", GetModuleHandle(NULL), SND_FILENAME | SND_SYNC | SND_NODEFAULT); //한번 재생
				break;
			case KEY_R:
				if (rand() % 100 < selected_char->get_luck()) {
					PlaySound("audio/Breathing_deep.wav", GetModuleHandle(NULL), SND_FILENAME | SND_SYNC | SND_NODEFAULT); //한번 재생		
					restore_msgs();
					print_msg("");
					print_msg("Run away successfully");
					return true;
				}
				else
					print_msg("Fail to run away...");
				break;
			case KEY_I:
				show_inven(1);
				is_act = false;
				break;
			case KEY_SLASH:
				input_com_fight();
				is_act = false;
				continue;
			default:
				continue;
			}
			if (is_act) {
				if (e_hp <= 0) {
					PlaySound(NULL, 0, 0); //반복재생 종료
					PlaySound("audio/monster_dead.wav", GetModuleHandle(NULL), SND_FILENAME | SND_ASYNC | SND_NODEFAULT); //한번 재생

					selected_char->exp += 10 + selected_char->state_num;
					selected_char->money += selected_char->state_num * 20 + rand() % (selected_char->get_luck());

					level_up();
					update_ui_maze();

					restore_msgs();
					print_msg("");
					print_msg("Defeated the enemy");
					return true;
				}

				double damage = selected_char->def - e_atk >= 0 ? -2.0 : selected_char->def - e_atk;

				selected_char->add_cur_hp(damage);
				print_msg("Attacked! (" + to_string(selected_char->get_cur_hp()) + ")");
				print_msg("");
				PlaySound("audio/attacked.wav", GetModuleHandle(NULL), SND_FILENAME | SND_ASYNC | SND_NODEFAULT); //한번 재생
				update_ui_maze();
				Sleep(1000);

				if (selected_char->get_cur_hp() <= 0) {
					PlaySound(NULL, 0, 0); //반복재생 종료
					clear_msgs();
					PlaySound("audio/death.wav", GetModuleHandle(NULL), SND_FILENAME | SND_SYNC | SND_NODEFAULT); //한번 재생
					print_msg("You died");
					selected_char->set_cur_hp(selected_char->get_max_hp());

					string empty = "                              ";

					for (int i = 0; i < 10; i++) {
						set_cursor(H_COLUMNS - 15, i);
						cout << empty;
					}

					print_msg("Return to base...");
					update_ui_maze();
					return false;
				}
			}

			if (remain_turn == 0) {
				if (d_target != NULL)
					(*d_target) -= using_item->get_effect();
				if (i_target != NULL)
					(*i_target) -= using_item->get_effect();

				using_item = NULL;
				int remain_turn = -1;
				int *d_target = NULL;
				int *i_target = NULL;
			}
		}
	}

	return true;
}

bool spawn_enemy() {
	srand((unsigned)time(NULL));
	int percent = 50 - selected_char->get_luck() < 15 ? 15 : 50 - selected_char->get_luck();
	if ((rand() % 100) < percent)
		return true;
	return false;
}

void level_up() {
	if (selected_char->exp >= 10 * selected_char->level) {
		selected_char->exp -= 10 * selected_char->level;
		selected_char->level++;
		selected_char->ap += 5;
	}
}

void status() {
	save_msgs();
	while (true) {
		clear_msgs();
		print_msg("[OTHER] EXIT");
		print_msg("[1] APPLY AP");
		print_msg("");
		print_msg("AP : " + to_string(selected_char->ap));
		print_msg("");
		print_msg("LUCK : " + to_string(selected_char->status[2]));
		print_msg("CON : " + to_string(selected_char->status[1]));
		print_msg("STR : " + to_string(selected_char->status[0]));
		print_msg("");
		print_msg("Current HP : " + to_string(selected_char->cur_hp));
		print_msg("MAX HP : " + to_string(selected_char->max_hp));
		print_msg("DEF : " + to_string(selected_char->def));
		print_msg("ATK : " + to_string(selected_char->atk));
		int input = atoi(input_in_game().c_str());

		if (input == 1) {
			if (selected_char->ap > 0) {
				clear_msgs();
				print_msg("[1] STR  [2] CON  [3] LUCK");
				print_msg("Select the number you want to set AP");

				int num = atoi(input_in_game().c_str());

				switch (num) {
				case 1:
				case 2:
				case 3:
					selected_char->set_AP(num);
					update_ui_maze();
					break;
				default:
					clear_msgs();
					print_msg("");
					print_msg("Worng number");
					Sleep(1000);
					break;
				}
			}
			else {
				clear_msgs();
				print_msg("");
				print_msg("Not enough AP");
				Sleep(1000);
			}
		}
		else {
			break;
		}
	}
	restore_msgs();
}

void save_msgs() {
	backuped_top = top;
	for (int i = 0; i < 20; i++)
		backuped_msgs[i] = messages[i];
}

void restore_msgs() {
	top = backuped_top;
	int index = top;

	for (int i = 0; i < 20; i++) {
		print_msg(backuped_msgs[index]);
		index = index + 1 > 19 ? 0 : index + 1;
	}
}
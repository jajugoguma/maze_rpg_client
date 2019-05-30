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

using namespace std;

typedef struct Maze_pos {
	int col;
	int row;
} Maze_pos;

HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);

const int INIT_MENUS = 4;
const int COLUMNS = 79, ROWS = 39;
const int H_COLUMNS = COLUMNS / 2, H_ROWS = ROWS / 2;
const int MAZE_SIZE = 10;

USER *log_in_user;
CHARACTER *selected_char;
int num_users = 0;
bool cursor_on;
COORD pos;
vector< vector<int> > maze;
Maze_pos start;
Maze_pos dest;
Maze_pos current;

string messages[20] = { " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " " };
int top;

int dwID;

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
//Show credit
void credit();
//Press enter to continue
void enter_to_continue();
//Cursor visiblity Switch, true : visible , false : invisible
void cur_visi_swit(bool flag);
//when a character is selected then true.
bool choose_character();

void create_char();
//Dispose of input error when input wrong type.
void input_error();

void play();

void print_main_UI();

void print_msg(string msg);

void clear_msgs();

void item_store();
//Fixed cursor row (Ignore press enter when input empty string)
void t_fix_cur_row(int x, int y);

void t_maze_gen();

void t_save();

void set_font_size();

void start_stage();

void print_maze();

void input_com_maze();

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
	bgm_on();
	//thread t(t_keystrok_sound);
	//프로그램 강제 종료시 핸들러 설정
	BOOL fSuccess = SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
	if (!fSuccess)
	{
		printf("Could not set control handler");
		return 1;
	}

	cur_visi_swit(false);
	set_font_size();
	system("mode con cols=79 lines=39 | title MAZE RPG");
	conn = new Connection();
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

void t_keystrok_sound() {
	while (TRUE) {
		//rewind(stdin);
		while (!_kbhit);
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
	string menus[INIT_MENUS] = { "[1] LOG IN", "[2] SIGN UP", "[9] CREDIT", "[OTHER] EXIT" };
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
		input = stoi(inputs(H_COLUMNS, H_ROWS + i + 3));

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
			continue;
			break;
		case 9:
			credit();
			continue;
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
			set_cursor((COLUMNS - tmp.length()) / 2, H_ROWS + 2 + i);
			cout << tmp;
			Sleep(50);
		}
		tmp = "[" + to_string(MAX_CHARS + 1) + "] New";
		set_cursor((COLUMNS - tmp.length()) / 2, H_ROWS + 2 + (++i));
		cout << tmp;

		tmp = "[OTHER] Log out";
		set_cursor((COLUMNS - tmp.length()) / 2, H_ROWS + 2 + (++i));
		cout << tmp;

		string menu_input = ">>   <<";
		set_cursor((COLUMNS - menu_input.length()) / 2, H_ROWS + (++i) + 3);
		cout << UNDERLINE << menu_input;
		cout << DEUNDERLINE;
		try {
			input = stoi(inputs(H_COLUMNS, H_ROWS + i + 3));
		}
		catch (invalid_argument&) {
			input = -1;
		}

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
		default:
			return false;
		}

	}
}

void create_char() {
	int i = -1, limit = log_in_user->get_created_char();
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

	while (i != limit) {
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
		if (result == "New character created")
			break;
		continue;
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
			print_msg("start : Enter next maze");
			print_msg("##### Commands #####");
			print_msg(" ");
		}
		else if (input == "start") {
			thread t(t_maze_gen);
			print_msg("Lading maze...");
			t.join();
			print_msg("Enter the maze");
			start_stage();
		}
		else if (input == "store") {
			item_store();
		}
		else if (input == "store") {
			item_store();
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
			Sleep(5000);
			system("cls");
			return;
		}
		else
			print_msg("'" + input + "' is invaild command");
	}
}

void print_main_UI() {
	system("cls");

	auto char_infos = selected_char->get_infos();

	set_cursor(3, ROWS - 5);
	cout << get<0>(char_infos);
	set_cursor(3, ROWS - 4);
	cout << "Lv. " << get<1>(char_infos);
	set_cursor(3, ROWS - 3);
	cout << "Stage : " << selected_char->state_num;
	set_cursor(15, ROWS - 5);
	cout << "HP";
	set_cursor(15, ROWS - 4);
	cout << get<3>(char_infos) << " / " << get<2>(char_infos);

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
		Sleep(500);
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
		messages[i] = "";
}

void item_store() {
	vector<Item> items;
	Item tmp(1, "health", 1, 1, "explain");
	items.push_back(tmp);

	int input = -1;
	while (true) {
		clear_msgs();
		for (int i = items.size() - 1; i >= 0; i--) {
			Item tmp = items[i];
			string info = "[" + to_string(tmp.get_item_no()) + "] " + tmp.get_name() + "  " + to_string(tmp.get_class_no());
			print_msg(info);
		}
		if (rand() % 10 != 0)
			print_msg("Take a look.");
		else
			print_msg("떼껄룩.");
		print_msg("##### store #####");
		print_msg("");

		input = atoi(input_in_game().c_str());

		if (input >= 1 && input <= 10) {
			selected_char->add_to_inven(items[input - 1]);
		}
		else {
			clear_msgs();
			print_msg("Bye.");
			print_msg("");
			break;
		}
	}
}

void start_stage() {
	current = { start.col, start.row };
	string input;
	int c = 0;

	print_maze();

	while (current.col != dest.col || current.row != dest.row) {
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
		case KEY_SLASH:
			input_com_maze();
			c = 0;
			break;
		/*default:
			print_msg("I can't understand");
			break;*/
		}
		
		//print_msg(to_string(c));
	}

	string empty = "                              ";

	for (int i = 0; i < 10; i++) {
		set_cursor(H_COLUMNS - 15, i);
		cout << empty;
	}

	selected_char->state_num++;
	
	thread t(t_save);
	print_msg("");
	print_msg("!! Stage clear !!");
	print_msg("Escape the maze");
	print_msg("");
	t.join();
}

void input_com_maze() {
	string input = input_in_game();

	if (input == "heal") {
		selected_char->add_cur_hp(10.5);
		update_ui_maze();
	}
	else if (input == "kick") {
		selected_char->add_cur_hp(-10.6);
		update_ui_maze();
	}

	string menu_input = ">>       ";
	set_cursor((COLUMNS - menu_input.length()) / 2, 10);
	cout << menu_input.substr(0, 2) << UNDERLINE << menu_input.substr(2);
	cout << DEUNDERLINE << "                 ";
}

void update_ui_maze() {
	auto char_infos = selected_char->get_infos();
	set_cursor(3, ROWS - 5);
	cout << get<0>(char_infos);
	set_cursor(3, ROWS - 4);
	cout << "Lv. " << get<1>(char_infos);
	set_cursor(3, ROWS - 3);
	cout << "Stage : " << selected_char->state_num;
	set_cursor(15, ROWS - 5);
	cout << "HP";
	set_cursor(15, ROWS - 4);
	cout << get<3>(char_infos) << " / " << get<2>(char_infos);
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
	if (spawn_enemy()) {
		PlaySound("audio/monster.wav", GetModuleHandle(NULL), SND_FILENAME | SND_ASYNC | SND_LOOP); // 반복재생
		print_msg("Enemy has appeared");
		int e_atk = 1;
		double e_hp = selected_char->cur_hp;
		while (e_hp > 0) {
			srand((unsigned)time(NULL));
			int c = 0;

			switch ((c = _getch())) {
			case KEY_A:
				if (rand() % 100 < selected_char->get_luck()) {
					e_hp = e_hp - selected_char->get_atk() * 2 <= 0 ? 0 : e_hp - selected_char->get_atk() * 2;
					print_msg("Critical Hit! (" + to_string(e_hp) + ")");
					print_msg("");
				}
				else {
					e_hp = e_hp - selected_char->get_atk() <= 0 ? 0 : e_hp - selected_char->get_atk() * 2;
					print_msg("Attack! (" + to_string(e_hp) + ")");
					print_msg("");
				}
				PlaySound("audio/attack.wav", GetModuleHandle(NULL), SND_FILENAME | SND_SYNC | SND_NODEFAULT); //한번 재생
				break;
			case KEY_R:
				if (rand() % 100 < selected_char->get_luck()) {
					print_msg("Run away successfully");
					return true;
				}
				else
					print_msg("Fail to run away...");
				break;
			case KEY_SLASH:
				input_com_maze();
				continue;
			default:
				continue;
			}
			if (e_hp <= 0) {
				PlaySound(NULL, 0, 0); //반복재생 종료
				PlaySound("audio/monster_dead.wav", GetModuleHandle(NULL), SND_FILENAME | SND_SYNC | SND_NODEFAULT); //한번 재생
				print_msg("Defeated the enemy");
				selected_char->exp += 10;
				level_up();
				update_ui_maze();
				return true;
			}

			selected_char->add_cur_hp(-e_atk);
			print_msg("Attacked! (" + to_string(selected_char->get_cur_hp()) + ")");
			print_msg("");
			PlaySound("audio/attacked.wav", GetModuleHandle(NULL), SND_FILENAME | SND_SYNC | SND_NODEFAULT); //한번 재생
			update_ui_maze();

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
	}
	return true;
}

bool spawn_enemy() {
	srand((unsigned)time(NULL));
	if ((rand() % 100) < 50 - selected_char->get_luck())
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
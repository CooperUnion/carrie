/*
 * console.c++ -- serial console for Carrie
 *
 * Copyright (C) 2023  Catherine Van West <cat@vanwestco.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "console.h"

#include <hardware/watchdog.h>

#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

/* stolen from
 * https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
 * and modified to suit my needs.
 */
static vector<string> split(string s, string delimiter) {
	size_t pos_start = 0;
	size_t pos_end;
	size_t delim_len = delimiter.length();
	string token;
	vector<string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
		// don't bother with empty tokens
		if (pos_end != pos_start) {
			token = s.substr(pos_start, pos_end - pos_start);
			res.push_back(token);
		}
		pos_start = pos_end + delim_len;
	}

	res.push_back (s.substr (pos_start));
	return res;
}

string console::prompt(const std::string& pr) {
	ostringstream command_buf;
	cout << pr;

	bool end = false;
	while (!end) {
		const int next = cin.get();
		const char c = static_cast<char>(next);

		switch (next) {
		case '\n':
		case '\r':
		case EOF:
			end = true;
			cout << '\n';
			break;
		default:
			command_buf << c;
			cout << c;
		}
	}

	return command_buf.str();
}

int exec(const std::string& command_str) {
	auto command = split(command_str, " ");
	const string name = command[0];

	if (name.compare("stop") == 0) {
#ifdef CARRIE_DEBUG
		if (testing_board) {
			cout << "stopping board test...\n";
			testing_board = false;
		}
#endif
	} else if (name.compare("simple-move") == 0) {
		if (command.size() != 4) {
			cerr << "usage: simple-move <dist> <curv> <speed>\n";
		} else {
			float dist = stof(command[1]);
			float curv = stof(command[2]);
			float speed = stof(command[3]);

			cout << "moving... ";
			do_simple_move(dist, curv, speed);
			cout << "done!\n";
		}
	} else if (name.compare("set-gain") == 0) {
		if (command.size() != 3) {
			cerr << "usage: set-gain (p | i | d) <gain>\n";
		} else {
			float gain = stof(command[2]);
			switch (command[1][0]) {
			case 'p':
				set_kp(gain);
				break;
			case 'i':
				set_ki(gain);
				break;
			case 'd':
				set_kd(gain);
				break;
			default:
				cerr << "which gain is '"
				     << command[1][0] << "'?\n";
			}
		}
	} else if (name.compare("gains") == 0) {
		cout << "gains:"
			" kp=" << get_kp()
		     << " ki=" << get_ki()
		     << " kd=" << get_kd()
		     << '\n';
	} else if (name.compare("reset") == 0) {
		watchdog_reboot(0, 0, 0);
#ifdef CARRIE_DEBUG
	} else if (name.compare("board-test") == 0) {
		cout << "starting board test "
			"(type 'stop' to stop)...\n";
		static repeating_timer_t timer;
		testing_board = true;
		add_repeating_timer_ms(
			1000, board_test_callback,
			nullptr, &timer
		);
#endif

	} else if (name.length() > 0) {
		cerr << "aaauaaugh!\n";
	}
}

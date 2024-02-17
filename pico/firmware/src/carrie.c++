/*
 * carrie.c++ -- entry point for Carrie's mind
 *
 * Copyright (C) 2023  Catherine Van West <cat@vanwestco.com>
 * Copyright (C) 2023  Jeannette Circe    <circe@cooper.edu>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

// our libraries
#include "control/low-level.h"
#include "comms/messages.h"
#include "comms/packet.h"

// Pico libraries
#include <hardware/gpio.h>
#include <hardware/pwm.h>
#include <hardware/watchdog.h>
#include <pico/stdlib.h>

// standard library
#include <atomic>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <tuple>
#include <vector>

using namespace std;

/* stolen from
 * https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
 * and modified to suit my needs.
 */
vector<string> split(string s, string delimiter) {
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

float x, y, z;

void serial_console() {
	bool switch_modes = false;
	while (!switch_modes) {
		ostringstream command_buf;
		cout << "carrie:> ";

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

		const string command_str = command_buf.str();
		auto command = split(command_str, " ");
		const string name = command[0];

		if (name.compare("stop") == 0) {
			cout << "stopping everything...\n";
			stop_control_loop();
			stop_board_test();
		} else if (name.compare("simple-move") == 0) {
			if (command.size() != 4) {
				cerr << "usage: simple-move <dist> <curv> <vel>\n";
			} else {
				float dist = stof(command[1]);
				float curv = stof(command[2]);
				float vel = stof(command[3]);

				cout << "moving... \n";
				do_simple_move(dist, curv, vel);
			}

#ifdef CARRIE_DEBUG
		} else if (name.compare("cal-steering") == 0) {
			cout << "calibrating steering...\n";
			cal_steering();
		} else if (name.compare("reset-bias") == 0) {
			cout << "resetting bias...\n";
			reset_bias();
		} else if (name.compare("set-gain") == 0) {
			if (command.size() < 3 || command.size() > 4) {
				cerr << "usage: set-gain (p | i | d) <gain> [-v]\n";
			} else {
				float gain = stof(command[2]);
				bool vel = command.size() > 3; // XXX

				switch (command[1][0]) {
				case 'p':
					if (vel) set_velocity_kp(gain);
					else set_steering_kp(gain);
					break;
				case 'i':
					if (vel) set_velocity_ki(gain);
					else set_steering_ki(gain);
					break;
				case 'd':
					if (vel) set_velocity_kd(gain);
					else set_steering_kd(gain);
					break;
				default:
					cerr << "which gain is '"
					     << command[1][0] << "'?\n";
				}
			}
		} else if (name.compare("gains") == 0) {
			if (command.size() > 1) {
				cout << "gains (vel):"
				        " kp=" << get_velocity_kp()
				     << " ki=" << get_velocity_ki()
				     << " kd=" << get_velocity_kd()
				     << '\n';
			} else {
				cout << "gains:"
				        " kp=" << get_steering_kp()
				     << " ki=" << get_steering_ki()
				     << " kd=" << get_steering_kd()
				     << '\n';
			}
		} else if (name.compare("set-bands") == 0) {
			if (command.size() < 3 || command.size() > 4) {
				cerr << "usage: set-bands <low> <high> [-v]\n";
			} else {
				float low_band = stof(command[1]);
				float high_band = stof(command[2]);
				if (command.size() > 3) {
					set_velocity_deadbands(
						low_band, high_band
					);
				} else {
					set_steering_deadbands(
						low_band, high_band
					);
				}
			}
		} else if (name.compare("bands") == 0) {
			float low_band, high_band;
			cout << "bands: ";
			if (command.size() > 1) {
				std::tie(low_band, high_band) =
					get_velocity_deadbands();
				cout << "(vel) ";
			} else {
				std::tie(low_band, high_band) =
					get_steering_deadbands();
			}
			cout << " low_band="  << low_band
			     << " high_band=" << high_band
			     << '\n';
		} else if (name.compare("reset") == 0) {
			watchdog_reboot(0, 0, 0);

		} else if (name.compare("board-test") == 0) {
			cout << "starting board test "
			        "(type 'stop' to stop)...\n";
			start_board_test();
		} else if (name.compare("where-am-i") == 0) {
			cout << "i think i'm at ("
			     << x << ", " << y << ", " << z
			     << ")?\n";
		} else if (name.compare("dist-vel") == 0) {
			cout << "distance=" << get_distance()
			     << " velocity=" << get_velocity()
			     << '\n';
#endif

		} else if (name.compare("switch-modes") == 0) {
			switch_modes = true;
			cout << "switching modes...\n";
		} else if (name.length() > 0) {
			cerr << "aaauaaugh!\n";
		}
	}
}

void packet_handler() {
	bool switch_modes = false;
	while (!switch_modes) {
		Packet p;
		cin >> p;

		switch (p.id()) {
		case 0:
			switch_modes = true;
			break;

		case Position::id: {
			Position new_pos(p);
			x = new_pos.x_trans;
			y = new_pos.y_trans;
			z = new_pos.z_trans;
			break;
		}

		case Simple_Move::id: {
			Simple_Move move(p);

			// will override the last active move
			do_simple_move(
				move.distance, move.curvature, move.velocity
			);

			Move_Feedback info {
				get_distance(),
				get_curvature(),
				get_velocity(),
				move.trans_id
			};
			cout << info.pack();
			break;
		}

		case Stop::id: {
			stop_control_loop();
			stop_board_test();
		}

		default:
			// nothing (yet, at least)
			break;
		}

		cout << std::flush;
	}
}

int main() {
	stdio_init_all();
	init_controls(0.05f); // 50 ms loop time
	while (true) {
		serial_console();
		packet_handler();
	}
	return 0;
}

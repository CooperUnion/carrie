/*
 * control/low-level.h -- public facing interface for the low-level car
 * controls
 *
 * Copyright (C) 2023  Jeannette Circe    <circe@cooper.edu>
 * Copyright (C) 2023  Catherine Van West <cat@vanwestco.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CONTROL_CONTROL_H
#define CONTROL_CONTROL_H

// TODO: think about whether this should move
#include "control/steering.h"
#include "control/velocity.h"

typedef bool (*loop_callback_t)();

void init_controls(float dt);
void start_control_loop(loop_callback_t callback = nullptr);
void stop_control_loop();

// returns the distance traveled (in metres) since startup
float odometer();

void do_simple_move(float distance, float curvature, float velocity);

void start_board_test();
void stop_board_test();

void cal_steering();
void reset_bias();

float get_distance();
float get_curvature();
float get_velocity();

#endif

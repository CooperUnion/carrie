/*
 * control/velocity.h -- public facing interface for the velocity loop
 *
 * Copyright (C) 2023  Jeannette Circe    <circe@cooper.edu>
 * Copyright (C) 2023  Catherine Van West <cat@vanwestco.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef VELOCITY_CONTROL_H
#define VELOCITY_CONTROL_H

#include <tuple>

void init_velocity_loop(float dt);

// TODO: make a better interface for this module!
float get_velocity_kp();
float get_velocity_ki();
float get_velocity_kd();

void set_velocity_kp(float kp);
void set_velocity_ki(float ki);
void set_velocity_kd(float kd);

void set_velocity_deadbands(float low, float high);
std::tuple<float, float> get_velocity_deadbands();

float velocity_inner_loop(float desired_velocity, float actual_velocity);

#endif

/*
 * steering.h -- interface for the steering control loop
 *
 * Copyright (C) 2023  Jeannette Circe    <circe@cooper.edu>
 * Copyright (C) 2023  Catherine Van West <cat@vanwestco.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CONTROL_STEERING_H
#define CONTROL_STEERING_H

#include <tuple>

void init_steering_loop(float dt);

// TODO: make a better interface for this module!
float get_steering_kp();
float get_steering_ki();
float get_steering_kd();

void set_steering_kp(float kp);
void set_steering_ki(float ki);
void set_steering_kd(float kd);

void set_steering_deadbands(float low, float high);
std::tuple<float, float> get_steering_deadbands();

float counts_to_angle_steer(long counts);
long angle_to_counts_steer(float angle);
float counts_to_angle_drive(long counts);
long angle_to_counts_drive(float angle);

float center_to_right(float center);
float right_to_center(float right);

float curvature_to_steering_angle(float curvature);
float linear_velocity(long left_count, long right_count, float dt);

float get_observer_angle(long left_counts, long right_counts, float dt);
float determine_angle_bias(long left_counts,
                           long right_counts,
                           long steering_pos,
                           float dt);
long get_desired_encoder_pos(float desired_angle, float bias_r);

float steering_inner_loop(long desired_steering_pos, long steering_pos);

#endif

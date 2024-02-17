/*
 * carrie.c++ -- entry point for Carrie's mind
 *
 * Copyright (C) 2023  Catherine Van West <cat@vanwestco.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "calibration/steering-cal.h"
#include "control/steering-control.h"
#include "rcc/actuator/motor.h"
#include "rcc/sensor/odom.h"

#include <hardware/gpio.h>
#include <hardware/pwm.h>
#include <pico/stdlib.h>

#include <cmath>
#include <iostream>

/* pin designations */
namespace pin {
enum {
	// encoder pins
	lenc1 = 2,
	lenc2 = 4,
	renc1 = 3,
	renc2 = 5, // XXX: fix these! arduino artifact lol
	senc1 = 0,
	senc2 = 1,

	// motor pins
	rin1 = 7,
	rin2 = 8,
	rena = 9,
	sin1 = 10,
	sin2 = 11,
	sena = 12,
}; /* ~enum */
}; /* ~namespace pin */

/* motors */
Motor rear_motors(pin::rin1, pin::rin2, pin::rena);
Motor steering_motor(pin::sin1, pin::sin2, pin::sena);

Dir_Odom<pin::lenc1, pin::lenc2> left_enc;
Dir_Odom<pin::renc1, pin::renc2> right_enc;
Dir_Odom<pin::senc1, pin::senc2> steer_enc;

const int loop_time = 100; // milliseconds
const float dt = loop_time/1000.0f; // seconds
const float desired_curvature = 1.0f;
float bias_r = 0;
long loop_counter = 0;
float distance = 0;
float period = 2*2*M_PI/desired_curvature; // periodic in distance!

bool square_wave(float distance, float period) {
	return static_cast<long>(2*distance/period) % 2 == 0;
}

void setup() {
	stdio_init_all();
//	Wire.begin();  // initialize the I2C hardware
	init_steering_loop(dt);

	sleep_ms(5000);
	long midpoint = steering_midpoint(steer_enc, steering_motor);
	steer_enc.setZero(midpoint);

#ifdef DEBUG_PRINTS
	std::cout << "steering midpoint: " << midpoint << '\n';
#endif

	right_enc.setSign(-1); // so they both show forward as forward
	rear_motors(0.65f);
}

void loop() {
	const long left_count = left_enc.getCount();
	const long right_count = right_enc.getCount();
	left_enc.setZero();
	right_enc.setZero();

	const float velocity = linear_velocity(left_count, right_count, dt);
	distance += velocity*dt;

	// this is a figure-8, i swear!
	const bool right_or_left = square_wave(distance, period);
	const float current_curvature =
		(2*right_or_left - 1)*desired_curvature;
//	const float current_curvature = desired_curvature;

	const float desired_angle =
		curvature_to_steering_angle(current_curvature);
	long desired_steering_pos =
		get_desired_encoder_pos(desired_angle, bias_r);

	// inner loop
	const long steering_pos = steer_enc.getCount();
	const float motor_signal = motor_inner_loop(desired_steering_pos,
	                                            steer_enc.getCount())/(255.0f);
	steering_motor(motor_signal);

	// outer loop
	if (++loop_counter % 10 == 0) {
		const float new_bias_r =
			determine_angle_bias(left_count,
			                     right_count,
			                     -steer_enc.getCount(),
			                     dt);
		if (!std::isnan(new_bias_r)) bias_r = new_bias_r; // XXX

#ifdef DEBUG_PRINTS
		std::cout << '\n'
		          << "bias (r): " << bias_r << '\n'
		          << "period: " << period << '\n'
		          << "desired pos: " << desired_steering_pos << '\n'
		          << "steering pos: " << steering_pos << '\n'
		          << "motor signal: " << motor_signal << '\n'
		          << "velocity: " << velocity << '\n'
		          << "distance: " << distance << '\n';
#endif
	}

	sleep_ms(loop_time);
}

int main() {
	setup();
	while (true) loop();
}

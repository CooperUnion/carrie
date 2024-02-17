#include "low-level.h"

#include "calibration/steering-cal.h"
#include "rcc/actuator/motor.h"
#include "rcc/sensor/odom.h"

#include <pico/multicore.h>
#include <pico/time.h>

#include <atomic>
#include <cmath>

#ifdef CARRIE_DEBUG
#include <iostream>
#endif

/* pin designations */
namespace pin {
enum {
	// encoder pins
	lenc1 = 11,
	lenc2 = 10,
	senc1 = 15,
	senc2 = 14,
	renc1 = 9,
	renc2 = 8,

	// motor pins
	rin1 = 21,
	rin2 = 19,
	rena = 20,
	sin1 = 18,
	sin2 = 16,
	sena = 17,

	// IMU pins
	scl = 4,
	sda = 5
}; /* ~enum */
}; /* ~namespace pin */

float dt;
std::atomic<loop_callback_t> loop_callback;

/* motors */
Motor rear_motors(pin::rin1, pin::rin2, pin::rena);
Motor steering_motor(pin::sin1, pin::sin2, pin::sena);

Dir_Odom<pin::lenc1, pin::lenc2> left_enc;
Dir_Odom<pin::renc1, pin::renc2> right_enc;
Dir_Odom<pin::senc1, pin::senc2> steer_enc;

int loop_time; // milliseconds
float bias_r = 0;
std::atomic<float> velocity = 0;
std::atomic<float> curvature = 0;

std::atomic<float> distance = 0;
std::atomic<float> desired_distance = 0.0f;
std::atomic<float> desired_curvature = 0.0f;
std::atomic<float> desired_velocity = 0.0f;

std::atomic<bool> board_test_active = false;
std::atomic<bool> control_loop_active = false;

std::atomic<alarm_pool_t*> pool = nullptr;

void cal_steering() {
	long midpoint = steering_midpoint(steer_enc, steering_motor);
	steer_enc.setZero(midpoint);

#ifdef CARRIE_DEBUG
	std::cout << "steering midpoint: " << midpoint << '\n';
#endif
}

void init_controls(float dt_near) {
	// back-calculate the nearest dt
	loop_time = static_cast<int>(dt_near*1000);
	dt = loop_time/1000.0f;

	init_steering_loop(dt);
	init_velocity_loop(dt);
	right_enc.setSign(-1); // so they both show forward as forward
}

bool simple_move_callback() {
	return distance < desired_distance;
}

void do_simple_move(float _desired_distance,
                    float _desired_curvature,
                    float _desired_velocity) {

	// XXX
	desired_distance = distance + _desired_distance;
	desired_curvature = _desired_curvature;
	desired_velocity = _desired_velocity;
	start_control_loop(&simple_move_callback);
}

bool board_test(repeating_timer_t* out) {
	std::cout << "\nleft encoder: "
	          << "lenc1(" << pin::lenc1 << ")=" << gpio_get(pin::lenc1) << " "
	             "lenc2(" << pin::lenc2 << ")=" << gpio_get(pin::lenc2) << " "
	             "count=" << left_enc.getCount()
	          << "\nright encoder: "
	          << "renc1(" << pin::renc1 << ")=" << gpio_get(pin::renc1) << " "
	             "renc2(" << pin::renc2 << ")=" << gpio_get(pin::renc2) << " "
	             "count=" << right_enc.getCount()
	          << "\nsteering encoder: "
	          << "senc1(" << pin::senc1 << ")=" << gpio_get(pin::senc1) << " "
	             "senc2(" << pin::senc2 << ")=" << gpio_get(pin::senc2) << " "
	             "count=" << steer_enc.getCount()
	          << '\n';

	rear_motors(0.4f*board_test_active);
	steering_motor(0.4f*board_test_active);
	return board_test_active;
}

// 4th-order direct form II elliptic filter, cutoff at .005π
constexpr const static double numerator[] = {
	0.003143942197224566,-0.01255567797981058,0.01882348891176122,-0.01255567797981059,0.003143942197224566
};

constexpr const static double denominator[] = {
	1,-3.98490431671971,5.955081561761521,-3.955447137945511,0.9852699123668941
};

static double state[] = {
	0, 0, 0, 0, 0
};

constexpr const size_t delays = sizeof(state)/sizeof(double) - 1;

void reset_bias() {
#ifdef CARRIE_DEBUG
	std::cout << "bias was: " << bias_r << '\n';
#endif

	bias_r = 0;
	for (size_t i = 0; i < delays + 1; ++i) {
		state[i] = 0;
	}
}

float filter_bias(float bias_sample) {
	// put the next input as the zeroth state
	state[0] = bias_sample;

	// calculate the next input state and next output
	double output = 0;
	for (size_t delay = delays; delay > 0; --delay) {
		// note the signs!
		state[0] -= state[delay]*denominator[delay];
		output += state[delay]*numerator[delay];

		// also, advance each state variable
		state[delay] = state[delay - 1];
	}

	output += numerator[0]*state[0];
	return static_cast<float>(output);
}

long loop_counter = 0;
bool control_loop(repeating_timer_t* out) {
	const long left_count = left_enc.getCount();
	const long right_count = right_enc.getCount();
	left_enc.setZero();
	right_enc.setZero();

	velocity = linear_velocity(left_count, right_count, dt);
	distance = distance + std::fabs(velocity*dt);
	const float velocity_signal =
		velocity_inner_loop(desired_velocity, velocity);
	rear_motors(velocity_signal);

	// XXX: testing hack?
	const float loop_curvature = desired_curvature;
	curvature = loop_curvature;

	const float desired_angle =
		curvature_to_steering_angle(desired_curvature);
	long desired_steering_pos =
		get_desired_encoder_pos(desired_angle, bias_r);

	// inner loop
	const long steering_pos = steer_enc.getCount();
	const float steering_signal = steering_inner_loop(
		desired_steering_pos, steer_enc.getCount()
	);
	steering_motor(steering_signal);

	// outer loop
	const float bias_sample = determine_angle_bias(
		left_count, right_count, -steer_enc.getCount(), dt
	);
//	if (!std::isnan(bias_sample)) bias_r = filter_bias(bias_sample);

#if 0
#ifdef CARRIE_DEBUG
	if (++loop_counter % 10 == 0) {
		std::cout << '\n'
		          << "bias sample: " << bias_sample << '\n'
		          << "bias (r): " << bias_r << '\n'
		          << "desired pos: " << desired_steering_pos << '\n'
		          << "steering pos: " << steering_pos << '\n'
		          << "steering signal: " << steering_signal << '\n'
		          << "velocity signal: " << velocity_signal << '\n'
		          << "velocity: " << velocity << '\n'
		          << "distance: " << distance << '\n';
	}
#endif
#endif

	bool keep_running = control_loop_active && loop_callback();

	if (!keep_running) {
		// shut down everything before we stop
		steering_motor(0.0f);
		rear_motors(0.0f);
		control_loop_active = false;
	}

	return keep_running;
}

float odometer() {
	return distance;
}

bool default_loop_callback() {
	return true; // no other stop condition
}


void second_core_init() {
	static repeating_timer_t timer;

#ifdef CARRIE_DEBUG
	std::cout << "launching core #1!\n";
#endif

	pool = alarm_pool_create_with_unused_hardware_alarm(4);

#ifdef CARRIE_DEBUG
	std::cout <<  "using hardware alarm #"
	          << alarm_pool_hardware_alarm_num(pool)
	          << " on core #" << alarm_pool_core_num(pool)
	          << '\n';
#endif

	alarm_pool_add_repeating_timer_ms(
		pool,
		loop_time, control_loop,
		nullptr, &timer
	);

	while (1) tight_loop_contents();
}

void start_control_loop(loop_callback_t callback) {
	if (callback) loop_callback = callback;
	else loop_callback = &default_loop_callback;

	if (!control_loop_active) {
		control_loop_active = true;
		if (pool) {
			std::cout << "destroying old hardware pool...\n";
			alarm_pool_destroy(pool); // probably should move this
			pool = nullptr;
		}
		multicore_reset_core1();
		multicore_launch_core1(&second_core_init);
	}
}

void stop_control_loop() {
	control_loop_active = false;
}

void start_board_test() {
	static repeating_timer_t timer;

	if (!board_test_active) {
		board_test_active = true;
		add_repeating_timer_ms(
			1000, board_test,
			nullptr, &timer
		);
	}
}

void stop_board_test() {
	board_test_active = false;
}

float get_distance() {
	return distance;
}

float get_velocity() {
	return velocity;
}

float get_curvature() {
	return curvature;
}

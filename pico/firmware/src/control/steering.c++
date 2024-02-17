#include "steering.h"

#include "rcc/util/pid-control.h"
#include <cmath>

/* controller */
static PID_control_config_t steering_config = {
	.kp = 0.015f,
	.ki = 0.001f,
	.kd = 0.0f,
	.lowerLimit = -1.0f,
	.upperLimit = 1.0f,
	.sigma = 0.05f,
	.ts = 0,
	.errorDotEnabled = false,
	.antiWindupEnabled = true
};
static PID_control steering(steering_config);

constexpr const static float wheelbase = 0.22f; // in m
constexpr const static float track_width = 0.225f; // in m
constexpr const static float wheel_radius = 0.04f; // in m

constexpr const static float radians_per_count_steer = 2*M_PI/1500.0f;
constexpr const static float counts_per_radian_steer = 1/radians_per_count_steer;
constexpr const static float radians_per_count_drive = 2*M_PI/100.0f;
constexpr const static float counts_per_radian_drive = 1/radians_per_count_drive;

static float low_band;
static float high_band;

void init_steering_loop(float dt) {
	set_steering_deadbands(-0.2f, 0.2f);
	steering.setTimeParameters(dt, steering_config.sigma);
}

void set_steering_deadbands(float low, float high) {
	low_band = low;
	high_band = high;
	steering.setDeadbands(low, high);
}

std::tuple<float, float> get_steering_deadbands() {
	return std::make_tuple(low_band, high_band);
}

float get_steering_kp() {
	return steering_config.kp;
}

float get_steering_ki() {
	return steering_config.ki;
}

float get_steering_kd() {
	return steering_config.kd;
}

static void update_gains() {
	steering.setGains(steering_config.kp,
	                  steering_config.ki,
	                  steering_config.kd);
}

void set_steering_kp(float kp) {
	steering_config.kp = kp;
	update_gains();
}

void set_steering_ki(float ki) {
	steering_config.ki = ki;
	update_gains();
}

void set_steering_kd(float kd) {
	steering_config.kd = kd;
	update_gains();
}

// converts encoder reading to radians for steering encoder
float counts_to_angle_steer(long counts) {
	return counts*radians_per_count_steer;
}

// converts radians to encoder reading steering encoder
long angle_to_counts_steer(float angle) {
	return static_cast<int>(angle*counts_per_radian_steer);
}

// the same, but for drive wheel angles
float counts_to_angle_drive(long counts) {
	return counts*radians_per_count_drive;
}

long angle_to_counts_drive(float angle) {
	return static_cast<int>(angle*counts_per_radian_drive);
}

// converts bicycle model angle to right wheel angle
float center_to_right(float center) {
	return atan((center*wheelbase)/(wheelbase - .5*track_width));
}

// converts right wheel angle to bicycle model angle
float right_to_center(float right) {
	const float sin_r = sin(right);
	const float cos_r = cos(right);
	return sin_r*wheelbase/(cos_r*wheelbase + sin_r*.5*track_width);
}

float curvature_to_steering_angle(float curvature) {
	return atan(wheelbase*curvature);
}

float linear_velocity(long left_count, long right_count, float dt) {
	return wheel_radius*counts_to_angle_drive(left_count + right_count)/2/dt;
}

// encoder zero set to midpoint from calibration routine
float get_observer_angle(long left_count, long right_count, float dt) {
	const float v_left =
		wheel_radius * counts_to_angle_drive((float)left_count)/dt;
	const float v_right =
		wheel_radius * counts_to_angle_drive((float)right_count)/dt;
	const float curvature =
		(v_right - v_left) / (0.5*track_width*(v_right + v_left));

	const float angle_observer = curvature_to_steering_angle(curvature);

	return angle_observer;
};

// figure out the bias in the steering encoder from observed wheel speeds
float determine_angle_bias(long left_count,
                           long right_count,
                           long steering_pos,
                           float dt) {
	// observer is based on bicycle model (center wheel)
	const float angle_observer_r = center_to_right(
		get_observer_angle(left_count, right_count, dt)
	);

	// this gives angle for the right wheel based on current encoder count
	// when called
	const float angle_encoder_r = counts_to_angle_steer(steering_pos);

	// difference of where we think 0 is and where going straight actually
	// is this bias in terms of the right wheel
	const float angle_bias_r = angle_observer_r - angle_encoder_r;

	return angle_bias_r;
}

long get_desired_encoder_pos(float desired_angle, float bias_r) {
	return angle_to_counts_steer(center_to_right(desired_angle) + bias_r);
}

float steering_inner_loop(long desired_steering_pos, long steering_pos) {
	return steering.pid(desired_steering_pos, steering_pos);
}

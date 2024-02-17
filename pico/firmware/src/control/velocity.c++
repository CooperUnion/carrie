#include "control/velocity.h"

#include "control/steering.h"
#include "rcc/util/pid-control.h"

#include <cmath>

/* controller */
static PID_control_config_t velocity_config = {
	.kp = 0.3f,
	.ki = 2.0f, // yes, really!
	.kd = 0.0f,
	.lowerLimit = -1.0f,
	.upperLimit = 1.0f,
	.sigma = 0.05f,
	.ts = 0,
	.errorDotEnabled = false,
	.antiWindupEnabled = true
};
static PID_control velocity(velocity_config);

static float low_band;
static float high_band;

void init_velocity_loop(float dt) {
	set_velocity_deadbands(-0.2f, 0.2f);
	velocity.setTimeParameters(dt, velocity_config.sigma);
}

void set_velocity_deadbands(float low, float high) {
	low_band = low;
	high_band = high;
	velocity.setDeadbands(low, high);
}

std::tuple<float, float> get_velocity_deadbands() {
	return std::make_tuple(low_band, high_band);
}

float get_velocity_kp() {
	return velocity_config.kp;
}

float get_velocity_ki() {
	return velocity_config.ki;
}

float get_velocity_kd() {
	return velocity_config.kd;
}

static void update_gains() {
	velocity.setGains(velocity_config.kp,
	               velocity_config.ki,
	               velocity_config.kd);
}

void set_velocity_kp(float kp) {
	velocity_config.kp = kp;
	update_gains();
}

void set_velocity_ki(float ki) {
	velocity_config.ki = ki;
	update_gains();
}

void set_velocity_kd(float kd) {
	velocity_config.kd = kd;
	update_gains();
}

float velocity_inner_loop(float desired_velocity, float actual_velocity) {
	return velocity.pid(desired_velocity, actual_velocity);
}

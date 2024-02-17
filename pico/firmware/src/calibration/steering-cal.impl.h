#include <pico/stdlib.h>

#ifdef CARRIE_DEBUG
#include <iostream>
#endif

/* finds one bound of the steering -- actuates until we stop moving */
template<int pin_1, int pin_2>
static inline long get_bound(Dir_Odom<pin_1, pin_2>& steer_enc,
                             Motor& steering_motor,
                             int motor_ctl) {
	auto last_count = steer_enc.getCount();
	bool moving = true;

	steering_motor(motor_ctl);
	while (moving) {
		sleep_ms(200);
		auto current_count = steer_enc.getCount();
		moving = current_count != last_count;
		last_count = current_count;
	}
	steering_motor(0);

	return last_count;
}

template<int pin_1, int pin_2>
long steering_midpoint(Dir_Odom<pin_1, pin_2>& steer_enc,
                       Motor& steering_motor) {
	const int motor_speed = 120;

	const auto first_bound =
		get_bound(steer_enc, steering_motor, motor_speed);
	const auto second_bound =
		get_bound(steer_enc, steering_motor, -motor_speed);

#ifdef CARRIE_DEBUG
	std::cout << "first bound: " << first_bound << '\n'
	          << "second bound: " << second_bound << '\n';
#endif

	return (first_bound + second_bound) / 2;
}

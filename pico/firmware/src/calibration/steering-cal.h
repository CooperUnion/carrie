/*
 * steering-cal.h -- steering calibration interface
 *
 * Copyright (C) 2023  Catherine Van West <cat@vanwestco.com>
 * Copyright (C) 2023  Jeannette Circe    <circe@cooper.edu>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef STEERING_CAL_H
#define STEERING_CAL_H

#include "rcc/actuator/motor.h"
#include "rcc/sensor/odom.h"

/* finds the center point of the steering by walking the steering to both
 * extremes, taking encoder readings, and finding the center; returns the
 * encoder reading at the hypothetical center. */
template<int pin_1, int pin_2>
long steering_midpoint(Dir_Odom<pin_1, pin_2>& steer_enc,
                       Motor& steering_motor);

#include "steering-cal.impl.h"

#endif

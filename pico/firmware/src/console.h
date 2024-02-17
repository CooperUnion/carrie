/*
 * console.h -- serial console handler for Carrie
 *
 * note: *will* write to cin/cout/cerr!
 * Copyright (C) 2023  Catherine Van West <cat@vanwestco.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CONSOLE_H
#define CONSOLE_H

#include <string>

namespace console {

std::string prompt(const std::string& pr = "carrie:> ");
int exec(const std::string&);

};

#endif

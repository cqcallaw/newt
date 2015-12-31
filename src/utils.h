/*
 Copyright (C) 2015 The newt Authors.

 This file is part of newt.

 newt is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 newt is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with newt.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <string>
#include <defaults.h>

using namespace std;

const_shared_ptr<string> AsString(const bool& value);

const_shared_ptr<string> AsString(const int& value);

const_shared_ptr<string> AsString(const double& value);

const_shared_ptr<string> AsString(bool* value);

const_shared_ptr<string> AsString(int* value);

const_shared_ptr<string> AsString(double* value);

bool EndsWith(std::string const &fullString, std::string const &ending);
#endif /* UTILS_H_ */

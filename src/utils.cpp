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

#include "utils.h"
#include <sstream>
#include <iomanip>

using namespace std;

const_shared_ptr<string> AsString(const bool& value) {
	ostringstream buffer;
	buffer << value;
	const_shared_ptr<string> converted = make_shared<string>(buffer.str());
	return converted;
}

const_shared_ptr<string> AsString(const std::uint8_t& value) {
	ostringstream buffer;
	buffer << "0x" << std::setfill('0') << std::setw(2) << std::hex << uppercase
			<< unsigned(value);
	const_shared_ptr<string> converted = make_shared<string>(buffer.str());
	return converted;
}

const_shared_ptr<string> AsString(const int& value) {
	ostringstream buffer;
	buffer << value;
	const_shared_ptr<string> converted = make_shared<string>(buffer.str());
	return converted;
}

const_shared_ptr<string> AsString(const double& value) {
	ostringstream buffer;
	buffer << value;
	const_shared_ptr<string> converted = make_shared<string>(buffer.str());
	return converted;
}

const_shared_ptr<string> AsString(bool* value) {
	return AsString(*value);
}

const_shared_ptr<string> AsString(int* value) {
	return AsString(*value);
}

const_shared_ptr<string> AsString(double* value) {
	return AsString(*value);
}

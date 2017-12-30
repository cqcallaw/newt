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

const_shared_ptr<string_list> Tokenize(const string& str,
		const string& delimiters) {
	// ref: https://stackoverflow.com/a/236137/577298
	auto tokens = make_shared<string_list>();
	// Skip delimiters at beginning.
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	string::size_type pos = str.find_first_of(delimiters, lastPos);

	while (string::npos != pos || string::npos != lastPos) {
		// Found a token, add it to the vector.
		tokens->push_back(
				make_shared<string>(str.substr(lastPos, pos - lastPos)));
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}

	return tokens;
}

const_shared_ptr<string_list> Unique(const_shared_ptr<string_list> list) {
	auto out = make_shared<string_list>();
	for (auto & value : *list) {
		auto found = false;
		for (auto & output_value : *out) {
			if (*value == *output_value) {
				found = true;
			}
		}

		if (!found) {
			out->push_back(value);
		}
	}

	return out;
}

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

using namespace std;

string* AsString(bool value) {
	ostringstream buffer;
	buffer << value;
	string* converted = new string(buffer.str());
	return converted;
}

string* AsString(int value) {
	ostringstream buffer;
	buffer << value;
	string* converted = new string(buffer.str());
	return converted;
}

string* AsString(double value) {
	ostringstream buffer;
	buffer << value;
	string* converted = new string(buffer.str());
	return converted;
}

string* AsString(bool* value) {
	return AsString(*value);
}

string* AsString(int* value) {
	return AsString(*value);
}

string* AsString(double* value) {
	return AsString(*value);
}

bool EndsWith(std::string const &fullString, std::string const &ending) {
	//ref: http://stackoverflow.com/a/874160/577298
	if (fullString.length() >= ending.length()) {
		return (0
				== fullString.compare(fullString.length() - ending.length(),
						ending.length(), ending));
	} else {
		return false;
	}
}

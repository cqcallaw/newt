/*
 * utils.cpp
 *
 *  Created on: Feb 20, 2015
 *      Author: caleb
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

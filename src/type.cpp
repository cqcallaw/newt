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

#include "type.h"

#include "assert.h"
#include <sstream>
#include <defaults.h>

using namespace std;

const string type_to_string(const BasicType type) {
	switch (type) {
	case NONE:
		return "NONE";
	case BOOLEAN:
		return "boolean";
	case INT:
		return "int";
	case DOUBLE:
		return "double";
	case STRING:
		return "string";
	}

	cout << "Unknown type: " << (int) type << "\n";
	assert(false);

	return "error";  // this keeps compiler happy
}

ostream &operator<<(ostream &os, const BasicType &type) {
	os << type_to_string(type);
	return os;
}

string operator_to_string(OperatorType op) {
	switch (op) {
	case PLUS:
		return "+";
	case MINUS:
		return "-";
	case DIVIDE:
		return "/";
	case MULTIPLY:
		return "*";
	case MOD:
		return "%";
	case UNARY_MINUS:
		return "-";
	case NOT:
		return "!";
	case AND:
		return "&&";
	case OR:
		return "||";
	case EQUAL:
		return "==";
	case NOT_EQUAL:
		return "!=";
	case LESS_THAN:
		return "<";
	case LESS_THAN_EQUAL:
		return "<=";
	case GREATER_THAN:
		return ">";
	case GREATER_THAN_EQUAL:
		return ">=";
	default:
		assert(false);
	}
	assert(false);
	return ""; // to prevent a compilation warning
}

const BasicType FromString(const string& str) {
	if (str.compare("bool") == 0) {
		return BasicType::BOOLEAN;
	} else if (str.compare("int") == 0) {
		return BasicType::INT;
	} else if (str.compare("double") == 0) {
		return BasicType::DOUBLE;
	} else if (str.compare("string") == 0) {
		return BasicType::STRING;
	} else {
		return BasicType::NONE;
	}
}

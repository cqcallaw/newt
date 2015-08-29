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
using namespace std;

const string type_to_string(const PrimitiveType type) {
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
	case INT_ARRAY:
		return "int array";
	case DOUBLE_ARRAY:
		return "double array";
	case STRING_ARRAY:
		return "string array";
	}

	cout << "Unknown type: " << (int) type << "\n";
	assert(false);

	return "error";  // this keeps compiler happy
}

ostream &operator<<(ostream &os, const PrimitiveType &type) {
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

const CompoundType* CompoundType::DefaultCompoundType = new CompoundType();

const string CompoundType::ToString() const {
	ostringstream os;
	CompoundType::const_iterator iter;
	for (iter = this->begin(); iter != this->end(); ++iter) {
		os << iter->first << ": ";
		const PrimitiveType type = iter->second;
		os << type_to_string(type) << endl;
	}
	return os.str();
}

std::ostream& operator <<(std::ostream& os, const CompoundType& type) {
	os << type.ToString();
	return os;
}

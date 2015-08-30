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
	case STRUCT:
		return "struct";
	}

	cout << "Unknown type: " << (int) type << "\n";
	assert(false);

	return "error";  // this keeps compiler happy
}

string AsString(const PrimitiveType type, const void* value) {
	ostringstream buffer;
	switch (type) {
	case BOOLEAN:
	case INT: {
		const int* default_value = (int*) value;
		buffer << *default_value;
		break;
	}
	case DOUBLE: {
		const double* default_value = (double*) value;
		buffer << *default_value;
		break;
	}
	case STRING: {
		const string* default_value = (string*) value;
		buffer << *default_value;
		break;
	}
		//TODO: array types
	default:
		assert(false);
	}

	return buffer.str();
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
		const MemberDefinition* type_information = iter->second;
		const PrimitiveType type = type_information->GetType();
		const void* value = type_information->GetValue();
		os << type_to_string(type) << endl;
		os << AsString(type, value) << endl;
	}
	return os.str();
}

const void* DefaultTypeValue(const PrimitiveType type) {
	switch (type) {
	case BOOLEAN:
		return DefaultBooleanValue;
	case INT:
		return DefaultIntValue;
	case DOUBLE:
		return DefaultDoubleValue;
	case STRING:
		return DefaultStringValue;
	default:
		//TODO: other types
		assert(false);
		return nullptr;
	}
}

std::ostream& operator <<(std::ostream& os, const CompoundType& type) {
	os << type.ToString();
	return os;
}

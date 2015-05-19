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
using namespace std;

string type_to_string(Type type) {
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

ostream &operator<<(ostream &os, const Type &type) {
	os << type_to_string(type);
	return os;
}

string status_to_string(Status status) {
	switch (status) {
	case OK:
		return "OK";
	case MEMBER_NOT_OF_GIVEN_TYPE:
		return "MEMBER_NOT_OF_GIVEN_TYPE";
	case MEMBER_NOT_DECLARED:
		return "MEMBER_NOT_DECLARED";
	default:
		assert(false);
	}
	assert(false);
	return ""; // to prevent a compilation warning
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
	case NEAR:
		return "near";
	case TOUCHES:
		return "touches";
	case SIN:
		return "sin";
	case COS:
		return "cos";
	case TAN:
		return "tan";
	case ASIN:
		return "asin";
	case ACOS:
		return "acos";
	case ATAN:
		return "atan";
	case SQRT:
		return "sqrt";
	case ABS:
		return "abs";
	case FLOOR:
		return "floor";
	case RANDOM:
		return "random";
	default:
		assert(false);
	}
	assert(false);
	return ""; // to prevent a compilation warning
}

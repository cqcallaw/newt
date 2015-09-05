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

string AsString(const BasicType type, const void* value) {
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

const bool ValidateBasicTypeAssignment(const BasicType left_type,
		const BasicType right_type) {
	if ((left_type <= STRING && right_type > left_type)
			|| left_type != right_type) {
		return false;
	} else {
		return true;
	}
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

const CompoundType* CompoundType::DefaultCompoundType = new CompoundType(
		new map<const string, const MemberDefinition*>());

CompoundType::CompoundType(
		const map<const string, const MemberDefinition*>* definition) :
		m_definition(definition) {
}

const MemberDefinition* CompoundType::GetMember(const string name) const {
	return m_definition->at(name);
}

const string CompoundType::ToString() const {
	ostringstream os;
	map<const string, const MemberDefinition*>::const_iterator type_iter;
	for (type_iter = m_definition->begin(); type_iter != m_definition->end();
			++type_iter) {
		const string member_name = type_iter->first;
		const MemberDefinition* member_definition = type_iter->second;
		const BasicType member_type = member_definition->GetType();
		const void* member_default_value = member_definition->GetDefaultValue();

		os << "\t" << member_type << " " << member_name << " ("
				<< AsString(member_type, member_default_value) << ")" << endl;
	}
	return os.str();
}

std::ostream& operator <<(std::ostream& os, const CompoundType& type) {
	os << type.ToString();
	return os;
}

const void* DefaultTypeValue(const BasicType type) {
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


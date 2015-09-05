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

#ifndef TYPE_H
#define TYPE_H

#include <iostream>
#include <string>
#include <linked_list.h>

using namespace std;

enum BasicType {
	NONE = 0,
	BOOLEAN = 1,
	INT = 2,
	DOUBLE = 4,
	STRING = 8,
	DOUBLE_ARRAY = 16,
	INT_ARRAY = 32,
	STRING_ARRAY = 64,
	STRUCT = 128
};

const string type_to_string(const BasicType type);

string AsString(const BasicType type, const void* value);

const void* DefaultTypeValue(const BasicType type);

const bool ValidateBasicTypeAssignment(const BasicType left_type,
		const BasicType right_type);

ostream &operator<<(ostream &os, const BasicType &type);

enum OperatorType {
	PLUS,
	MINUS,
	MULTIPLY,
	DIVIDE,
	MOD,
	UNARY_MINUS,
	NOT,
	AND,
	OR,
	EQUAL,
	NOT_EQUAL,
	LESS_THAN,
	LESS_THAN_EQUAL,
	GREATER_THAN,
	GREATER_THAN_EQUAL
};

string operator_to_string(OperatorType op);

/*class MemberDefinition {
public:
	MemberDefinition(const BasicType type, const void* value) :
			m_type(type), m_value(value) {
	}
	const BasicType GetType() const {
		return m_type;
	}
	const void* GetDefaultValue() const {
		return m_value;
	}
private:
	const BasicType m_type;
	const void* m_value;
};*/

/*class CompoundType {
public:
	CompoundType(const map<const string, const MemberDefinition*>* definition);
	const MemberDefinition* GetMember(const string name) const;

	const string ToString() const;

	const map<const string, const MemberDefinition*>* GetDefinition() const {
		return m_definition;
	}

	const static CompoundType* DefaultCompoundType;

private:
	const map<const string, const MemberDefinition*>* m_definition;
};

std::ostream &operator<<(std::ostream &os, const CompoundType &symbol);*/

#endif // #ifndef TYPE_H

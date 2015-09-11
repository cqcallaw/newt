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

#ifndef PRIMITIVE_TYPE_SPECIFIER_H_
#define PRIMITIVE_TYPE_SPECIFIER_H_

#include <type_specifier.h>
#include <assert.h>
#include <sstream>
#include <defaults.h>
#include <type.h>

class PrimitiveTypeSpecifier: public TypeSpecifier {
public:
	virtual ~PrimitiveTypeSpecifier() {
	}

	virtual const string ToString() const {
		return type_to_string(m_basic_type);
	}

	const string ToString(const void* value) const {
		ostringstream buffer;
		const BasicType type = GetBasicType();
		buffer << ToString() << ": ";
		switch (type) {
		case BasicType::BOOLEAN:
		case BasicType::INT: {
			const int* default_value = (int*) value;
			buffer << *default_value;
			break;
		}
		case BasicType::DOUBLE: {
			const double* default_value = (double*) value;
			buffer << *default_value;
			break;
		}
		case BasicType::STRING: {
			const string* default_value = (string*) value;
			buffer << *default_value;
			break;
		}
		default:
			assert(false);
		}

		return buffer.str();
	}

	virtual const bool IsAssignableTo(const TypeSpecifier* other) const {
		const PrimitiveTypeSpecifier* as_primitive =
				dynamic_cast<const PrimitiveTypeSpecifier*>(other);
		if (as_primitive != nullptr) {
			return m_basic_type <= as_primitive->GetBasicType();
		}
		return false;
	}

	virtual const void* DefaultValue(const TypeTable* type_table) const {
		const BasicType basic_type = GetBasicType();

		switch (basic_type) {
		case BasicType::BOOLEAN:
			return DefaultBooleanValue;
		case BasicType::INT:
			return DefaultIntValue;
		case BasicType::DOUBLE:
			return DefaultDoubleValue;
		case BasicType::STRING:
			return DefaultStringValue;
		default:
			assert(false);
			return nullptr;
		}
	}

	const BasicType GetBasicType() const {
		return m_basic_type;
	}

private:
	PrimitiveTypeSpecifier(const BasicType basic_type) :
			m_basic_type(basic_type) {
	}

	const BasicType m_basic_type;

public:
	const static PrimitiveTypeSpecifier* NONE;
	const static PrimitiveTypeSpecifier* BOOLEAN;
	const static PrimitiveTypeSpecifier* INT;
	const static PrimitiveTypeSpecifier* DOUBLE;
	const static PrimitiveTypeSpecifier* STRING;
	const static PrimitiveTypeSpecifier* ARRAY;
	const static PrimitiveTypeSpecifier* COMPOUND;
};

#endif /* PRIMITIVE_TYPE_SPECIFIER_H_ */

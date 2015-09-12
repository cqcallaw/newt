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

#ifndef MEMBER_DEFINITION_H_
#define MEMBER_DEFINITION_H_

#include <type.h>
#include <type_specifier.h>
#include <primitive_type_specifier.h>
#include <compound_type_specifier.h>
#include <compound_type.h>

class MemberDefinition {
public:
	MemberDefinition(const TypeSpecifier* type, const void* value) :
			m_type(type), m_value(value) {
	}
	virtual ~MemberDefinition() {
	}

	const TypeSpecifier* GetType() const {
		return m_type;
	}

	const void* GetDefaultValue() const {
		return m_value;
	}

	const string ToString(const TypeTable* type_table,
			const Indent indent) const {
		ostringstream buffer;
		buffer << indent;
		const PrimitiveTypeSpecifier* as_primitive =
				dynamic_cast<const PrimitiveTypeSpecifier*>(m_type);
		if (as_primitive != nullptr) {
			buffer << as_primitive->ToString(m_value);
		}

		const ArrayTypeSpecifier* as_array =
				dynamic_cast<const ArrayTypeSpecifier*>(m_type);
		if (as_array != nullptr) {
			buffer << as_array->ToString();
		}

		const CompoundTypeSpecifier* as_compound =
				dynamic_cast<const CompoundTypeSpecifier*>(m_type);
		if (as_compound != nullptr) {
			const string type_name = as_compound->GetTypeName();
			buffer << type_name << ": ";
			const CompoundType* compound_type = type_table->GetType(type_name);
			buffer << compound_type->ToString(type_table, indent + 1);
		}

		return buffer.str();
	}

private:
	const TypeSpecifier* m_type;
	const void* m_value;
};

#endif /* MEMBER_DEFINITION_H_ */

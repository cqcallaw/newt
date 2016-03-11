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
#include <record_type.h>
#include <primitive_type_specifier.h>
#include <record_type_specifier.h>
#include <specifiers/type_specifier.h>
#include <record.h>

class MemberDefinition {
public:
	MemberDefinition(const_shared_ptr<TypeSpecifier> type,
			const_shared_ptr<void> value) :
			m_type(type), m_value(value) {
		assert(m_value);
	}
	virtual ~MemberDefinition() {
	}

	const_shared_ptr<TypeSpecifier> GetType() const {
		return m_type;
	}

	const_shared_ptr<void> GetDefaultValue() const {
		return m_value;
	}

	const std::string ToString(const TypeTable& type_table,
			const Indent& indent) const {
		ostringstream buffer;
		const_shared_ptr<PrimitiveTypeSpecifier> as_primitive =
				std::dynamic_pointer_cast<const PrimitiveTypeSpecifier>(m_type);
		if (as_primitive) {
			buffer << as_primitive->ToString(m_value);
		}

		const_shared_ptr<ArrayTypeSpecifier> as_array =
				std::dynamic_pointer_cast<const ArrayTypeSpecifier>(m_type);
		if (as_array) {
			auto array = static_pointer_cast<const Array>(m_value);
			if (array->GetSize() > 0) {
				buffer << endl;
				buffer << array->ToString(type_table, indent);
				buffer << endl;
				buffer << indent;
			}
		}

		const_shared_ptr<RecordTypeSpecifier> as_record =
				std::dynamic_pointer_cast<const RecordTypeSpecifier>(m_type);
		if (as_record) {
			buffer << endl;
			auto instance = static_pointer_cast<const Record>(m_value);
			buffer << instance->ToString(type_table, indent + 1);
			buffer << indent;
		}

		std::string result = buffer.str();
		return result;
	}

	static const_shared_ptr<MemberDefinition> GetDefaultMemberDefinition();

private:
	const_shared_ptr<TypeSpecifier> m_type;
	const_shared_ptr<void> m_value;
};

#endif /* MEMBER_DEFINITION_H_ */

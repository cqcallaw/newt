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

#include <complex_instantiation_statement.h>
#include <typeinfo>
#include <expression.h>
#include <record_type.h>
#include <record.h>
#include <record_type_specifier.h>
#include <sum_type_specifier.h>

bool RecordTypeSpecifier::operator ==(const TypeSpecifier& other) const {
	try {
		const RecordTypeSpecifier& as_record =
				dynamic_cast<const RecordTypeSpecifier&>(other);
		return CompareContainers(as_record)
				&& *GetTypeName() == *as_record.GetTypeName();
	} catch (std::bad_cast& e) {
		return false;
	}
}

const bool RecordTypeSpecifier::IsAssignableTo(
		const_shared_ptr<TypeSpecifier> other,
		const TypeTable& type_table) const {
	const_shared_ptr<RecordTypeSpecifier> as_record = std::dynamic_pointer_cast<
			const RecordTypeSpecifier>(other);
	if (as_record) {
		try {
			auto resolved_specifier =
					dynamic_cast<const RecordTypeSpecifier&>(ComplexTypeSpecifier::ResolveAliasing(
							*this, type_table));
			auto resolved_other =
					dynamic_cast<const RecordTypeSpecifier&>(ComplexTypeSpecifier::ResolveAliasing(
							*as_record, type_table));

			auto container = resolved_specifier.GetContainer();
			auto other_container = resolved_other.GetContainer();
			if (container) {
				if (other_container) {
					return *container == *other_container
							&& resolved_other.GetTypeName()->compare(
									*resolved_specifier.GetTypeName()) == 0;
				}
			} else {
				if (resolved_other.GetTypeName()->compare(
						*resolved_specifier.GetTypeName()) == 0) {
					return true;
				}
			}
		} catch (std::bad_cast& e) {
		}
	}

	return false;
}

const_shared_ptr<TypeDefinition> RecordTypeSpecifier::GetType(
		const TypeTable& type_table, AliasResolution resolution) const {
	plain_shared_ptr<RecordType> type = nullptr;

	auto container_type = GetContainerType(type_table);
	if (container_type) {
		type = container_type->GetDefinition()->GetType<RecordType>(
				m_type_name);
	} else {
		type = type_table.GetType<RecordType>(m_type_name);
	}

	return type;
}

const std::string RecordTypeSpecifier::ToString() const {
	return (GetContainer() ? GetContainer()->ToString() + "." : "")
			+ *m_type_name;
}

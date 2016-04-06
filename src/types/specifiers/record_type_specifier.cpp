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
		return *GetTypeName() == *as_record.GetTypeName();
	} catch (std::bad_cast& e) {
		return false;
	}
}

const bool RecordTypeSpecifier::IsAssignableTo(
		const_shared_ptr<TypeSpecifier> other,
		const TypeTable& type_table) const {
	const_shared_ptr<RecordTypeSpecifier> as_record = std::dynamic_pointer_cast<
			const RecordTypeSpecifier>(other);
	if (as_record && as_record->GetTypeName()->compare(*m_type_name) == 0) {
		return true;
	}

	auto unaliased = other->ResolveAliasing(type_table);
	if (unaliased) {
		return IsAssignableTo(unaliased, type_table);
	}

	return false;
}

const_shared_ptr<TypeDefinition> RecordTypeSpecifier::GetType(
		const TypeTable& type_table, AliasResolution resolution) const {
	return type_table.GetType<ConcreteType>(GetTypeName(), resolution);
}

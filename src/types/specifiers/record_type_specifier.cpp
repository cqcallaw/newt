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

const_shared_ptr<DeclarationStatement> RecordTypeSpecifier::GetDeclarationStatement(
		const yy::location position, const_shared_ptr<TypeSpecifier> type,
		const yy::location type_position, const_shared_ptr<string> name,
		const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) const {
	return make_shared<ComplexInstantiationStatement>(position,
			static_pointer_cast<const RecordTypeSpecifier>(type), type_position,
			name, name_position, initializer_expression);
}

const bool RecordTypeSpecifier::IsAssignableTo(
		const_shared_ptr<TypeSpecifier> other) const {
	const_shared_ptr<RecordTypeSpecifier> as_record = std::dynamic_pointer_cast<
			const RecordTypeSpecifier>(other);
	return (as_record && as_record->GetTypeName()->compare(*m_type_name) == 0);
}

const_shared_ptr<void> RecordTypeSpecifier::DefaultValue(
		const TypeTable& type_table) const {
	const_shared_ptr<RecordType> type = type_table.GetType<RecordType>(
			m_type_name);

	//this result cannot be cached because the type table is mutable
	if (type) {
		return type->GetDefaultValue(m_type_name);
	} else {
		return const_shared_ptr<RecordType>(); //ugh, nulls
	}
}

const_shared_ptr<Symbol> RecordTypeSpecifier::GetSymbol(
		const_shared_ptr<void> value, const TypeTable& container) const {
	return make_shared<Symbol>(static_pointer_cast<const Record>(value));
}

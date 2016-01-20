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

#include <array_type_specifier.h>
#include <array.h>
#include <typeinfo>
#include <array_declaration_statement.h>
#include <expression.h>
#include <sum_type_specifier.h>

const string ArrayTypeSpecifier::ToString() const {
	ostringstream buffer;

	buffer << m_element_type_specifier->ToString();
	buffer << "[]";
	return buffer.str();
}

bool ArrayTypeSpecifier::operator ==(const TypeSpecifier& other) const {
	try {
		const ArrayTypeSpecifier& as_array =
				dynamic_cast<const ArrayTypeSpecifier&>(other);
		return *GetElementTypeSpecifier() == *as_array.GetElementTypeSpecifier();
	} catch (std::bad_cast& e) {
		return false;
	}
}

const_shared_ptr<DeclarationStatement> ArrayTypeSpecifier::GetDeclarationStatement(
		const yy::location position, const_shared_ptr<TypeSpecifier> type,
		const yy::location type_position, const_shared_ptr<string> name,
		const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) const {
	return make_shared<ArrayDeclarationStatement>(position,
			static_pointer_cast<const ArrayTypeSpecifier>(type), type_position,
			name, name_position, initializer_expression);
}

const_shared_ptr<void> ArrayTypeSpecifier::DefaultValue(
		const TypeTable& type_table) const {
	return const_shared_ptr<void>(
			new Array(m_element_type_specifier, type_table));
}

const bool ArrayTypeSpecifier::IsAssignableTo(
		const_shared_ptr<TypeSpecifier> other) const {
	const_shared_ptr<SumTypeSpecifier> as_sum = dynamic_pointer_cast<
			const SumTypeSpecifier>(other);
	if (as_sum) {
		return as_sum->ContainsType(*this, ALLOW_WIDENING);
	}

	const_shared_ptr<ArrayTypeSpecifier> as_array = std::dynamic_pointer_cast<
			const ArrayTypeSpecifier>(other);
	return as_array
			&& m_element_type_specifier->IsAssignableTo(
					as_array->GetElementTypeSpecifier());
}

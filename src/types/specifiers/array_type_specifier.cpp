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
#include <array_declaration_statement.h>
#include <typeinfo>
#include <expression.h>
#include <array_type.h>

ArrayTypeSpecifier::ArrayTypeSpecifier(
		const_shared_ptr<TypeSpecifier> element_type_specifier) :
		m_element_type_specifier(element_type_specifier), m_type(
				make_shared<ArrayType>(m_element_type_specifier)) {
}

ArrayTypeSpecifier::~ArrayTypeSpecifier() {
}

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

const AnalysisResult ArrayTypeSpecifier::AnalyzeAssignmentTo(
		const_shared_ptr<TypeSpecifier> other, const TypeTable& type_table,
		const_shared_ptr<type_specifier_map> type_specifier_mapping) const {
	const_shared_ptr<ArrayTypeSpecifier> as_array = std::dynamic_pointer_cast<
			const ArrayTypeSpecifier>(other);
	if (as_array) {
		return m_element_type_specifier->AnalyzeAssignmentTo(
				as_array->GetElementTypeSpecifier(), type_table,
				type_specifier_mapping);
	}

	return INCOMPATIBLE;
}

const_shared_ptr<Result> ArrayTypeSpecifier::GetType(
		const TypeTable& type_table, AliasResolution resolution) const {
	return make_shared<Result>(m_type, ErrorList::GetTerminator());
}

const ErrorListRef ArrayTypeSpecifier::ValidateDeclaration(
		const TypeTable& type_table,
		const TypeSpecifierListRef type_parameter_list,
		const yy::location position) const {
	return m_element_type_specifier->ValidateDeclaration(type_table,
			type_parameter_list, position);
}

const_shared_ptr<TypeSpecifier> ArrayTypeSpecifier::WithTypeArgumentList(
		TypeSpecifierListRef type_arguments) const {
	return make_shared<ArrayTypeSpecifier>(m_element_type_specifier);
}

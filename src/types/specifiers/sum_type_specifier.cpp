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
#include <sum_type_specifier.h>
#include <sum.h>
#include <complex_type_specifier.h>
#include <sum_type.h>

SumTypeSpecifier::SumTypeSpecifier(const ComplexTypeSpecifier& complex) :
		SumTypeSpecifier(complex.GetTypeName(), complex.GetNamespace()) {
}

SumTypeSpecifier::SumTypeSpecifier(
		const_shared_ptr<ComplexTypeSpecifier> complex) :
		SumTypeSpecifier(complex->GetTypeName(), complex->GetNamespace()) {
}

const string SumTypeSpecifier::ToString() const {
	return *m_type_name;
}

const bool SumTypeSpecifier::IsAssignableTo(
		const_shared_ptr<TypeSpecifier> other,
		const TypeTable& type_table) const {
	const_shared_ptr<SumTypeSpecifier> as_sum = std::dynamic_pointer_cast<
			const SumTypeSpecifier>(other);
	if (as_sum) {
		return as_sum->GetTypeName()->compare(*m_type_name) == 0;
	}

	auto unaliased = other->ResolveAliasing(type_table);
	if (unaliased) {
		return IsAssignableTo(unaliased, type_table);
	}

	return false;
}

const_shared_ptr<void> SumTypeSpecifier::DefaultValue(
		const TypeTable& type_table) const {
	auto type = type_table.GetType<SumType>(*m_type_name);
	if (type) {
		return type->GetDefaultValue(m_type_name, type_table);
	}
	return const_shared_ptr<void>();
}

const_shared_ptr<DeclarationStatement> SumTypeSpecifier::GetDeclarationStatement(
		const yy::location position, const_shared_ptr<TypeSpecifier> type,
		const yy::location type_position, const_shared_ptr<string> name,
		const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) const {
	return make_shared<ComplexInstantiationStatement>(position,
			static_pointer_cast<const RecordTypeSpecifier>(type), type_position,
			name, name_position, initializer_expression);
}

bool SumTypeSpecifier::operator ==(const TypeSpecifier& other) const {
	try {
		const SumTypeSpecifier& as_sum =
				dynamic_cast<const SumTypeSpecifier&>(other);
		return *GetTypeName() == *as_sum.GetTypeName();
	} catch (std::bad_cast& e) {
		return false;
	}
}

const_shared_ptr<Symbol> SumTypeSpecifier::GetSymbol(
		const_shared_ptr<void> value, const TypeTable& container) const {
	return make_shared<Symbol>(static_pointer_cast<const Sum>(value));
}

const_shared_ptr<TypeDefinition> SumTypeSpecifier::GetType(
		const TypeTable& type_table, AliasResolution resolution) const {
	return type_table.GetType<ConcreteType>(GetTypeName(), resolution);
}

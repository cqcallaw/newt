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

#include <primitive_type_specifier.h>
#include <maybe_type_specifier.h>
#include <maybe_type.h>
#include <unit_type.h>
#include <sum.h>
#include <maybe_declaration_statement.h>
#include <type_alias_declaration_statement.h>

MaybeType::MaybeType(const_shared_ptr<TypeTable> type_table,
		const_shared_ptr<string> first_variant_name,
		const_shared_ptr<SymbolTable> constructors,
		const_shared_ptr<TypeSpecifier> base_type_specifier) :
		SumType(type_table, first_variant_name, constructors, nullptr,
				TypeSpecifierList::GetTerminator()), m_base_type_specifier(
				base_type_specifier) {
}

MaybeType::~MaybeType() {
}

const_shared_ptr<void> MaybeType::GetDefaultValue(const TypeTable& type_table,
		const_shared_ptr<type_parameter_map> type_mapping) const {
	auto value = TypeTable::GetNilType()->GetDefaultValue(type_table,
			ComplexType::DefaultTypeParameterMap);
	return make_shared<Sum>(TypeTable::GetNilName(), value);
}

const std::string MaybeType::ValueToString(const TypeTable& type_table,
		const Indent& indent, const_shared_ptr<void> value,
		const_shared_ptr<type_parameter_map> type_mapping) const {
	ostringstream buffer;
	auto sum_instance = static_pointer_cast<const Sum>(value);
	auto tag = sum_instance->GetTag();
	buffer << "{" << *tag << "}";
	if (*tag == *TypeTable::GetNilName()) {
	} else if (*tag == *MaybeTypeSpecifier::VARIANT_NAME) {
		auto type_result = m_base_type_specifier->GetType(type_table, RESOLVE);
		if (ErrorList::IsTerminator(type_result->GetErrors())) {
			auto type = type_result->GetData<TypeDefinition>();
			auto inner_value = sum_instance->GetValue();
			buffer
					<< type->GetValueSeparator(indent, inner_value.get(),
							type_mapping);
			buffer
					<< type->ValueToString(type_table, indent + 1, inner_value,
							type_mapping);
		} else {
			buffer << "<undefined type>";
		}
	} else {
		buffer << "<undefined variant>";
	}
	return buffer.str();
}

const std::string MaybeType::GetValueSeparator(const Indent& indent,
		const void* value,
		const_shared_ptr<type_parameter_map> type_mapping) const {
	ostringstream buffer;
	auto sum_instance = static_cast<const Sum*>(value);

	auto tag = sum_instance->GetTag();
	if (*tag == *MaybeTypeSpecifier::VARIANT_NAME) {
		buffer << "\n" << indent + 1;
	} else if (*tag == *TypeTable::GetNilName()) {
		buffer << " ";
	}
	return buffer.str();
}

const_shared_ptr<TypeSpecifier> MaybeType::GetTypeSpecifier(
		const_shared_ptr<std::string> name,
		const_shared_ptr<ComplexTypeSpecifier> container,
		yy::location location) const {
	return make_shared<MaybeTypeSpecifier>(m_base_type_specifier, location);
}

const_shared_ptr<Symbol> MaybeType::GetSymbol(const TypeTable& type_table,
		const_shared_ptr<TypeSpecifier> type_specifier,
		const_shared_ptr<void> value,
		const_shared_ptr<type_parameter_map> type_mapping) const {
	auto cast = static_pointer_cast<const Sum>(value);
	const_shared_ptr<MaybeTypeSpecifier> specifier = make_shared<
			MaybeTypeSpecifier>(m_base_type_specifier, GetDefaultLocation());
	return make_shared<Symbol>(specifier, cast);
}

const std::string MaybeType::GetTagSeparator(const Indent& indent,
		const void* value,
		const_shared_ptr<type_parameter_map> type_mapping) const {
	return "";
}

const_shared_ptr<DeclarationStatement> MaybeType::GetDeclarationStatement(
		const yy::location position, const_shared_ptr<TypeSpecifier> type,
		const yy::location type_position, const_shared_ptr<std::string> name,
		const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) const {
	return make_shared<MaybeDeclarationStatement>(position,
			static_pointer_cast<const MaybeTypeSpecifier>(type), type_position,
			name, name_position, initializer_expression);
}

const_shared_ptr<Result> MaybeType::Build(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<TypeSpecifier> base_type_specifier) {
	return Build(context->GetTypeTable(), base_type_specifier);
}

const_shared_ptr<Result> MaybeType::Build(
		const shared_ptr<TypeTable> base_type_table,
		const_shared_ptr<TypeSpecifier> base_type_specifier) {
	auto definition = make_shared<TypeTable>(base_type_table);

	auto constructors = make_shared<SymbolTable>(); //TODO: generate type constructors

	auto nil_alias = make_shared<AliasDefinition>(base_type_table,
			TypeTable::GetNilTypeSpecifier(), DIRECT);
	definition->AddType(*TypeTable::GetNilName(), nil_alias);
	auto value_alias = make_shared<AliasDefinition>(base_type_table,
			base_type_specifier, DIRECT);
	definition->AddType(*MaybeTypeSpecifier::VARIANT_NAME, value_alias);

	auto maybe_type = const_shared_ptr<MaybeType>(
			new MaybeType(definition, TypeTable::GetNilName(), constructors,
					base_type_specifier));
	return make_shared<Result>(maybe_type, ErrorList::GetTerminator());
}

const_shared_ptr<Result> MaybeType::Build(
		const_shared_ptr<PrimitiveTypeSpecifier> base_type_specifier) {
	static shared_ptr<TypeTable> parent_type_table = make_shared<TypeTable>();
	shared_ptr<TypeTable> definition = make_shared<TypeTable>();

	auto constructors = make_shared<SymbolTable>(); //TODO: generate type constructors

	auto nil_alias = make_shared<AliasDefinition>(parent_type_table,
			TypeTable::GetNilTypeSpecifier(), DIRECT);
	definition->AddType(*TypeTable::GetNilName(), nil_alias);
	auto value_alias = make_shared<AliasDefinition>(parent_type_table,
			base_type_specifier, DIRECT);
	definition->AddType(*MaybeTypeSpecifier::VARIANT_NAME, value_alias);

	auto maybe_type = const_shared_ptr<MaybeType>(
			new MaybeType(definition, TypeTable::GetNilName(), constructors,
					base_type_specifier));
	return make_shared<Result>(maybe_type, ErrorList::GetTerminator());
}

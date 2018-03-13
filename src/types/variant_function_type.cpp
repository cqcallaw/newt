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

#include <variant_function_type.h>
#include <sstream>
#include <function.h>
#include <variant_function_specifier.h>
#include <execution_context.h>
#include <function_declaration_statement.h>

VariantFunctionType::VariantFunctionType(
		const FunctionVariantListRef variant_list) :
		m_variant_list(variant_list) {
}

VariantFunctionType::~VariantFunctionType() {
}

const_shared_ptr<void> VariantFunctionType::GetDefaultValue(
		const TypeTable& type_table,
		const_shared_ptr<type_parameter_map> type_mapping) const {
	const const_shared_ptr<void> default_value = GetDefaultFunction(
			m_variant_list, type_table, type_mapping);
	return default_value;
}

const std::string VariantFunctionType::ToString(const TypeTable& type_table,
		const Indent& indent,
		const_shared_ptr<type_parameter_map> type_mapping) const {
	ostringstream buffer;
	auto subject = m_variant_list;
	while (!FunctionVariantList::IsTerminator(subject)) {
		auto variant = subject->GetData();
		buffer << variant->ToString(indent) << endl;
		subject = subject->GetNext();
	}
	return buffer.str();
}

const std::string VariantFunctionType::ValueToString(
		const TypeTable& type_table, const Indent& indent,
		const_shared_ptr<void> value,
		const_shared_ptr<type_parameter_map> type_mapping) const {
	auto as_function = static_pointer_cast<const Function>(value);
	return as_function->ToString(type_table, indent + 1);
}

const std::string VariantFunctionType::GetValueSeparator(const Indent& indent,
		const void* value,
		const_shared_ptr<type_parameter_map> type_mapping) const {
	return "\n";
}

const_shared_ptr<TypeSpecifier> VariantFunctionType::GetTypeSpecifier(
		const_shared_ptr<std::string> name,
		const_shared_ptr<ComplexTypeSpecifier> container,
		yy::location location) const {
	return make_shared<VariantFunctionSpecifier>(GetDefaultLocation(),
			m_variant_list);
}

const_shared_ptr<Symbol> VariantFunctionType::GetSymbol(
		const TypeTable& type_table,
		const_shared_ptr<TypeSpecifier> type_specifier,
		const_shared_ptr<void> value,
		const_shared_ptr<type_parameter_map> type_mapping) const {
	return make_shared<Symbol>(static_pointer_cast<const Function>(value));
}

const_shared_ptr<DeclarationStatement> VariantFunctionType::GetDeclarationStatement(
		const yy::location location, const_shared_ptr<TypeSpecifier> type,
		const yy::location type_position, const_shared_ptr<std::string> name,
		const yy::location name_location,
		const_shared_ptr<Expression> initializer_expression) const {
	return make_shared<FunctionDeclarationStatement>(location,
			static_pointer_cast<const VariantFunctionSpecifier>(type), name,
			name_location, initializer_expression);
}

const_shared_ptr<Function> VariantFunctionType::GetDefaultFunction(
		const FunctionVariantListRef variant_list, const TypeTable& type_table,
		const_shared_ptr<type_parameter_map> type_mapping) {
	// TODO: pass type arguments to function builder
	return Function::Build(GetDefaultLocation(), variant_list,
			ExecutionContext::GetDefault());
}

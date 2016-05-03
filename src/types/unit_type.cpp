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

#include <unit_type.h>
#include <unit_declaration_statement.h>
#include <sstream>

UnitType::UnitType() :
		m_value(make_shared<Unit>()), m_table(make_shared<TypeTable>()) {
}

UnitType::~UnitType() {
}

const_shared_ptr<void> UnitType::GetDefaultValue(
		const TypeTable& type_table) const {
	return m_value;
}

const std::string UnitType::ToString(const TypeTable& type_table,
		const Indent& indent) const {
	ostringstream buffer;
	buffer << indent + 1 << "<unit>";
	return buffer.str();
}

const std::string UnitType::ValueToString(const TypeTable& type_table,
		const Indent& indent, const_shared_ptr<void> value) const {
	ostringstream buffer;
	buffer << "<unit>";
	return buffer.str();
}

const std::string UnitType::GetValueSeparator(const Indent& indent,
		const void* value) const {
	return " ";
}

const_shared_ptr<TypeSpecifier> UnitType::GetTypeSpecifier(
		const_shared_ptr<std::string> name,
		const_shared_ptr<ComplexTypeSpecifier> container) const {
	return make_shared<ComplexTypeSpecifier>(name, container,
			NamespaceQualifierList::GetTerminator());
}

const_shared_ptr<Symbol> UnitType::GetSymbol(const TypeTable& type_table,
		const_shared_ptr<TypeSpecifier> type_specifier,
		const_shared_ptr<void> value) const {
	return make_shared<Symbol>(type_specifier, m_value);
}

const_shared_ptr<DeclarationStatement> UnitType::GetDeclarationStatement(
		const yy::location position, const_shared_ptr<TypeSpecifier> type,
		const yy::location type_position, const_shared_ptr<std::string> name,
		const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) const {
	return make_shared<UnitDeclarationStatement>(position, type, type_position,
			name, name_position);
}

const_shared_ptr<void> UnitType::GetMemberDefaultValue(
		const_shared_ptr<std::string> member_name) const {
	return const_shared_ptr<void>();
}

const_shared_ptr<TypeTable> UnitType::GetDefinition() const {
	return m_table;
}

const ConversionResult UnitType::AnalyzeConversion(
		const ComplexTypeSpecifier& current,
		const TypeSpecifier& unaliased_other) const {
	return INCOMPATIBLE;
}

const_shared_ptr<Result> UnitType::PreprocessSymbolCore(
		const std::shared_ptr<ExecutionContext> execution_context,
		const_shared_ptr<ComplexTypeSpecifier> type_specifier,
		const_shared_ptr<Expression> initializer) const {
	assert(false);
	return nullptr;

}

const SetResult UnitType::InstantiateCore(
		const std::shared_ptr<ExecutionContext> execution_context,
		const_shared_ptr<ComplexTypeSpecifier> type_specifier,
		const_shared_ptr<TypeSpecifier> value_type_specifier,
		const std::string& instance_name, const_shared_ptr<void> data) const {
	assert(false);
	return INCOMPATIBLE_TYPE;
}

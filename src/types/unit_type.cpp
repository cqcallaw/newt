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
#include <complex_instantiation_statement.h>
#include <expression.h>

UnitType::UnitType(const_shared_ptr<MaybeType> maybe_type) :
		m_value(make_shared<Unit>()), m_maybe_type(maybe_type), m_table(
				make_shared<TypeTable>()) {
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
		const_shared_ptr<ComplexTypeSpecifier> container,
		yy::location location) const {
	return make_shared<ComplexTypeSpecifier>(name, container,
			NamespaceQualifierList::GetTerminator(), location);
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
	auto as_complex_specifier =
			dynamic_pointer_cast<const ComplexTypeSpecifier>(type);
	if (as_complex_specifier) {
		return make_shared<ComplexInstantiationStatement>(position,
				as_complex_specifier, type_position, name, name_position,
				initializer_expression);
	} else {
		assert(false);
		return nullptr;
	}
}

const_shared_ptr<void> UnitType::GetMemberDefaultValue(
		const_shared_ptr<std::string> member_name) const {
	return const_shared_ptr<void>();
}

const_shared_ptr<TypeTable> UnitType::GetDefinition() const {
	return m_table;
}

const AnalysisResult UnitType::AnalyzeConversion(
		const ComplexTypeSpecifier& current,
		const TypeSpecifier& unaliased_other) const {
	return INCOMPATIBLE;
}

const_shared_ptr<Result> UnitType::PreprocessSymbolCore(
		const std::shared_ptr<ExecutionContext> execution_context,
		const_shared_ptr<ComplexTypeSpecifier> type_specifier,
		const_shared_ptr<Expression> initializer) const {
	plain_shared_ptr<Unit> instance = nullptr;
	plain_shared_ptr<Symbol> symbol = Symbol::GetDefaultSymbol();

	auto initializer_expression_type_result = initializer->GetTypeSpecifier(
			execution_context);

	auto errors = initializer_expression_type_result.GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto initializer_expression_type =
				initializer_expression_type_result.GetData();
		auto initializer_analysis =
				initializer_expression_type->AnalyzeAssignmentTo(type_specifier,
						execution_context->GetTypeTable());
		if (initializer_analysis == EQUIVALENT) {
			instance = m_value;
		} else {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::ASSIGNMENT_TYPE_ERROR,
							initializer->GetLocation().begin,
							type_specifier->ToString(),
							initializer_expression_type->ToString()), errors);
		}

		if (ErrorList::IsTerminator(errors)) {
			symbol = make_shared<Symbol>(type_specifier, instance);
		}
	}

	return make_shared<Result>(symbol, errors);
}

const SetResult UnitType::InstantiateCore(
		const std::shared_ptr<ExecutionContext> execution_context,
		const_shared_ptr<ComplexTypeSpecifier> type_specifier,
		const_shared_ptr<TypeSpecifier> value_type_specifier,
		const std::string& instance_name, const_shared_ptr<void> data) const {
	auto instance = static_pointer_cast<const Unit>(data);

	if (value_type_specifier->AnalyzeAssignmentTo(type_specifier,
			execution_context->GetTypeTable())) {
		auto set_result = execution_context->SetSymbol(instance_name,
				type_specifier, instance, execution_context->GetTypeTable());
		return set_result;
	} else {
		return SetResult::INCOMPATIBLE_TYPE;
	}
}

const_shared_ptr<MaybeType> UnitType::GetMaybeType() const {
	return m_maybe_type;
}

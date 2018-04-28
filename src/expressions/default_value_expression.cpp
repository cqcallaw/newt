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

#include <default_value_expression.h>
#include <record_type.h>
#include <sum_type.h>
#include <type_table.h>
#include <execution_context.h>
#include <nested_type_specifier.h>
#include <record_type_specifier.h>
#include <sum_type_specifier.h>
#include <placeholder_type.h>

DefaultValueExpression::DefaultValueExpression(const yy::location position,
		const_shared_ptr<TypeSpecifier> type, const yy::location type_position) :
		Expression(position), m_type_specifier(type), m_type_position(
				type_position) {
}

DefaultValueExpression::DefaultValueExpression(
		const DefaultValueExpression* other) :
		Expression(other->GetLocation()), m_type_specifier(
				other->m_type_specifier), m_type_position(
				other->m_type_position) {
}

DefaultValueExpression::~DefaultValueExpression() {
}

TypedResult<TypeSpecifier> DefaultValueExpression::GetTypeSpecifier(
		const shared_ptr<ExecutionContext> execution_context,
		AliasResolution resolution) const {
	if (resolution == RESOLVE) {
		return NestedTypeSpecifier::Resolve(m_type_specifier,
				*execution_context->GetTypeTable());
	} else {
		return m_type_specifier;
	}
}

const_shared_ptr<Result> DefaultValueExpression::Evaluate(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	auto type_table = *context->GetTypeTable();
	//here we RETURN aliases instead of RESOLVING them so that
	//the _alias'_ default value will be used instead of the aliased type's default value
	auto type_result = m_type_specifier->GetType(type_table, RETURN);

	plain_shared_ptr<void> return_value = nullptr;
	auto errors = type_result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto type = type_result->GetData<TypeDefinition>();
		auto type_specifier_mapping_result = ComplexType::GetTypeParameterMap(
				type->GetTypeParameterList(),
				m_type_specifier->GetTypeArgumentList(),
				context->GetTypeTable());
		errors = type_specifier_mapping_result.GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto type_specifier_mapping = type_specifier_mapping_result.GetData();
			return_value = type->GetDefaultValue(type_table, type_specifier_mapping);
		}
	}

	return make_shared<Result>(return_value, errors);
}

const ErrorListRef DefaultValueExpression::Validate(
		const shared_ptr<ExecutionContext> execution_context) const {
	auto type_table = *execution_context->GetTypeTable();
	auto type_result = m_type_specifier->GetType(type_table, RESOLVE);

	auto errors = type_result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto type = type_result->GetData<TypeDefinition>();
		auto as_placeholder = dynamic_pointer_cast<const PlaceholderType>(type);
		if (as_placeholder) {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::PARTIALLY_DECLARED_TYPE,
							m_type_position.begin,
							m_type_specifier->ToString()), errors);
		}
	}

	return errors;
}

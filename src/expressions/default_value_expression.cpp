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

DefaultValueExpression::DefaultValueExpression(const yy::location position,
		const_shared_ptr<TypeSpecifier> type, const yy::location type_position) :
		Expression(position), m_type(type), m_type_position(type_position) {
}

DefaultValueExpression::DefaultValueExpression(
		const DefaultValueExpression* other) :
		Expression(other->GetPosition()), m_type(other->m_type), m_type_position(
				other->m_type_position) {
}

DefaultValueExpression::~DefaultValueExpression() {
}

const_shared_ptr<TypeSpecifier> DefaultValueExpression::GetTypeSpecifier(
		const shared_ptr<ExecutionContext> execution_context,
		AliasResolution resolution) const {
	return NestedTypeSpecifier::Resolve(m_type,
			*execution_context->GetTypeTable());
}

const_shared_ptr<Result> DefaultValueExpression::Evaluate(
		const shared_ptr<ExecutionContext> execution_context) const {
	auto type_table = *execution_context->GetTypeTable();
	auto type = m_type->GetType(type_table, RETURN);
	plain_shared_ptr<void> return_value = type->GetDefaultValue(type_table);
	return make_shared<Result>(return_value, ErrorList::GetTerminator());
}

const ErrorListRef DefaultValueExpression::Validate(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	auto type_table = *execution_context->GetTypeTable();
	auto resolved_specifier = GetTypeSpecifier(execution_context);
	auto type = resolved_specifier->GetType(type_table, RESOLVE);

	if (!type) {
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC, Error::UNDECLARED_TYPE,
						m_type_position.begin.line,
						m_type_position.begin.column, m_type->ToString()),
				errors);
	}

	return errors;
}

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

#include "constant_expression.h"

ConstantExpression::ConstantExpression(const yy::location position,
		const bool value) :
		ConstantExpression(position, PrimitiveTypeSpecifier::GetBoolean(),
				const_shared_ptr<void>(new bool(value))) {
}

ConstantExpression::ConstantExpression(const yy::location position,
		const std::uint8_t value) :
		ConstantExpression(position, PrimitiveTypeSpecifier::GetByte(),
				const_shared_ptr<void>(new std::uint8_t(value))) {
}

ConstantExpression::ConstantExpression(const yy::location position,
		const int value) :
		ConstantExpression(position, PrimitiveTypeSpecifier::GetInt(),
				const_shared_ptr<void>(new int(value))) {
}

ConstantExpression::ConstantExpression(const yy::location position,
		const double value) :
		ConstantExpression(position, PrimitiveTypeSpecifier::GetDouble(),
				const_shared_ptr<void>(new double(value))) {
}

ConstantExpression::ConstantExpression(const yy::location position,
		const_shared_ptr<string> value) :
		ConstantExpression(position, PrimitiveTypeSpecifier::GetString(),
				std::static_pointer_cast<const void>(value)) {
}

ConstantExpression::ConstantExpression(const yy::location position,
		const_shared_ptr<Array> value) :
		ConstantExpression(position, value->GetTypeSpecifier(),
				std::static_pointer_cast<const void>(value)) {
}

ConstantExpression::ConstantExpression(const ConstantExpression* other) :
		ConstantExpression(other->GetLocation(), other->m_type, other->m_value) {
}

ConstantExpression::~ConstantExpression() {
}

TypedResult<TypeSpecifier> ConstantExpression::GetTypeSpecifier(
		const shared_ptr<ExecutionContext> execution_context,
		AliasResolution resolution) const {
	return TypedResult<TypeSpecifier>(m_type);
}

const_shared_ptr<Result> ConstantExpression::Evaluate(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	return make_shared<Result>(m_value, ErrorList::GetTerminator());
}

const_shared_ptr<ConstantExpression> ConstantExpression::GetDefaultExpression(
		const_shared_ptr<TypeSpecifier> type, const TypeTable& type_table) {
	return const_shared_ptr<ConstantExpression>(
			new ConstantExpression(GetDefaultLocation(), type,
					type->DefaultValue(type_table)));
}

const_shared_ptr<Result> ConstantExpression::GetConstantExpression(
		const_shared_ptr<Expression> expression,
		const shared_ptr<ExecutionContext> context) {
	const_shared_ptr<Result> evaluation = expression->Evaluate(context,
			context);
	plain_shared_ptr<void> result;

	auto errors = evaluation->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto expression_type_specifier_result = expression->GetTypeSpecifier(
				context);
		auto expression_type_specifier_errors =
				expression_type_specifier_result.GetErrors();
		if (ErrorList::IsTerminator(expression_type_specifier_errors)) {
			auto expression_type_specifier =
					expression_type_specifier_result.GetData();
			result = const_shared_ptr<void>(
					new ConstantExpression(expression->GetLocation(),
							expression_type_specifier,
							evaluation->GetRawData()));
		} else {
			errors = ErrorList::Concatenate(errors,
					expression_type_specifier_errors);
		}
	}

	return make_shared<Result>(result, errors);
}

ConstantExpression::ConstantExpression(const yy::location position,
		const_shared_ptr<TypeSpecifier> type, const_shared_ptr<void> value) :
		Expression(position), m_type(type), m_value(value) {
}

const ErrorListRef ConstantExpression::Validate(
		const shared_ptr<ExecutionContext> execution_context) const {
	return ErrorList::GetTerminator();
}

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

const_shared_ptr<TypeSpecifier> ConstantExpression::GetType(
		const ExecutionContext* execution_context) const {
	return m_type;
}

const Result* ConstantExpression::Evaluate(
		const ExecutionContext* execution_context) const {
	return new Result(m_value, LinkedList<const Error*>::GetTerminator());
}

const ConstantExpression* ConstantExpression::GetDefaultExpression(
		const_shared_ptr<TypeSpecifier> type, const TypeTable& type_table) {
	return new ConstantExpression(GetDefaultLocation(), type,
			type->DefaultValue(type_table));
}

const Result* ConstantExpression::GetConstantExpression(
		const Expression* expression,
		const ExecutionContext* execution_context) {
	const Result* evaluation = expression->Evaluate(execution_context);
	plain_shared_ptr<void> result;

	auto errors = evaluation->GetErrors();
	if (errors->IsTerminator()) {
		result = const_shared_ptr<void>(
				new ConstantExpression(expression->GetPosition(),
						expression->GetType(execution_context),
						evaluation->GetData()));
	}

	delete evaluation;

	return new Result(result, errors);
}

ConstantExpression::ConstantExpression(const yy::location position,
		const_shared_ptr<TypeSpecifier> type, const_shared_ptr<void> value) :
		Expression(position), m_type(type), m_value(value) {
}

const LinkedList<const Error*>* ConstantExpression::Validate(
		const ExecutionContext* execution_context) const {
	return LinkedList<const Error*>::GetTerminator();
}

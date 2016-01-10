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

#ifndef EXPRESSIONS_CONSTANT_EXPRESSION_H_
#define EXPRESSIONS_CONSTANT_EXPRESSION_H_

#include <string>
#include <memory>

#include "expression.h"

class ConstantExpression: public Expression {
public:
	ConstantExpression(const yy::location position, const bool value);
	ConstantExpression(const yy::location position, const int value);
	ConstantExpression(const yy::location position, const double value);
	ConstantExpression(const yy::location position,
			const_shared_ptr<string> value);
	ConstantExpression(const ConstantExpression* other);
	~ConstantExpression();

	static const_shared_ptr<ConstantExpression> GetDefaultExpression(
			const_shared_ptr<TypeSpecifier> type, const TypeTable& type_table);

	virtual const_shared_ptr<TypeSpecifier> GetType(
			const shared_ptr<ExecutionContext> execution_context) const;

	virtual const_shared_ptr<Result> Evaluate(
			const shared_ptr<ExecutionContext> execution_context) const;

	virtual const bool IsConstant() const {
		return true;
	}

	virtual const ErrorListRef Validate(
			const shared_ptr<ExecutionContext> execution_context) const;

	static const_shared_ptr<Result> GetConstantExpression(
			const_shared_ptr<Expression> expression,
			const shared_ptr<ExecutionContext> execution_context);

private:
	ConstantExpression(const yy::location position,
			const_shared_ptr<TypeSpecifier> type,
			const_shared_ptr<const void> value);
	const_shared_ptr<TypeSpecifier> m_type;
	const_shared_ptr<void> m_value;
};

#endif /* EXPRESSIONS_CONSTANT_EXPRESSION_H_ */

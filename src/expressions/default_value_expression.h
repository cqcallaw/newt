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

#ifndef EXPRESSIONS_DEFAULT_VALUE_EXPRESSION_H_
#define EXPRESSIONS_DEFAULT_VALUE_EXPRESSION_H_

#include <expression.h>

class DefaultValueExpression: public Expression {
public:
	DefaultValueExpression(const yy::location position,
			const_shared_ptr<TypeSpecifier> type,
			const yy::location type_position);

	DefaultValueExpression(const DefaultValueExpression* other);

	virtual ~DefaultValueExpression();

	virtual const_shared_ptr<TypeSpecifier> GetTypeSpecifier(
			const shared_ptr<ExecutionContext> execution_context,
			AliasResolution resolution =
					AliasResolution::RESOLVE) const;

	virtual const_shared_ptr<Result> Evaluate(
			const shared_ptr<ExecutionContext> execution_context) const;

	virtual const bool IsConstant() const {
		return true;
	}

	virtual const ErrorListRef Validate(
			const shared_ptr<ExecutionContext> execution_context) const;

private:
	const_shared_ptr<TypeSpecifier> m_type_specifier;
	const yy::location m_type_position;
};

#endif /* EXPRESSIONS_DEFAULT_VALUE_EXPRESSION_H_ */

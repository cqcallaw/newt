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

#ifndef EXPRESSIONS_FUNCTION_EXPRESSION_H_
#define EXPRESSIONS_FUNCTION_EXPRESSION_H_

#include <expression.h>
#include <yyltype.h>

class FunctionTypeSpecifier;
class StatementBlock;
class DeclarationList;

class FunctionExpression: public Expression {
public:
	FunctionExpression(const YYLTYPE position,
			const DeclarationList* parameter_list,
			const TypeSpecifier* return_type, const StatementBlock* body);
	virtual ~FunctionExpression();

	virtual const TypeSpecifier* GetType(
			const ExecutionContext* execution_context) const;

	virtual const Result* Evaluate(
			const ExecutionContext* execution_context) const;

	virtual const bool IsConstant() const;

	virtual const LinkedList<const Error*>* Validate(
			const ExecutionContext* execution_context) const;

	const FunctionTypeSpecifier* GetType() const {
		return m_type;
	}

private:
	const FunctionTypeSpecifier* m_type;
	const StatementBlock* m_body;
};

#endif /* EXPRESSIONS_FUNCTION_EXPRESSION_H_ */

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

#ifndef STATEMENTS_FUNCTION_DECLARATION_STATEMENT_H_
#define STATEMENTS_FUNCTION_DECLARATION_STATEMENT_H_

#include <declaration_statement.h>
#include <string>

class StatementBlock;
class Expression;
class FunctionTypeSpecifier;

class FunctionDeclarationStatement: public DeclarationStatement {
public:
	FunctionDeclarationStatement(const yy::location position,
			const_shared_ptr<FunctionTypeSpecifier> type,
			const yy::location type_position, const_shared_ptr<string> name,
			const yy::location name_location, const_shared_ptr<Expression> expression);
	virtual ~FunctionDeclarationStatement();

	virtual const ErrorListRef preprocess(
			const_shared_ptr<ExecutionContext> execution_context) const;

	virtual const ErrorListRef execute(
			shared_ptr<ExecutionContext> execution_context) const;

	virtual const_shared_ptr<Expression> GetInitializerExpression() const;

	virtual const DeclarationStatement* WithInitializerExpression(
			const_shared_ptr<Expression> expression) const;

	virtual const_shared_ptr<TypeSpecifier> GetType() const;

	virtual const_shared_ptr<string> GetName() const {
		return m_name;
	}

private:
	const_shared_ptr<FunctionTypeSpecifier> m_type;
	const yy::location m_type_position;
	const_shared_ptr<string> m_name;
	const yy::location m_name_location;
	const_shared_ptr<Expression> m_initializer_expression;
};

#endif /* STATEMENTS_FUNCTION_DECLARATION_STATEMENT_H_ */

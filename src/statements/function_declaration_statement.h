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
#include <yyltype.h>

class StatementBlock;
class Expression;
class FunctionTypeSpecifier;

class FunctionDeclarationStatement: public DeclarationStatement {
public:
	FunctionDeclarationStatement(const YYLTYPE position,
			const FunctionTypeSpecifier* type, const YYLTYPE type_position,
			const std::string* name, const YYLTYPE name_location,
			const Expression* expression);
	virtual ~FunctionDeclarationStatement();

	virtual const LinkedList<const Error*>* preprocess(
			const ExecutionContext* execution_context) const;

	virtual const LinkedList<const Error*>* execute(
			ExecutionContext* execution_context) const;

	virtual const Expression* GetInitializerExpression() const;

	virtual const DeclarationStatement* WithInitializerExpression(
			const Expression* expression) const;

	virtual const TypeSpecifier* GetType() const;

	virtual const std::string* GetName() const {
		return m_name;
	}

private:
	const FunctionTypeSpecifier* m_type;
	const YYLTYPE m_type_position;
	const std::string* m_name;
	const YYLTYPE m_name_location;
	const Expression* m_initializer_expression;
};

#endif /* STATEMENTS_FUNCTION_DECLARATION_STATEMENT_H_ */

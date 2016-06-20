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

#ifndef STATEMENTS_DECLARATIONS_ARRAY_DECLARATION_STATEMENT_H_
#define STATEMENTS_DECLARATIONS_ARRAY_DECLARATION_STATEMENT_H_

#include <declaration_statement.h>
#include "statement.h"
#include <string>
#include <type.h>
#include <defaults.h>

class Result;
class ArrayTypeSpecifier;

class ArrayDeclarationStatement: public DeclarationStatement {
public:
	ArrayDeclarationStatement(const yy::location position,
			const_shared_ptr<ArrayTypeSpecifier> type_specifier,
			const yy::location type_specifier_location,
			const_shared_ptr<string> name, const yy::location name_location,
			const_shared_ptr<Expression> initializer_expression = nullptr);

	virtual ~ArrayDeclarationStatement();

	virtual const ErrorListRef Preprocess(
			const shared_ptr<ExecutionContext> execution_context) const;

	virtual const ErrorListRef Execute(
			shared_ptr<ExecutionContext> execution_context) const;

	virtual const DeclarationStatement* WithInitializerExpression(
			const_shared_ptr<Expression> expression) const;

	virtual const_shared_ptr<TypeSpecifier> GetTypeSpecifier() const;
	virtual const yy::location GetTypeSpecifierLocation() const;

private:
	const_shared_ptr<ArrayTypeSpecifier> m_type_specifier;
	const yy::location m_type_specifier_location;
};

#endif /* STATEMENTS_DECLARATIONS_ARRAY_DECLARATION_STATEMENT_H_ */

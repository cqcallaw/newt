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

#ifndef STATEMENTS_DECLARATIONS_NESTED_DECLARATION_STATEMENT_H_
#define STATEMENTS_DECLARATIONS_NESTED_DECLARATION_STATEMENT_H_

#include <declaration_statement.h>
#include <nested_type_specifier.h>

class NestedDeclarationStatement: public DeclarationStatement {
public:
	NestedDeclarationStatement(const yy::location position,
			const_shared_ptr<NestedTypeSpecifier> type_specifier,
			const yy::location type_specifier_location,
			const_shared_ptr<string> name, const yy::location name_position,
			const_shared_ptr<Expression> initializer_expression = nullptr);
	virtual ~NestedDeclarationStatement();

	virtual const ErrorListRef Preprocess(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure) const;

	virtual const ErrorListRef Execute(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure) const;

	virtual const DeclarationStatement* WithInitializerExpression(
			const_shared_ptr<Expression> expression) const;

	virtual const const_shared_ptr<TypeSpecifier> GetTypeSpecifier() const {
		return m_type_specifier;
	}

	virtual const yy::location GetTypeSpecifierLocation() const {
		return m_type_specifier_location;
	}

private:
	const_shared_ptr<NestedTypeSpecifier> m_type_specifier;
	const yy::location m_type_specifier_location;
};

#endif /* STATEMENTS_DECLARATIONS_NESTED_DECLARATION_STATEMENT_H_ */

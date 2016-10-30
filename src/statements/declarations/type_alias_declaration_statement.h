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

#ifndef STATEMENTS_DECLARATIONS_TYPE_ALIAS_DECLARATION_STATEMENT_H_
#define STATEMENTS_DECLARATIONS_TYPE_ALIAS_DECLARATION_STATEMENT_H_

#include <declaration_statement.h>

class TypeAliasDeclarationStatement: public DeclarationStatement {
public:
	TypeAliasDeclarationStatement(const yy::location position,
			const_shared_ptr<TypeSpecifier> type_specifier,
			const yy::location type_specifier_location,
			const_shared_ptr<string> name, const yy::location name_position);
	virtual ~TypeAliasDeclarationStatement();

	virtual const_shared_ptr<TypeSpecifier> GetTypeSpecifier() const {
		return m_type_specifier;
	}

	virtual const yy::location GetTypeSpecifierLocation() const {
		return m_type_specifier_location;
	}

	virtual const ErrorListRef Preprocess(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure,
			const_shared_ptr<TypeSpecifier> return_type_specifier = nullptr) const;

	virtual const ErrorListRef Execute(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure) const;

	virtual const DeclarationStatement* WithInitializerExpression(
			const_shared_ptr<Expression> expression) const;

private:
	const_shared_ptr<TypeSpecifier> m_type_specifier;
	const yy::location m_type_specifier_location;
};

#endif /* STATEMENTS_DECLARATIONS_TYPE_ALIAS_DECLARATION_STATEMENT_H_ */

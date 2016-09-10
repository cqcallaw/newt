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

#ifndef STATEMENTS_DECLARATIONS_MAYBE_DECLARATION_STATEMENT_H_ORIG_
#define STATEMENTS_DECLARATIONS_MAYBE_DECLARATION_STATEMENT_H_ORIG_

#include <declaration_statement.h>

class MaybeTypeSpecifier;

class MaybeDeclarationStatement: public DeclarationStatement {
public:
	MaybeDeclarationStatement(const yy::location location,
			const_shared_ptr<MaybeTypeSpecifier> type_specifier,
			const yy::location type_specifier_location,
			const_shared_ptr<string> name, const yy::location name_location,
			const_shared_ptr<Expression> initializer_expression = nullptr);
	virtual ~MaybeDeclarationStatement();

	virtual const ErrorListRef Preprocess(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure,
			const_shared_ptr<TypeSpecifier> return_type_specifier = nullptr) const;

	virtual const ErrorListRef Execute(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure) const;

	virtual const ErrorListRef GetReturnStatementErrors(
			const_shared_ptr<TypeSpecifier> type_specifier,
			const std::shared_ptr<ExecutionContext> execution_context) const;

	virtual const DeclarationStatement* WithInitializerExpression(
			const_shared_ptr<Expression> expression) const;

	virtual const_shared_ptr<TypeSpecifier> GetTypeSpecifier() const;

	virtual const yy::location GetTypeSpecifierLocation() const;

	const_shared_ptr<MaybeTypeSpecifier> GetMaybeTypeSpecifier() const {
		return m_type_specifier;
	}

private:
	const_shared_ptr<MaybeTypeSpecifier> m_type_specifier;
	const yy::location m_type_specifier_location;
};

#endif /* STATEMENTS_DECLARATIONS_MAYBE_DECLARATION_STATEMENT_H_ORIG_ */

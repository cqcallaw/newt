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

#ifndef STATEMENTS_DECLARATIONS_INFERRED_DECLARATION_STATEMENT_H_
#define STATEMENTS_DECLARATIONS_INFERRED_DECLARATION_STATEMENT_H_

#include <declaration_statement.h>
#include <string>

class Expression;
class TypeSpecifier;

class InferredDeclarationStatement: public DeclarationStatement {
public:
	InferredDeclarationStatement(const yy::location position,
			const_shared_ptr<string> name, const yy::location name_position,
			const_shared_ptr<Expression> initializer_expression);
	virtual ~InferredDeclarationStatement();

	virtual const PreprocessResult Preprocess(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure,
			const_shared_ptr<TypeSpecifier> return_type_specifier = nullptr) const;

	virtual const ExecutionResult Execute(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure) const;

	virtual const DeclarationStatement* WithInitializerExpression(
			const_shared_ptr<Expression> expression) const;

	virtual const_shared_ptr<TypeSpecifier> GetTypeSpecifier() const;
	virtual const yy::location GetTypeSpecifierLocation() const;

private:
};

#endif /* STATEMENTS_DECLARATIONS_INFERRED_DECLARATION_STATEMENT_H_ */

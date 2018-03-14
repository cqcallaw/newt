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

#ifndef STATEMENTS_DECLARATIONS_UNIT_DECLARATION_STATEMENT_H_
#define STATEMENTS_DECLARATIONS_UNIT_DECLARATION_STATEMENT_H_

#include <declaration_statement.h>

class ComplexTypeSpecifier;

class UnitDeclarationStatement: public DeclarationStatement {
public:
	UnitDeclarationStatement(const yy::location location,
			const_shared_ptr<TypeSpecifier> type_specifier,
			const yy::location type_specifier_location,
			const_shared_ptr<string> name, const yy::location name_location);
	virtual ~UnitDeclarationStatement();

	virtual const PreprocessResult Preprocess(
			const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure,
		const TypeSpecifierListRef type_parameter_list,
		const_shared_ptr<TypeSpecifier> return_type_specifier = nullptr) const;

	virtual const ExecutionResult Execute(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure) const;

	virtual const_shared_ptr<TypeSpecifier> GetTypeSpecifier() const;
	virtual const yy::location GetTypeSpecifierLocation() const;

	virtual const DeclarationStatement* WithInitializerExpression(
			const_shared_ptr<Expression> expression) const;
private:
	const_shared_ptr<TypeSpecifier> m_type_specifier;
	const yy::location m_type_specifier_location;
};

#endif /* STATEMENTS_DECLARATIONS_UNIT_DECLARATION_STATEMENT_H_ */

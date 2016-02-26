/*
 * array_declaration_statement.h
 *
 *  Created on: Jul 28, 2015
 *      Author: caleb
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
			const_shared_ptr<ArrayTypeSpecifier> type,
			const yy::location type_position, const_shared_ptr<string> name,
			const yy::location name_position,
			const_shared_ptr<Expression> initializer_expression = nullptr);

	virtual ~ArrayDeclarationStatement();

	virtual const ErrorListRef preprocess(
			const shared_ptr<ExecutionContext> execution_context) const;

	virtual const ErrorListRef execute(
			shared_ptr<ExecutionContext> execution_context) const;

	virtual const DeclarationStatement* WithInitializerExpression(
			const_shared_ptr<Expression> expression) const;

	virtual const_shared_ptr<TypeSpecifier> GetType() const;

private:
	const_shared_ptr<ArrayTypeSpecifier> m_type;
	const yy::location m_type_position;
};

#endif /* STATEMENTS_DECLARATIONS_ARRAY_DECLARATION_STATEMENT_H_ */

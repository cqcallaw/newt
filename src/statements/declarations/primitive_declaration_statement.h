/*
 * declaration_statement.h
 *
 *  Created on: Jun 14, 2015
 *      Author: caleb
 */

#ifndef STATEMENTS_DECLARATIONS_PRIMITIVE_DECLARATION_STATEMENT_H_
#define STATEMENTS_DECLARATIONS_PRIMITIVE_DECLARATION_STATEMENT_H_

#include <declaration_statement.h>
#include "statement.h"
#include <string>
#include <defaults.h>
#include <type.h>

class PrimitiveDeclarationStatement: public DeclarationStatement {
public:
	PrimitiveDeclarationStatement(const yy::location position,
			const_shared_ptr<TypeSpecifier> type,
			const yy::location type_position, const_shared_ptr<string> name,
			const yy::location name_position,
			const_shared_ptr<Expression> initializer_expression = nullptr);
	virtual ~PrimitiveDeclarationStatement();

	virtual const ErrorListRef preprocess(
			const shared_ptr<ExecutionContext> execution_context) const;

	virtual const ErrorListRef execute(
			shared_ptr<ExecutionContext> execution_context) const;

	virtual const DeclarationStatement* WithInitializerExpression(
			const_shared_ptr<Expression> expression) const;

	virtual const_shared_ptr<TypeSpecifier> GetTypeSpecifier() const {
		return m_type;
	}

private:
	const_shared_ptr<TypeSpecifier> m_type;
	const yy::location m_type_position;
};

#endif /* STATEMENTS_DECLARATIONS_PRIMITIVE_DECLARATION_STATEMENT_H_ */

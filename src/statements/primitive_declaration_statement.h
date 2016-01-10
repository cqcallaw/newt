/*
 * declaration_statement.h
 *
 *  Created on: Jun 14, 2015
 *      Author: caleb
 */

#ifndef STATEMENTS_PRIMITIVE_DECLARATION_STATEMENT_H_
#define STATEMENTS_PRIMITIVE_DECLARATION_STATEMENT_H_

#include "statement.h"
#include <string>
#include <defaults.h>
#include <type.h>
#include <declaration_statement.h>

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

	virtual const_shared_ptr<Expression> GetInitializerExpression() const {
		return m_initializer_expression;
	}

	virtual const DeclarationStatement* WithInitializerExpression(
			const_shared_ptr<Expression> expression) const;

	virtual const_shared_ptr<TypeSpecifier> GetType() const {
		return m_type;
	}

	virtual const_shared_ptr<string> GetName() const {
		return m_name;
	}

private:
	const_shared_ptr<TypeSpecifier> m_type;
	const yy::location m_type_position;
	const_shared_ptr<string> m_name;
	const yy::location m_name_position;
	const_shared_ptr<Expression> m_initializer_expression;
};

#endif /* STATEMENTS_PRIMITIVE_DECLARATION_STATEMENT_H_ */

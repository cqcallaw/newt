/*
 * array_declaration_statement.h
 *
 *  Created on: Jul 28, 2015
 *      Author: caleb
 */

#ifndef STATEMENTS_ARRAY_DECLARATION_STATEMENT_H_
#define STATEMENTS_ARRAY_DECLARATION_STATEMENT_H_

#include "statement.h"
#include <string>
#include <type.h>
#include <defaults.h>
#include <declaration_statement.h>

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
			const_shared_ptr<ExecutionContext> execution_context) const;

	virtual const ErrorListRef execute(
			shared_ptr<ExecutionContext> execution_context) const;

	virtual const_shared_ptr<Expression> GetInitializerExpression() const {
		return nullptr;
	}

	virtual const DeclarationStatement* WithInitializerExpression(
			const_shared_ptr<Expression> expression) const;

	virtual const_shared_ptr<TypeSpecifier> GetType() const;

	virtual const_shared_ptr<string> GetName() const {
		return m_name;
	}

private:
	const_shared_ptr<ArrayTypeSpecifier> m_type;
	const yy::location m_type_position;
	const_shared_ptr<string> m_name;
	const yy::location m_name_position;
	const_shared_ptr<Expression> m_initializer_expression;
};

#endif /* STATEMENTS_ARRAY_DECLARATION_STATEMENT_H_ */

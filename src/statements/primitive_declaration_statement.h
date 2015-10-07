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
	PrimitiveDeclarationStatement(const TypeSpecifier* type,
			const YYLTYPE type_position, const std::string* name,
			const YYLTYPE name_position,
			const Expression* initializer_expression = nullptr,
			const YYLTYPE initializer_position = DefaultLocation);
	virtual ~PrimitiveDeclarationStatement();

	virtual const LinkedList<const Error*>* preprocess(
			const ExecutionContext* execution_context) const;

	virtual const LinkedList<const Error*>* execute(
			const ExecutionContext* execution_context) const;

	virtual const Expression* GetInitializerExpression() const {
		return m_initializer_expression;
	}

	virtual const TypeSpecifier* GetType() const {
		return m_type;
	}

	virtual const std::string* GetName() const {
		return m_name;
	}

private:
	const TypeSpecifier* m_type;
	const YYLTYPE m_type_position;
	const std::string* m_name;
	const YYLTYPE m_name_position;
	const Expression* m_initializer_expression;
	const YYLTYPE m_initializer_position;
};

#endif /* STATEMENTS_PRIMITIVE_DECLARATION_STATEMENT_H_ */

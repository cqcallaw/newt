/*
 * declaration_statement.h
 *
 *  Created on: Jun 14, 2015
 *      Author: caleb
 */

#ifndef STATEMENTS_DECLARATION_STATEMENT_H_
#define STATEMENTS_DECLARATION_STATEMENT_H_

#include "statement.h"
#include <string>
#include <defaults.h>
#include <type.h>

class DeclarationStatement: public Statement {
public:
	DeclarationStatement(const Type type, const YYLTYPE type_position,
			const std::string* name, const YYLTYPE name_position,
			const Expression* initializer_expression = nullptr,
			const YYLTYPE initializer_position = DefaultLocation);
	virtual ~DeclarationStatement();

	virtual LinkedList<const Error*>* preprocess(
			const ExecutionContext* execution_context) const;

	virtual void execute(const ExecutionContext* execution_context) const;
private:
	const Type m_type;
	const YYLTYPE m_type_position;
	const std::string* m_name;
	const YYLTYPE m_name_position;
	const Expression* m_initializer_expression;
	const YYLTYPE m_initializer_position;
};

#endif /* STATEMENTS_DECLARATION_STATEMENT_H_ */

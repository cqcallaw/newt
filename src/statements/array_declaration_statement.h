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

class Result;

class ArrayDeclarationStatement: public Statement {
public:
	ArrayDeclarationStatement(const Type type, const YYLTYPE type_position,
			const std::string* name, const YYLTYPE name_position,
			const Expression* size_expression = nullptr,
			const YYLTYPE size_expression_position = DefaultLocation);
	virtual ~ArrayDeclarationStatement();

	virtual LinkedList<const Error*>* preprocess(
			const ExecutionContext* execution_context) const;

	virtual const LinkedList<const Error*>* execute(
			const ExecutionContext* execution_context) const;
private:
	const Type m_type;
	const YYLTYPE m_type_position;
	const std::string* m_name;
	const YYLTYPE m_name_position;
	const Expression* m_size_expression;
	const YYLTYPE m_size_expression_position;
};

#endif /* STATEMENTS_ARRAY_DECLARATION_STATEMENT_H_ */

/*
 * print_statement.h
 *
 *  Created on: Mar 20, 2015
 *      Author: caleb
 */

#ifndef PRINT_STATEMENT_H_
#define PRINT_STATEMENT_H_

#include "statement.h"

class Expression;

class PrintStatement: public Statement {
public:
	PrintStatement(const int line_number, const Expression* expression);
	virtual ~PrintStatement();

	virtual void execute() const;

	const Expression* GetExpression() const {
		return m_expression;
	}

private:
	const int m_line_number;
	const Expression* m_expression;
};

#endif /* PRINT_STATEMENT_H_ */

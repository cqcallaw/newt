/*
 * exit_statement.h
 *
 *  Created on: Apr 14, 2015
 *      Author: caleb
 */

#ifndef EXIT_STATEMENT_H_
#define EXIT_STATEMENT_H_

#include "statement.h"

class Expression;

class ExitStatement: public Statement {
public:
	ExitStatement(const int line_number, const Expression* exit_expression);
	virtual ~ExitStatement();

	virtual void execute() const;

private:
	const int m_line_number;
	const Expression* m_exit_expression;
};

#endif /* EXIT_STATEMENT_H_ */

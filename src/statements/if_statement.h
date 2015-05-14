/*
 * if_statement.h
 *
 *  Created on: Mar 20, 2015
 *      Author: caleb
 */

#ifndef IF_STATEMENT_H_
#define IF_STATEMENT_H_

#include "statement.h"

class Expression;
class StatementBlock;

class IfStatement: public Statement {
public:
	IfStatement(const Expression* expression, const StatementBlock* block);
	IfStatement(const Expression* expression, const StatementBlock* block,
			const StatementBlock* else_block);
	virtual ~IfStatement();

	const Expression* GetExpression() const {
		return m_expression;
	}

	virtual void execute() const;
private:
	const Expression* m_expression;
	const StatementBlock* m_block;
	const StatementBlock* m_else_block;
};

#endif /* IF_STATEMENT_H_ */

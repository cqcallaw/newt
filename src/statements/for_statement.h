/*
 * for_statement.h
 *
 *  Created on: Apr 14, 2015
 *      Author: caleb
 */

#ifndef FOR_STATEMENT_H_
#define FOR_STATEMENT_H_

#include "statement.h"

class AssignmentStatement;
class Expression;
class StatementBlock;

class ForStatement: public Statement {
public:
	ForStatement(const AssignmentStatement* initial,
			const Expression* loop_expression,
			const AssignmentStatement* loop_assignment,
			const StatementBlock* statement_block);
	virtual ~ForStatement();

	virtual void execute() const;

private:
	const AssignmentStatement* m_initial;
	const Expression* m_loop_expression;
	const AssignmentStatement* m_loop_assignment;
	const StatementBlock* m_statement_block;
};

#endif /* FOR_STATEMENT_H_ */

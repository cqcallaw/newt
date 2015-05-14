/*
 * assignment_statement.h
 *
 *  Created on: Apr 6, 2015
 *      Author: caleb
 */

#ifndef ASSIGNMENT_STATEMENT_H_
#define ASSIGNMENT_STATEMENT_H_

#include <type.h>
#include "statement.h"

using namespace std;

class Expression;
class Variable;

class AssignmentStatement: public Statement {
public:
	enum AssignmentType {
		ASSIGN, PLUS_ASSIGN, MINUS_ASSIGN
	};

	AssignmentStatement(const Variable* variable, const AssignmentType op_type,
			const Expression* expression);
	virtual ~AssignmentStatement();

	const Expression* GetExpression() const {
		return m_expression;
	}

	const AssignmentType GetOpType() const {
		return m_op_type;
	}

	const Variable* GetVariable() const {
		return m_variable;
	}

	virtual void execute() const;

private:
	const void do_op(const bool old_value, const bool expression_value,
			const AssignmentType op, bool& out) const;
	const void do_op(const int old_value, const int expression_value,
			const AssignmentType op, int& out) const;
	const void do_op(const double old_value, const double expression_value,
			const AssignmentType op, double& out) const;

	const void do_op(const string* old_value, const bool expression_value,
			const AssignmentType op, string* &out) const;
	const void do_op(const string* old_value, const int expression_value,
			const AssignmentType op, string* &out) const;
	const void do_op(const string* old_value, const double expression_value,
			const AssignmentType op, string* &out) const;
	const void do_op(const string* old_value, const string* expression_value,
			const AssignmentType op, string* &out) const;

	const void* do_op(const int value, const Expression* expression,
			const AssignmentType op) const;

	const void* do_op(const double value, const Expression* expression,
			const AssignmentType op) const;

	const void* do_op(const string* value, const Expression* expression,
			const AssignmentType op) const;

	const Variable* m_variable;
	const AssignmentType m_op_type;
	const Expression* m_expression;
};

#endif /* ASSIGNMENT_STATEMENT_H_ */

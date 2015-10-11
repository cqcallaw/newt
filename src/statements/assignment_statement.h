/*
 Copyright (C) 2015 The newt Authors.

 This file is part of newt.

 newt is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 newt is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with newt.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ASSIGNMENT_STATEMENT_H_
#define ASSIGNMENT_STATEMENT_H_

#include <type.h>
#include "statement.h"
#include <assignment_type.h>

using namespace std;

class Expression;
class Variable;
class ArrayVariable;
class MemberVariable;
class Result;
struct YYLTYPE;

class AssignmentStatement: public Statement {
public:
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

	virtual const LinkedList<const Error*>* preprocess(
			const ExecutionContext* execution_context) const;

	virtual const LinkedList<const Error*>* execute(
			const ExecutionContext* execution_context) const;

	const static LinkedList<const Error*>* do_op(const string* variable_name,
			const BasicType variable_type, int variable_line,
			int variable_column, const bool old_value,
			const bool expression_value, const AssignmentType op,
			const ExecutionContext* execution_context, bool& out);
	const static LinkedList<const Error*>* do_op(const string* variable_name,
			const BasicType variable_type, int variable_line,
			int variable_column, const int old_value,
			const int expression_value, const AssignmentType op,
			const ExecutionContext* execution_context, int& out);
	const static LinkedList<const Error*>* do_op(const string* variable_name,
			const BasicType variable_type, int variable_line,
			int variable_column, const double old_value,
			const double expression_value, const AssignmentType op,
			const ExecutionContext* execution_context, double& out);
	const static LinkedList<const Error*>* do_op(const string* variable_name,
			const BasicType variable_type, int variable_line,
			int variable_column, const string* old_value,
			const bool expression_value, const AssignmentType op,
			const ExecutionContext* execution_context, string* &out);
	const static LinkedList<const Error*>* do_op(const string* variable_name,
			const BasicType variable_type, int variable_line,
			int variable_column, const string* old_value,
			const int expression_value, const AssignmentType op,
			const ExecutionContext* execution_context, string* &out);
	const static LinkedList<const Error*>* do_op(const string* variable_name,
			const BasicType variable_type, int variable_line,
			int variable_column, const string* old_value,
			const double expression_value, const AssignmentType op,
			const ExecutionContext* execution_context, string* &out);
	const static LinkedList<const Error*>* do_op(const string* variable_name,
			const BasicType variable_type, int variable_line,
			int variable_column, const string* old_value,
			const string* expression_value, const AssignmentType op,
			const ExecutionContext* execution_context, string* &out);

	const static Result* do_op(const string* variable_name,
			const BasicType variable_type, int variable_line,
			int variable_column, const int value, const Expression* expression,
			const AssignmentType op, const ExecutionContext* execution_context);

	const static Result* do_op(const string* variable_name,
			const BasicType variable_type, int variable_line,
			int variable_column, const double value,
			const Expression* expression, const AssignmentType op,
			const ExecutionContext* execution_context);

	const static Result* do_op(const string* variable_name,
			const BasicType variable_type, int variable_line,
			int variable_column, const string* value,
			const Expression* expression, const AssignmentType op,
			const ExecutionContext* execution_context);

private:
	const Variable* m_variable;
	const AssignmentType m_op_type;
	const Expression* m_expression;

};

#endif /* ASSIGNMENT_STATEMENT_H_ */

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

#include <defaults.h>
#include <type.h>
#include "statement.h"
#include <assignment_type.h>

using namespace std;

class Expression;
class Variable;
class ArrayVariable;
class MemberVariable;
class Result;

class AssignmentStatement: public Statement {
public:
	AssignmentStatement(const_shared_ptr<Variable> variable,
			const AssignmentType op_type,
			const_shared_ptr<Expression> expression);
	virtual ~AssignmentStatement();

	const_shared_ptr<Expression> GetExpression() const {
		return m_expression;
	}

	const AssignmentType GetOpType() const {
		return m_op_type;
	}

	const_shared_ptr<Variable> GetVariable() const {
		return m_variable;
	}

	virtual const ErrorListRef preprocess(
			const shared_ptr<ExecutionContext> execution_context) const;

	virtual const ErrorListRef execute(
			shared_ptr<ExecutionContext> execution_context) const;

	virtual const ErrorListRef GetReturnStatementErrors(
			const_shared_ptr<TypeSpecifier> type_specifier,
			const shared_ptr<ExecutionContext> execution_context) const {
		return ErrorList::GetTerminator();
	}

	static const ErrorListRef do_op(const_shared_ptr<string> variable_name,
			const BasicType variable_type, int variable_line,
			int variable_column, const bool old_value,
			const bool expression_value, const AssignmentType op,
			const shared_ptr<ExecutionContext> execution_context, bool& out);
	static const ErrorListRef do_op(const_shared_ptr<string> variable_name,
			const BasicType variable_type, int variable_line,
			int variable_column, const int old_value,
			const int expression_value, const AssignmentType op,
			const shared_ptr<ExecutionContext> execution_context, int& out);
	static const ErrorListRef do_op(const_shared_ptr<string> variable_name,
			const BasicType variable_type, int variable_line,
			int variable_column, const double old_value,
			const double expression_value, const AssignmentType op,
			const shared_ptr<ExecutionContext> execution_context, double& out);
	static const ErrorListRef do_op(const_shared_ptr<string> variable_name,
			const BasicType variable_type, int variable_line,
			int variable_column, const_shared_ptr<string> old_value,
			const bool expression_value, const AssignmentType op,
			const shared_ptr<ExecutionContext> execution_context, string* &out);
	static const ErrorListRef do_op(const_shared_ptr<string> variable_name,
			const BasicType variable_type, int variable_line,
			int variable_column, const_shared_ptr<string> old_value,
			const int expression_value, const AssignmentType op,
			const shared_ptr<ExecutionContext> execution_context, string* &out);
	static const ErrorListRef do_op(const_shared_ptr<string> variable_name,
			const BasicType variable_type, int variable_line,
			int variable_column, const_shared_ptr<string> old_value,
			const double expression_value, const AssignmentType op,
			const shared_ptr<ExecutionContext> execution_context, string* &out);
	static const ErrorListRef do_op(const_shared_ptr<string> variable_name,
			const BasicType variable_type, int variable_line,
			int variable_column, const_shared_ptr<string> old_value,
			const_shared_ptr<string> expression_value, const AssignmentType op,
			const shared_ptr<ExecutionContext> execution_context, string* &out);

	static const_shared_ptr<Result> do_op(
			const_shared_ptr<string> variable_name,
			const BasicType variable_type, int variable_line,
			int variable_column, const int value,
			const_shared_ptr<Expression> expression, const AssignmentType op,
			const shared_ptr<ExecutionContext> execution_context);

	static const_shared_ptr<Result> do_op(
			const_shared_ptr<string> variable_name,
			const BasicType variable_type, int variable_line,
			int variable_column, const double value,
			const_shared_ptr<Expression> expression, const AssignmentType op,
			const shared_ptr<ExecutionContext> execution_context);

	static const_shared_ptr<Result> do_op(
			const_shared_ptr<string> variable_name,
			const BasicType variable_type, int variable_line,
			int variable_column, const_shared_ptr<string> value,
			const_shared_ptr<Expression> expression, const AssignmentType op,
			const shared_ptr<ExecutionContext> execution_context);

private:
	const_shared_ptr<Variable> m_variable;
	const AssignmentType m_op_type;
	const const_shared_ptr<Expression> m_expression;

};

#endif /* ASSIGNMENT_STATEMENT_H_ */

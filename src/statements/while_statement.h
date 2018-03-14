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

#ifndef STATEMENTS_WHILE_STATEMENT_H_
#define STATEMENTS_WHILE_STATEMENT_H_

#include "statement.h"

class Expression;
class StatementBlock;

class WhileStatement: public Statement {
public:
	enum WhileMode {
		WHILE, DO_WHILE
	};

	WhileStatement(const_shared_ptr<Expression> expression,
			const_shared_ptr<StatementBlock> block, WhileMode mode);
	virtual ~WhileStatement();

	virtual const PreprocessResult Preprocess(
			const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure,
		const TypeSpecifierListRef type_parameter_list,
		const_shared_ptr<TypeSpecifier> return_type_specifier = nullptr) const;

	virtual const ExecutionResult Execute(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure) const;

	const_shared_ptr<StatementBlock> GetBlock() const {
		return m_block;
	}

	const_shared_ptr<Expression> GetExpression() const {
		return m_expression;
	}

private:
	const_shared_ptr<Expression> m_expression;
	const_shared_ptr<StatementBlock> m_block;
	shared_ptr<ExecutionContext> m_block_context;
	const WhileMode m_mode;
};

#endif /* STATEMENTS_WHILE_STATEMENT_H_ */

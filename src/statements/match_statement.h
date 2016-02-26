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

#ifndef STATEMENTS_MATCH_STATEMENT_H_
#define STATEMENTS_MATCH_STATEMENT_H_

#include <statement.h>
#include <match.h>

class Expression;

class MatchStatement: public Statement {
public:
	MatchStatement(const yy::location statement_location,
			const_shared_ptr<Expression> source_expression,
			const MatchListRef match_list,
			const yy::location match_list_location) :
			m_statement_location(statement_location), m_source_expression(
					source_expression), m_match_list(match_list), m_match_list_location(
					match_list_location) {
	}
	virtual ~MatchStatement();

	const yy::location GetStatementLocation() const {
		return m_statement_location;
	}

	const const_shared_ptr<Expression> GetSourceExpression() const {
		return m_source_expression;
	}

	const MatchListRef GetMatchList() const {
		return m_match_list;
	}

	const yy::location GetMatchListLocation() const {
		return m_match_list_location;
	}

	virtual const ErrorListRef preprocess(
			const shared_ptr<ExecutionContext> execution_context) const;

	virtual const ErrorListRef execute(
			shared_ptr<ExecutionContext> execution_context) const;

	virtual const ErrorListRef GetReturnStatementErrors(
			const_shared_ptr<TypeSpecifier> type_specifier,
			const shared_ptr<ExecutionContext> execution_context) const;

private:
	const yy::location m_statement_location;
	const_shared_ptr<Expression> m_source_expression;
	const MatchListRef m_match_list;
	const yy::location m_match_list_location;
};

#endif /* STATEMENTS_MATCH_STATEMENT_H_ */

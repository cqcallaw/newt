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

typedef const LinkedList<ExecutionContext, NO_DUPLICATES> MatchContextList;
typedef std::shared_ptr<MatchContextList> MatchContextListRef;

class MatchStatement: public Statement {
public:
	MatchStatement(const yy::location statement_location,
			const_shared_ptr<Expression> source_expression,
			const MatchListRef match_list,
			const yy::location match_list_location);
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

	virtual const PreprocessResult Preprocess(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure,
			const_shared_ptr<TypeSpecifier> return_type_specifier = nullptr) const;

	virtual const ExecutionResult Execute(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure) const;

	static const MatchContextListRef GenerateMatchContexts(
			const MatchListRef match_list);

private:
	const yy::location m_statement_location;
	const_shared_ptr<Expression> m_source_expression;
	const MatchListRef m_match_list;
	const yy::location m_match_list_location;
	const MatchContextListRef m_match_contexts;
};

#endif /* STATEMENTS_MATCH_STATEMENT_H_ */

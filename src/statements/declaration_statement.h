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

#ifndef STATEMENTS_DECLARATION_STATEMENT_H_
#define STATEMENTS_DECLARATION_STATEMENT_H_

#include <statement.h>
#include <string>

class Expression;
class TypeSpecifier;

using namespace std;

class DeclarationStatement: public Statement {
public:
	DeclarationStatement(const yy::location position);
	virtual ~DeclarationStatement();

	virtual const_shared_ptr<Expression> GetInitializerExpression() const = 0;
	virtual const DeclarationStatement* WithInitializerExpression(
			const_shared_ptr<Expression> expression) const = 0;
	virtual const_shared_ptr<TypeSpecifier> GetType() const = 0;
	virtual const_shared_ptr<string> GetName() const = 0;

	virtual const AnalysisResult Returns(
			const_shared_ptr<TypeSpecifier> type_specifier,
			const shared_ptr<ExecutionContext> execution_context) const {
		return AnalysisResult::NO;
	}

	const yy::location GetPosition() const {
		return m_position;
	}

private:
	const yy::location m_position;

};

typedef const LinkedList<const DeclarationStatement, NO_DUPLICATES> DeclarationList;
typedef shared_ptr<DeclarationList> DeclarationListRef;

#endif /* STATEMENTS_DECLARATION_STATEMENT_H_ */

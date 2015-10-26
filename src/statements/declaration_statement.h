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
#include <yyltype.h>

class Expression;
class TypeSpecifier;

class DeclarationStatement: public Statement {
public:
	DeclarationStatement(const YYLTYPE position);
	virtual ~DeclarationStatement();

	virtual const Expression* GetInitializerExpression() const = 0;
	virtual const DeclarationStatement* WithInitializerExpression(
			const Expression* expression) const = 0;
	virtual const TypeSpecifier* GetType() const = 0;
	virtual const std::string* GetName() const = 0;

	virtual const AnalysisResult Returns(const TypeSpecifier* type_specifier,
			const ExecutionContext* execution_context) const {
		return AnalysisResult::NO;
	}

	const YYLTYPE GetPosition() const {
		return m_position;
	}

private:
	const YYLTYPE m_position;

};

#endif /* STATEMENTS_DECLARATION_STATEMENT_H_ */

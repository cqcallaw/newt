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

#ifndef STATEMENTS_STRUCT_INSTANTIATION_STATEMENT_H_
#define STATEMENTS_STRUCT_INSTANTIATION_STATEMENT_H_

#include <statement.h>
#include <string>
#include <yyltype.h>
#include <member_instantiation_list.h>
#include <type.h>
#include <declaration_statement.h>

class Symbol;
class Expression;
class CompoundTypeSpecifier;

using namespace std;

class StructInstantiationStatement: public DeclarationStatement {
public:
	StructInstantiationStatement(const CompoundTypeSpecifier* type_specifier,
			const YYLTYPE type_name_position, const string* name,
			const YYLTYPE name_position,
			const Expression* initializer_expression = nullptr);
	virtual ~StructInstantiationStatement();

	virtual const string* GetName() const {
		return m_name;
	}

	const YYLTYPE GetNamePosition() const {
		return m_name_position;
	}

	const TypeSpecifier* GetType() const;

	const YYLTYPE GetTypeNamePosition() const {
		return m_type_name_position;
	}

	virtual const LinkedList<const Error*>* preprocess(
			const ExecutionContext* execution_context) const;

	virtual const LinkedList<const Error*>* execute(
			ExecutionContext* execution_context) const;

	virtual const Expression* GetInitializerExpression() const {
		return m_initializer_expression;
	}

	virtual const DeclarationStatement* WithInitializerExpression(
			const Expression* expression) const;

	virtual const AnalysisResult Returns(const TypeSpecifier* type_specifier) const {
		return AnalysisResult::NO;
	}

private:
	const CompoundTypeSpecifier* m_type_specifier;
	const YYLTYPE m_type_name_position;
	const string* m_name;
	const YYLTYPE m_name_position;
	const Expression* m_initializer_expression;
};

#endif /* STATEMENTS_STRUCT_INSTANTIATION_STATEMENT_H_ */

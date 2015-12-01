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

#include <function_type_specifier.h>
#include <typeinfo>
#include <declaration_statement.h>
#include <sstream>
#include <type_list.h>
#include <assert.h>
#include <type_list.h>
#include <function_declaration_statement.h>
#include <function_expression.h>
#include <constant_expression.h>
#include <return_statement.h>
#include <statement_list.h>
#include <statement_block.h>
#include <function.h>
#include <function_declaration.h>
#include <execution_context.h>

FunctionTypeSpecifier::FunctionTypeSpecifier(
		const TypeList* parameter_type_list, const TypeSpecifier* return_type) :
		m_parameter_type_list(parameter_type_list), m_return_type(return_type) {
}

FunctionTypeSpecifier::~FunctionTypeSpecifier() {
}

const string FunctionTypeSpecifier::ToString() const {
	ostringstream buffer;
	buffer << "(";
	const LinkedList<const TypeSpecifier*>* subject = m_parameter_type_list;
	while (!subject->IsTerminator()) {
		const TypeSpecifier* type = subject->GetData();
		buffer << type->ToString();
		subject = subject->GetNext();

		if (!subject->IsTerminator()) {
			//add separator
			buffer << ", ";
		}
	}
	buffer << ") -> " << m_return_type->ToString() << "";
	return buffer.str();
}

const bool FunctionTypeSpecifier::IsAssignableTo(
		const TypeSpecifier* other) const {
	return *this == *other;
}

const void* FunctionTypeSpecifier::DefaultValue(
		const TypeTable* type_table) const {
	//return a function that returns the default value of the return type
	auto statement_block = GetDefaultStatementBlock(type_table);

	const FunctionDeclaration* declaration =
			FunctionDeclaration::FromTypeSpecifier(this);
	return new Function(declaration, statement_block,
			ExecutionContext::GetDefault());
}

bool FunctionTypeSpecifier::operator ==(const TypeSpecifier& other) const {
	try {
		const FunctionTypeSpecifier& as_function =
				dynamic_cast<const FunctionTypeSpecifier&>(other);
		if (*m_return_type == *as_function.GetReturnType()) {
			const LinkedList<const TypeSpecifier*>* subject =
					m_parameter_type_list;
			const LinkedList<const TypeSpecifier*>* other_subject =
					as_function.GetParameterTypeList();
			while (!subject->IsTerminator()) {
				const TypeSpecifier* type = subject->GetData();
				const TypeSpecifier* other_type = other_subject->GetData();
				if (*type == *other_type) {
					subject = subject->GetNext();
					other_subject = other_subject->GetNext();
				} else {
					return false;
				}
			}

			return true;
		} else {
			return false;
		}
	} catch (std::bad_cast& e) {
		return false;
	}
}

const DeclarationStatement* FunctionTypeSpecifier::GetDeclarationStatement(
		const YYLTYPE position, const YYLTYPE type_position,
		const std::string* name, const YYLTYPE name_position,
		const Expression* initializer_expression) const {
	return new FunctionDeclarationStatement(position, this, type_position, name,
			name_position, initializer_expression);
}

const StatementBlock* FunctionTypeSpecifier::GetDefaultStatementBlock(
		const TypeTable* type_table) const {
	const ConstantExpression* return_expression =
			ConstantExpression::GetDefaultExpression(m_return_type, type_table);
	const ReturnStatement* default_return_statement = new ReturnStatement(
			return_expression);
	const StatementList* default_list = new StatementList(
			default_return_statement, StatementList::Terminator);
	const StatementBlock* statement_block = new StatementBlock(default_list);

	return statement_block;
}

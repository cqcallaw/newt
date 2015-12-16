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

#include <function_declaration.h>
#include <linked_list.h>
#include <declaration_list.h>
#include <sstream>
#include <defaults.h>
#include <type_list.h>
#include <function_declaration_statement.h>
#include <function_expression.h>
#include <constant_expression.h>
#include <return_statement.h>
#include <statement_list.h>
#include <statement_block.h>
#include <function.h>
#include <execution_context.h>

FunctionDeclaration::FunctionDeclaration(const DeclarationList* parameter_list,
		const TypeSpecifier* return_type) :
		FunctionTypeSpecifier(parameter_list->GetTypeList(), return_type), m_parameter_list(
				parameter_list) {
}

FunctionDeclaration::~FunctionDeclaration() {
}

const FunctionDeclaration* FunctionDeclaration::FromTypeSpecifier(
		const FunctionTypeSpecifier* type_specifier) {
	const LinkedList<const TypeSpecifier*>* subject =
			type_specifier->GetParameterTypeList();
	const LinkedList<const DeclarationStatement*>* result =
			DeclarationList::GetTerminator();
	int count = 1;
	while (!subject->IsTerminator()) {
		ostringstream buf;
		const TypeSpecifier* data = subject->GetData();
		buf << "Arg" << count;
		result = result->With(
				data->GetDeclarationStatement(DefaultLocation, DefaultLocation,
						new string(buf.str()), DefaultLocation, nullptr));
		count++;
		subject = subject->GetNext();
	}

	const DeclarationList* declaration_list = new DeclarationList(
			result->Reverse(true));

	return new FunctionDeclaration(declaration_list,
			type_specifier->GetReturnType());
}

const void* FunctionDeclaration::DefaultValue(
		const TypeTable* type_table) const {
	//return a function that returns the default value of the return type
	const ConstantExpression* return_expression =
			ConstantExpression::GetDefaultExpression(GetReturnType(),
					type_table);
	const ReturnStatement* default_return_statement = new ReturnStatement(
			return_expression);
	const StatementList* default_list = new StatementList(
			default_return_statement, StatementList::GetTerminator());
	const StatementBlock* statement_block = new StatementBlock(default_list);

	return new Function(this, statement_block, ExecutionContext::GetDefault());
}

const DeclarationStatement* FunctionDeclaration::GetDeclarationStatement(
		const YYLTYPE position, const YYLTYPE type_position,
		const std::string* name, const YYLTYPE name_position,
		const Expression* initializer_expression) const {
	return new FunctionDeclarationStatement(position, this, type_position, name,
			name_position, initializer_expression);
}

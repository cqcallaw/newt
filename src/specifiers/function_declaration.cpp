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
#include <sstream>
#include <defaults.h>
#include <function_declaration_statement.h>
#include <function_expression.h>
#include <constant_expression.h>
#include <return_statement.h>
#include <statement_block.h>
#include <function.h>
#include <execution_context.h>

FunctionDeclaration::FunctionDeclaration(DeclarationListRef parameter_list,
		const_shared_ptr<TypeSpecifier> return_type,
		const yy::location return_type_location) :
		FunctionTypeSpecifier(GetTypeList(parameter_list), return_type,
				return_type_location), m_parameter_list(parameter_list) {
}

FunctionDeclaration::FunctionDeclaration(const FunctionDeclaration& other) :
		FunctionTypeSpecifier(other.GetParameterTypeList(),
				other.GetReturnType(), other.GetReturnTypeLocation()), m_parameter_list(
				other.m_parameter_list) {
}

FunctionDeclaration::~FunctionDeclaration() {
}

const_shared_ptr<FunctionDeclaration> FunctionDeclaration::FromTypeSpecifier(
		const FunctionTypeSpecifier& type_specifier) {
	TypeSpecifierListRef subject = type_specifier.GetParameterTypeList();
	DeclarationListRef result = DeclarationList::GetTerminator();
	int count = 1;
	while (!TypeSpecifierList::IsTerminator(subject)) {
		ostringstream buf;
		const_shared_ptr<TypeSpecifier> data = subject->GetData();
		buf << "Arg" << count;
		result = DeclarationList::From(
				data->GetDeclarationStatement(GetDefaultLocation(), data,
						GetDefaultLocation(),
						const_shared_ptr<string>(new string(buf.str())),
						GetDefaultLocation(), nullptr), result);
		count++;
		subject = subject->GetNext();
	}

	DeclarationListRef declaration_list = DeclarationList::Reverse(result);

	return make_shared<FunctionDeclaration>(declaration_list,
			type_specifier.GetReturnType(),
			type_specifier.GetReturnTypeLocation());
}

const_shared_ptr<void> FunctionDeclaration::DefaultValue(
		const TypeTable& type_table) const {
	return GetDefaultFunctionDeclaration(*this, type_table);
}

const_shared_ptr<DeclarationStatement> FunctionDeclaration::GetDeclarationStatement(
		const yy::location position, const_shared_ptr<TypeSpecifier> type,
		const yy::location type_position, const_shared_ptr<string> name,
		const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) const {
	return make_shared<FunctionDeclarationStatement>(position,
			static_pointer_cast<const FunctionTypeSpecifier>(type),
			type_position, name, name_position, initializer_expression);
}

const_shared_ptr<Function> FunctionDeclaration::GetDefaultFunctionDeclaration(
		const FunctionDeclaration& function_declaration,
		const TypeTable& type_table) {
	auto statement_block = GetDefaultStatementBlock(
			function_declaration.GetReturnType(), type_table);

	return make_shared<const Function>(
			make_shared<FunctionDeclaration>(function_declaration),
			statement_block, ExecutionContext::GetDefault());
}

TypeSpecifierListRef FunctionDeclaration::GetTypeList(
		DeclarationListRef parameter_list) {
	DeclarationListRef subject = parameter_list;
	TypeSpecifierListRef result = TypeSpecifierList::GetTerminator();
	while (!DeclarationList::IsTerminator(subject)) {
		const_shared_ptr<DeclarationStatement> statement = subject->GetData();
		const_shared_ptr<TypeSpecifier> type = statement->GetType();
		result = TypeSpecifierList::From(type, result);
		subject = subject->GetNext();
	}

	return TypeSpecifierList::Reverse(result);
}

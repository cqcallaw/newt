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

FunctionDeclaration::FunctionDeclaration(DeclarationList parameter_list,
		const_shared_ptr<TypeSpecifier> return_type) :
		FunctionTypeSpecifier(GetTypeList(parameter_list), return_type), m_parameter_list(
				parameter_list) {
}

FunctionDeclaration::FunctionDeclaration(const FunctionDeclaration& other) :
		FunctionTypeSpecifier(other.GetParameterTypeList(),
				other.GetReturnType()), m_parameter_list(other.m_parameter_list) {
}

FunctionDeclaration::~FunctionDeclaration() {
}

const_shared_ptr<FunctionDeclaration> FunctionDeclaration::FromTypeSpecifier(
		const FunctionTypeSpecifier& type_specifier) {
	TypeSpecifierList subject = type_specifier.GetParameterTypeList();
	DeclarationList result = DeclarationListBase::GetTerminator();
	int count = 1;
	while (!TypeSpecifierListBase::IsTerminator(subject)) {
		ostringstream buf;
		const_shared_ptr<TypeSpecifier> data = subject->GetData();
		buf << "Arg" << count;
		result = DeclarationListBase::From(
				data->GetDeclarationStatement(GetDefaultLocation(), data,
						GetDefaultLocation(),
						const_shared_ptr<string>(new string(buf.str())),
						GetDefaultLocation(), nullptr), result);
		count++;
		subject = subject->GetNext();
	}

	DeclarationList declaration_list = DeclarationListBase::Reverse(result);

	return const_shared_ptr<FunctionDeclaration>(
			new FunctionDeclaration(declaration_list,
					type_specifier.GetReturnType()));
}

const_shared_ptr<void> FunctionDeclaration::DefaultValue(
		const TypeTable& type_table) const {
	const Function* default_value = GetDefaultFunctionDeclaration(*this,
			type_table);
	return const_shared_ptr<void>(default_value);
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

const Function* FunctionDeclaration::GetDefaultFunctionDeclaration(
		const FunctionDeclaration& function_declaration,
		const TypeTable& type_table) {
	auto statement_block = GetDefaultStatementBlock(
			function_declaration.GetReturnType(), type_table);

	return new Function(
			const_shared_ptr<FunctionDeclaration>(
					new FunctionDeclaration(function_declaration)),
			statement_block, ExecutionContext::GetDefault());
}

TypeSpecifierList FunctionDeclaration::GetTypeList(
		DeclarationList parameter_list) {
	DeclarationList subject = parameter_list;
	TypeSpecifierList result = TypeSpecifierListBase::GetTerminator();
	while (!DeclarationListBase::IsTerminator(subject)) {
		const_shared_ptr<DeclarationStatement> statement = subject->GetData();
		const_shared_ptr<TypeSpecifier> type = statement->GetType();
		result = TypeSpecifierListBase::From(type, result);
		subject = subject->GetNext();
	}

	return TypeSpecifierListBase::Reverse(result);
}

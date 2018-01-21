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
#include <function_expression.h>
#include <constant_expression.h>
#include <function_declaration_statement.h>
#include <return_statement.h>
#include <statement_block.h>
#include <function.h>
#include <execution_context.h>
#include <function_type.h>

FunctionDeclaration::FunctionDeclaration(DeclarationListRef parameter_list,
		TypeSpecifierListRef type_parameter_list,
		const_shared_ptr<TypeSpecifier> return_type,
		const yy::location location) :
		FunctionTypeSpecifier(GetTypeList(parameter_list), type_parameter_list,
				return_type, location), m_parameter_list(parameter_list) {
}

FunctionDeclaration::FunctionDeclaration(const FunctionDeclaration& other) :
		FunctionTypeSpecifier(other.GetParameterTypeList(),
				other.GetTypeParameterList(), other.GetReturnTypeSpecifier(),
				other.GetLocation()), m_parameter_list(other.m_parameter_list) {
}

FunctionDeclaration::~FunctionDeclaration() {
}

const_shared_ptr<Result> FunctionDeclaration::FromTypeSpecifier(
		const FunctionTypeSpecifier& type_specifier,
		const TypeTable& type_table) {
	ErrorListRef errors = ErrorList::GetTerminator();

	TypeSpecifierListRef subject = type_specifier.GetParameterTypeList();
	DeclarationListRef result = DeclarationList::GetTerminator();
	int count = 1;
	while (!TypeSpecifierList::IsTerminator(subject)) {
		ostringstream buf;
		const_shared_ptr<TypeSpecifier> data = subject->GetData();
		buf << "Arg" << count;
		auto parameter_type_result = data->GetType(type_table);

		auto parameter_errors = parameter_type_result->GetErrors();
		if (ErrorList::IsTerminator(parameter_errors)) {
			auto parameter_type =
					parameter_type_result->GetData<TypeDefinition>();
			result = DeclarationList::From(
					parameter_type->GetDeclarationStatement(
							GetDefaultLocation(), data, GetDefaultLocation(),
							const_shared_ptr<string>(new string(buf.str())),
							GetDefaultLocation(), nullptr), result);
		} else {
			errors = ErrorList::Concatenate(errors, parameter_errors);
		}
		count++;
		subject = subject->GetNext();
	}

	plain_shared_ptr<FunctionDeclaration> declaration = nullptr;
	if (ErrorList::IsTerminator(errors)) {
		DeclarationListRef declaration_list = DeclarationList::Reverse(result);
		declaration = make_shared<FunctionDeclaration>(declaration_list,
				type_specifier.GetTypeParameterList(),
				type_specifier.GetReturnTypeSpecifier(),
				type_specifier.GetLocation());
	}

	return make_shared<Result>(declaration, errors);
}

TypeSpecifierListRef FunctionDeclaration::GetTypeList(
		DeclarationListRef parameter_list) {
	DeclarationListRef subject = parameter_list;
	TypeSpecifierListRef result = TypeSpecifierList::GetTerminator();
	while (!DeclarationList::IsTerminator(subject)) {
		const_shared_ptr<DeclarationStatement> statement = subject->GetData();
		auto type_specifier = statement->GetTypeSpecifier();

		result = TypeSpecifierList::From(type_specifier, result);
		subject = subject->GetNext();
	}

	return TypeSpecifierList::Reverse(result);
}

//TODO: re-enable this
//const string FunctionDeclaration::ToString() const {
//	ostringstream buffer;
//	buffer << "(";
//	DeclarationListRef subject = m_parameter_list;
//	while (!DeclarationList::IsTerminator(subject)) {
//		const_shared_ptr<DeclarationStatement> declaration = subject->GetData();
//		buffer << *declaration->GetName() << ":"
//				<< declaration->GetType()->ToString();
//		subject = subject->GetNext();
//
//		if (!DeclarationList::IsTerminator(subject)) {
//			//add separator
//			buffer << ", ";
//		}
//	}
//	buffer << ") -> " << GetReturnType()->ToString() << "";
//	return buffer.str();
//}

const_shared_ptr<Result> FunctionDeclaration::GetType(
		const TypeTable& type_table, AliasResolution resolution) const {
	auto result = make_shared<const FunctionType>(m_parameter_list,
			GetTypeParameterList(), GetReturnTypeSpecifier());

	return make_shared<Result>(result, ErrorList::GetTerminator());
}

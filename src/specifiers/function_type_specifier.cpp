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
#include <assert.h>
#include <function_declaration_statement.h>
#include <function_expression.h>
#include <constant_expression.h>
#include <return_statement.h>
#include <statement_block.h>
#include <function.h>
#include <function_declaration.h>
#include <execution_context.h>

FunctionTypeSpecifier::FunctionTypeSpecifier(
		TypeSpecifierList parameter_type_list,
		const_shared_ptr<TypeSpecifier> return_type) :
		m_parameter_type_list(parameter_type_list), m_return_type(return_type) {
}

FunctionTypeSpecifier::FunctionTypeSpecifier(const FunctionTypeSpecifier& other) :
		m_parameter_type_list(other.m_parameter_type_list), m_return_type(
				other.m_return_type) {
}

FunctionTypeSpecifier::~FunctionTypeSpecifier() {
}

const string FunctionTypeSpecifier::ToString() const {
	ostringstream buffer;
	buffer << "(";
	TypeSpecifierList subject = m_parameter_type_list;
	while (!TypeSpecifierListBase::IsTerminator(subject)) {
		const_shared_ptr<TypeSpecifier> type = subject->GetData();
		buffer << type->ToString();
		subject = subject->GetNext();

		if (!TypeSpecifierListBase::IsTerminator(subject)) {
			//add separator
			buffer << ", ";
		}
	}
	buffer << ") -> " << m_return_type->ToString() << "";
	return buffer.str();
}

const bool FunctionTypeSpecifier::IsAssignableTo(
		const_shared_ptr<TypeSpecifier> other) const {
	return *this == *other;
}

const_shared_ptr<void> FunctionTypeSpecifier::DefaultValue(
		const TypeTable& type_table) const {
	const static const_shared_ptr<void> default_value = const_shared_ptr<void>(
			GetDefaultFunction(*this, type_table));
	return default_value;
}

bool FunctionTypeSpecifier::operator ==(const TypeSpecifier& other) const {
	try {
		const FunctionTypeSpecifier& as_function =
				dynamic_cast<const FunctionTypeSpecifier&>(other);
		if (*m_return_type == *as_function.GetReturnType()) {
			TypeSpecifierList subject = m_parameter_type_list;
			TypeSpecifierList other_subject =
					as_function.GetParameterTypeList();
			while (!TypeSpecifierListBase::IsTerminator(subject)) {
				const_shared_ptr<TypeSpecifier> type = subject->GetData();
				const_shared_ptr<TypeSpecifier> other_type =
						other_subject->GetData();
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

const_shared_ptr<DeclarationStatement> FunctionTypeSpecifier::GetDeclarationStatement(
		const yy::location position, const_shared_ptr<TypeSpecifier> type,
		const yy::location type_position, const_shared_ptr<string> name,
		const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) const {
	return make_shared<FunctionDeclarationStatement>(position,
			static_pointer_cast<const FunctionTypeSpecifier>(type),
			type_position, name, name_position, initializer_expression);
}

const Function* FunctionTypeSpecifier::GetDefaultFunction(
		const FunctionTypeSpecifier& type_specifier,
		const TypeTable& type_table) {
	auto statement_block = GetDefaultStatementBlock(
			type_specifier.m_return_type, type_table);
	auto declaration = FunctionDeclaration::FromTypeSpecifier(type_specifier);
	return new Function(declaration, statement_block,
			ExecutionContext::GetDefault());
}

const_shared_ptr<StatementBlock> FunctionTypeSpecifier::GetDefaultStatementBlock(
		const_shared_ptr<TypeSpecifier> return_type,
		const TypeTable& type_table) {
	//return a function that returns the default value of the return type
	const_shared_ptr<ConstantExpression> return_expression =
			ConstantExpression::GetDefaultExpression(return_type, type_table);
	const_shared_ptr<ReturnStatement> default_return_statement = make_shared<
			ReturnStatement>(return_expression);
	const StatementList default_list = StatementListBase::From(
			default_return_statement, StatementListBase::GetTerminator());

	const_shared_ptr<StatementBlock> statement_block = make_shared<
			StatementBlock>(default_list);

	return statement_block;
}

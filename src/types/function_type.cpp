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
#include <function_declaration.h>
#include <function_type.h>
#include <function.h>
#include <assert.h>
#include <function_declaration_statement.h>
#include <constant_expression.h>
#include <declaration_statement.h>
#include <function_declaration_statement.h>
#include <statement_block.h>
#include <execution_context.h>
#include <return_statement.h>

const_shared_ptr<void> FunctionType::GetDefaultValue(
		const TypeTable& type_table) const {
	const static const_shared_ptr<void> default_value = GetDefaultFunction(
			*m_type_specifier, type_table);
	return default_value;
}

const std::string FunctionType::ToString(const TypeTable& type_table,
		const Indent& indent) const {
	return m_type_specifier->ToString();
}

const std::string FunctionType::ValueToString(const TypeTable& type_table,
		const Indent& indent, const_shared_ptr<void> value) const {
	auto as_function = static_pointer_cast<const Function>(value);
	return as_function->ToString(type_table, Indent(0));
}

const_shared_ptr<TypeSpecifier> FunctionType::GetTypeSpecifier(
		const_shared_ptr<std::string> name) const {
	return m_type_specifier;
}

bool FunctionType::IsSpecifiedBy(const std::string& name,
		const TypeSpecifier& type_specifier) const {
	return *m_type_specifier == type_specifier;
}

const std::string FunctionType::GetValueSeperator(const Indent& indent,
		const_shared_ptr<void> value) const {
	return "\n" + (indent + 1).ToString();
}

const_shared_ptr<Symbol> FunctionType::GetSymbol(
		const_shared_ptr<TypeSpecifier> type_specifier,
		const_shared_ptr<void> value) const {
	return make_shared<Symbol>(static_pointer_cast<const Function>(value));
}

const_shared_ptr<DeclarationStatement> FunctionType::GetDeclarationStatement(
		const yy::location position, const_shared_ptr<TypeSpecifier> type,
		const yy::location type_position, const_shared_ptr<std::string> name,
		const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) const {
	return make_shared<FunctionDeclarationStatement>(position,
			static_pointer_cast<const FunctionTypeSpecifier>(type),
			type_position, name, name_position, initializer_expression);
}

const_shared_ptr<Function> FunctionType::GetDefaultFunction(
		const FunctionTypeSpecifier& type_specifier,
		const TypeTable& type_table) {
	auto statement_block = GetDefaultStatementBlock(
			type_specifier.GetReturnType(), type_table);
	auto declaration = FunctionDeclaration::FromTypeSpecifier(type_specifier,
			type_table);
	return make_shared<Function>(declaration, statement_block,
			ExecutionContext::GetDefault());
}

const_shared_ptr<StatementBlock> FunctionType::GetDefaultStatementBlock(
		const_shared_ptr<TypeSpecifier> return_type,
		const TypeTable& type_table) {
	//return a function that returns the default value of the return type
	const_shared_ptr<ConstantExpression> return_expression =
			ConstantExpression::GetDefaultExpression(return_type, type_table);
	const_shared_ptr<ReturnStatement> default_return_statement = make_shared<
			ReturnStatement>(return_expression);
	const StatementListRef default_list = StatementList::From(
			default_return_statement, StatementList::GetTerminator());

	const_shared_ptr<StatementBlock> statement_block = make_shared<
			StatementBlock>(default_list, GetDefaultLocation());

	return statement_block;
}


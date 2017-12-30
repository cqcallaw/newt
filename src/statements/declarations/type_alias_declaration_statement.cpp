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

#include <type_alias_declaration_statement.h>
#include <statement_block.h>
#include <primitive_declaration_statement.h>
#include <return_statement.h>
#include <alias_definition.h>
#include <primitive_type_specifier.h>
#include <function_declaration.h>
#include <variable_expression.h>
#include <basic_variable.h>
#include <function.h>

TypeAliasDeclarationStatement::TypeAliasDeclarationStatement(
		const yy::location position,
		const_shared_ptr<TypeSpecifier> type_specifier,
		const yy::location type_specifier_location,
		const_shared_ptr<string> name, const yy::location name_position) :
		DeclarationStatement(position, name, name_position,
				const_shared_ptr<Expression>(), ModifierList::GetTerminator(),
				GetDefaultLocation()), m_type_specifier(type_specifier), m_type_specifier_location(
				type_specifier_location) {
}

TypeAliasDeclarationStatement::~TypeAliasDeclarationStatement() {
}

const PreprocessResult TypeAliasDeclarationStatement::Preprocess(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure,
		const_shared_ptr<TypeSpecifier> return_type_specifier) const {
	//no-op
	return PreprocessResult(PreprocessResult::ReturnCoverage::NONE,
			ErrorList::GetTerminator());
}

const ExecutionResult TypeAliasDeclarationStatement::Execute(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	//no-op
	return ExecutionResult();
}

const DeclarationStatement* TypeAliasDeclarationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
	//no-op
	return new TypeAliasDeclarationStatement(GetLocation(), m_type_specifier,
			m_type_specifier_location, GetName(), GetNameLocation());
}

TypedResult<Symbol> TypeAliasDeclarationStatement::GetTypeConstructor(
		//const_shared_ptr<TypeAliasDeclarationStatement> alias_declaration,
		const yy::location alias_declaration_location,
		const_shared_ptr<string> alias_name,
		const yy::location alias_name_location,
		const yy::location alias_type_specifier_location,
		const_shared_ptr<PrimitiveTypeSpecifier> primitive_type_specifier,
		const_shared_ptr<ComplexTypeSpecifier> result_type_specifier,
		const_shared_ptr<AliasDefinition> alias,
		const shared_ptr<ExecutionContext> output,
		const shared_ptr<ExecutionContext> closure) {
	auto errors = ErrorList::GetTerminator();
	auto symbol = Symbol::GetDefaultSymbol();

	const_shared_ptr<PrimitiveDeclarationStatement> parameter_declaration =
			make_shared<PrimitiveDeclarationStatement>(
					alias_declaration_location, primitive_type_specifier,
					alias_type_specifier_location,
					alias_name,
					alias_name_location);
	DeclarationListRef parameter = DeclarationList::From(parameter_declaration,
			DeclarationList::GetTerminator());

	auto function_signature = make_shared<FunctionDeclaration>(parameter,
			result_type_specifier, GetDefaultLocation());

	const_shared_ptr<Expression> return_expression = make_shared<
			VariableExpression>(GetDefaultLocation(),
			make_shared<BasicVariable>(alias_name,
					GetDefaultLocation()));

	const_shared_ptr<ReturnStatement> return_statement = make_shared<
			ReturnStatement>(return_expression);

	const StatementListRef statement_list = StatementList::From(
			return_statement, StatementList::GetTerminator());
	const_shared_ptr<StatementBlock> statement_block = make_shared<
			StatementBlock>(statement_list,
					alias_name_location);

	auto weak = weak_ptr<ExecutionContext>(output);

	const_shared_ptr<Function> function = Function::Build(GetDefaultLocation(),
			function_signature, statement_block, weak);

	auto variant_context = function->GetVariantList()->GetData()->GetContext();

	auto parameter_preprocess_result = parameter_declaration->Preprocess(
			variant_context, closure, result_type_specifier);
	auto parameter_preprocess_errors = parameter_preprocess_result.GetErrors();
	if (ErrorList::IsTerminator(parameter_preprocess_errors)) {
		auto parameter_execute_errors = parameter_declaration->Execute(
				variant_context, closure).GetErrors();
		if (ErrorList::IsTerminator(parameter_execute_errors)) {
			symbol = make_shared<Symbol>(function);
		} else {
			errors = ErrorList::Concatenate(errors, parameter_execute_errors);
		}
	} else {
		errors = ErrorList::Concatenate(errors, parameter_preprocess_errors);
	}

	return TypedResult<Symbol>(symbol, errors);
}

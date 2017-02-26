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

#include <assert.h>
#include <expression.h>
#include <array_declaration_statement.h>
#include <complex_instantiation_statement.h>
#include <function_declaration_statement.h>
#include <inferred_declaration_statement.h>
#include <primitive_declaration_statement.h>
#include <function_type_specifier.h>
#include <function_expression.h>
#include <specifiers/type_specifier.h>
#include <execution_context.h>

InferredDeclarationStatement::InferredDeclarationStatement(
		const yy::location position, const_shared_ptr<string> name,
		const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) :
		DeclarationStatement(position, name, name_position,
				initializer_expression, ModifierList::GetTerminator(),
				GetDefaultLocation()) {
	assert(initializer_expression);
}

InferredDeclarationStatement::~InferredDeclarationStatement() {
}

const_shared_ptr<TypeSpecifier> InferredDeclarationStatement::GetTypeSpecifier() const {
	if (GetInitializerExpression()->IsConstant()) {
		auto initializer_type_specifier_result =
				GetInitializerExpression()->GetTypeSpecifier(nullptr);

		if (ErrorList::IsTerminator(
				initializer_type_specifier_result.GetErrors())) {
			return initializer_type_specifier_result.GetData();
		}
	}

	return PrimitiveTypeSpecifier::GetNone();
}

const yy::location InferredDeclarationStatement::GetTypeSpecifierLocation() const {
	return GetInitializerExpression()->GetLocation();
}

const PreprocessResult InferredDeclarationStatement::Preprocess(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure,
		const_shared_ptr<TypeSpecifier> return_type_specifier) const {
	auto expression_type_specifier_result =
			GetInitializerExpression()->GetTypeSpecifier(context,
					AliasResolution::RESOLVE);

	auto errors = expression_type_specifier_result.GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto expression_type_specifier =
				expression_type_specifier_result.GetData();

		assert(expression_type_specifier != PrimitiveTypeSpecifier::GetNone());

		auto type_table = context->GetTypeTable();
		auto type_result = expression_type_specifier->GetType(type_table);
		errors = ErrorList::Concatenate(errors, type_result->GetErrors());
		if (ErrorList::IsTerminator(errors)) {
			auto type = type_result->GetData<TypeDefinition>();
			const_shared_ptr<Statement> temp_statement =
					type->GetDeclarationStatement(GetLocation(),
							expression_type_specifier,
							GetInitializerExpression()->GetLocation(),
							GetName(), GetNameLocation(),
							GetInitializerExpression());
			auto preprocess_result = temp_statement->Preprocess(context,
					closure, return_type_specifier);
			errors = preprocess_result.GetErrors();
		}
	}

	return PreprocessResult(PreprocessResult::ReturnCoverage::NONE, errors);
}

const ErrorListRef InferredDeclarationStatement::Execute(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	auto expression_type_specifier_result =
			GetInitializerExpression()->GetTypeSpecifier(context);

	auto errors = expression_type_specifier_result.GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto expression_type_specifier =
				expression_type_specifier_result.GetData();
		auto type_table = context->GetTypeTable();
		auto type_result = expression_type_specifier->GetType(type_table);

		auto type_errors = type_result->GetErrors();
		if (ErrorList::IsTerminator(type_errors)) {
			auto type = type_result->GetData<TypeDefinition>();
			const_shared_ptr<Statement> temp_statement =
					type->GetDeclarationStatement(GetLocation(),
							expression_type_specifier,
							GetInitializerExpression()->GetLocation(),
							GetName(), GetNameLocation(),
							GetInitializerExpression());
			errors = temp_statement->Execute(context, closure);
		} else {
			errors = type_errors;
			auto initializer_errors = GetInitializerExpression()->Validate(
					context);
			errors = ErrorList::Concatenate(errors, initializer_errors);
		}
	}

	return errors;
}

const DeclarationStatement* InferredDeclarationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
	return new InferredDeclarationStatement(GetLocation(), GetName(),
			GetNameLocation(), expression);
}

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
#include <function_declaration.h>
#include <function_expression.h>
#include <expression.h>
#include <basic_variable.h>
#include <assignment_statement.h>
#include <function_declaration_statement.h>
#include <execution_context.h>
#include <function.h>
#include <variant_function_specifier.h>

FunctionDeclarationStatement::FunctionDeclarationStatement(
		const yy::location location,
		const_shared_ptr<FunctionTypeSpecifier> type_specifier,
		const_shared_ptr<string> name, const yy::location name_location,
		const_shared_ptr<Expression> initializer_expression) :
		DeclarationStatement(location, name, name_location,
				initializer_expression, ModifierList::GetTerminator(),
				GetDefaultLocation()), m_type_specifier(type_specifier) {
}

FunctionDeclarationStatement::FunctionDeclarationStatement(
		const yy::location location,
		const_shared_ptr<VariantFunctionSpecifier> type_specifier,
		const_shared_ptr<string> name, const yy::location name_location,
		const_shared_ptr<Expression> initializer_expression) :
		DeclarationStatement(location, name, name_location,
				initializer_expression, ModifierList::GetTerminator(),
				GetDefaultLocation()), m_type_specifier(type_specifier) {
}

FunctionDeclarationStatement::FunctionDeclarationStatement(
		const yy::location location,
		const_shared_ptr<TypeSpecifier> type_specifier,
		const_shared_ptr<string> name, const yy::location name_location,
		const_shared_ptr<Expression> initializer_expression) :
		DeclarationStatement(location, name, name_location,
				initializer_expression, ModifierList::GetTerminator(),
				GetDefaultLocation()), m_type_specifier(type_specifier) {
}

FunctionDeclarationStatement::~FunctionDeclarationStatement() {
}

const PreprocessResult FunctionDeclarationStatement::Preprocess(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure,
		const_shared_ptr<TypeSpecifier> return_type_specifier) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	auto type_table = context->GetTypeTable();

	auto existing = context->GetSymbol(GetName(), SHALLOW);
	if (existing == nullptr || existing == Symbol::GetDefaultSymbol()) {
		if (GetInitializerExpression()) {
			auto expression_type_specifier_result =
					GetInitializerExpression()->GetTypeSpecifier(context);

			errors = expression_type_specifier_result.GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				auto expression_type_specifier =
						expression_type_specifier_result.GetData();
				auto as_function = std::dynamic_pointer_cast<
						const FunctionTypeSpecifier>(expression_type_specifier);
				auto as_variant_function = std::dynamic_pointer_cast<
						const VariantFunctionSpecifier>(
						expression_type_specifier);

				if (as_function || as_variant_function) {
					// insert default value to enable validation of recursive invocations
					// we create this default value in a validation context to avoid polluting the real name space
					// the validation context is "TEMPORARY" so that strong references aren't made to it
					auto validation_context = context->GetEmptyChild(context,
							Modifier::Type::MUTABLE, TEMPORARY);
					auto value = m_type_specifier->DefaultValue(*type_table);
					auto symbol = make_shared<Symbol>(
							static_pointer_cast<const Function>(value));
					InsertResult insert_result =
							validation_context->InsertSymbol(*GetName(),
									symbol);
					assert(insert_result == INSERT_SUCCESS);

					errors = GetInitializerExpression()->Validate(
							validation_context);
					if (ErrorList::IsTerminator(errors)) {
						auto assignment_analysis =
								expression_type_specifier->AnalyzeAssignmentTo(
										m_type_specifier, type_table);
						if (assignment_analysis == EQUIVALENT) {
							// passed validation; insert into output context
							InsertResult insert_result = context->InsertSymbol(
									*GetName(), symbol);
							assert(insert_result == INSERT_SUCCESS);
						} else {
							//TODO: consider widening conversions
							errors =
									ErrorList::From(
											make_shared<Error>(Error::SEMANTIC,
													Error::ASSIGNMENT_TYPE_ERROR,
													GetInitializerExpression()->GetLocation().begin.line,
													GetInitializerExpression()->GetLocation().begin.column,
													m_type_specifier->ToString(),
													expression_type_specifier->ToString()),
											errors);
						}
					}
				} else {
					errors =
							ErrorList::From(
									make_shared<Error>(Error::SEMANTIC,
											Error::NOT_A_FUNCTION,
											GetInitializerExpression()->GetLocation().begin.line,
											GetInitializerExpression()->GetLocation().begin.column),
									errors);
				}
			}
		} else {
			// no initializer; initialize to default value
			auto value = m_type_specifier->DefaultValue(*type_table);
			auto as_function = static_pointer_cast<const Function>(value);
			// juggle closure references so we have some valid closure with which to work
			as_function = Function::Build(as_function->GetLocation(),
					as_function->GetVariantList(), closure);

			auto symbol = make_shared<Symbol>(as_function);
			InsertResult insert_result = context->InsertSymbol(*GetName(),
					symbol);
			assert(insert_result == INSERT_SUCCESS);
		}
	} else {
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC, Error::PREVIOUS_DECLARATION,
						GetNameLocation().begin.line,
						GetNameLocation().begin.column, *(GetName())), errors);
	}

	return PreprocessResult(PreprocessResult::ReturnCoverage::NONE, errors);
}

const ErrorListRef FunctionDeclarationStatement::Execute(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	if (GetInitializerExpression()) {
		Variable* temp_variable = new BasicVariable(GetName(),
				GetNameLocation());
		auto errors = temp_variable->AssignValue(context, closure,
				GetInitializerExpression(), AssignmentType::ASSIGN);
		delete (temp_variable);

		return errors;
	} else {
		return ErrorList::GetTerminator();
	}
}

const DeclarationStatement* FunctionDeclarationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
	return new FunctionDeclarationStatement(GetLocation(), m_type_specifier,
			GetName(), GetNameLocation(), expression);
}

const_shared_ptr<TypeSpecifier> FunctionDeclarationStatement::GetTypeSpecifier() const {
	return m_type_specifier;
}

const yy::location FunctionDeclarationStatement::GetTypeSpecifierLocation() const {
	return m_type_specifier->GetLocation();
}

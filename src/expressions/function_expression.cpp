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
#include <function_expression.h>
#include <function.h>
#include <statement_block.h>
#include <execution_context.h>
#include <function_type_specifier.h>
#include <symbol_table.h>
#include <complex_type.h>
#include <declaration_statement.h>
#include <inferred_declaration_statement.h>
#include <variant_function_specifier.h>

FunctionExpression::FunctionExpression(const yy::location location,
		FunctionVariantListRef variant_list) :
		Expression(location), m_variant_list(variant_list) {
}

FunctionExpression::~FunctionExpression() {
}

TypedResult<TypeSpecifier> FunctionExpression::GetTypeSpecifier(
		const shared_ptr<ExecutionContext> execution_context,
		AliasResolution resolution) const {
	if (FunctionVariantList::IsTerminator(m_variant_list->GetNext())) {
		return TypedResult<TypeSpecifier>(
				m_variant_list->GetData()->GetDeclaration(),
				ErrorList::GetTerminator());
	} else {
		// overloaded function
		return TypedResult<TypeSpecifier>(
				make_shared<VariantFunctionSpecifier>(GetLocation(),
						m_variant_list), ErrorList::GetTerminator());
	}
}

const_shared_ptr<Result> FunctionExpression::Evaluate(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	ErrorListRef errors = ErrorList::GetTerminator();
	auto function = Function::Build(GetLocation(), m_variant_list, closure);
	return make_shared<Result>(function, errors);
}

const bool FunctionExpression::IsConstant() const {
	return true;
}

const ErrorListRef FunctionExpression::Validate(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	auto subject = m_variant_list;
	while (!FunctionVariantList::IsTerminator(subject)) {
		auto variant = subject->GetData();
		auto declaration = variant->GetDeclaration();
		auto body = variant->GetBody();

		auto variant_context = variant->GetContext();
		variant_context->LinkToParent(execution_context);

		// check that this variant doesn't conflict with any subsequent definitions
		// for this, we assume the function variants are in order
		auto duplication_subject = subject->GetNext();
		while (!FunctionVariantList::IsTerminator(duplication_subject)) {
			auto duplication_subject_variant = duplication_subject->GetData();
			auto duplication_subject_declaration =
					duplication_subject_variant->GetDeclaration();
			auto assignment_result =
					duplication_subject_declaration->AnalyzeAssignmentTo(
							declaration, execution_context->GetTypeTable());

			if (assignment_result == EQUIVALENT) {
				ostringstream out;
				out << variant->GetLocation();
				errors =
						ErrorList::From(
								make_shared<Error>(Error::SEMANTIC,
										Error::FUNCTION_VARIANT_WITH_DUPLICATE_SIGNATURE,
										duplication_subject_variant->GetLocation().begin.line,
										duplication_subject_variant->GetLocation().begin.column,
										declaration->ToString(), out.str()),
								errors);
			}

			duplication_subject = duplication_subject->GetNext();
		}

		DeclarationListRef parameter_subject = declaration->GetParameterList();
		while (!DeclarationList::IsTerminator(parameter_subject)) {
			auto declaration_statement = parameter_subject->GetData();

			ErrorListRef parameter_errors = ErrorList::GetTerminator();
			//validate the constancy of parameter declarations of an inferred type
			const_shared_ptr<InferredDeclarationStatement> as_inferred =
					dynamic_pointer_cast<const InferredDeclarationStatement>(
							declaration_statement);
			if (as_inferred) {
				auto init_expression = as_inferred->GetInitializerExpression();
				if (!init_expression->IsConstant()) {
					parameter_errors =
							ErrorList::From(
									make_shared<Error>(Error::SEMANTIC,
											Error::FUNCTION_PARAMETER_DEFAULT_MUST_BE_CONSTANT,
											init_expression->GetLocation().begin.line,
											init_expression->GetLocation().begin.column),
									parameter_errors);
				}
			}

			if (ErrorList::IsTerminator(parameter_errors)) {
				auto preprocess_result = declaration_statement->Preprocess(
						variant_context, variant_context);
				auto preprocess_errors = preprocess_result.GetErrors();

				if (ErrorList::IsTerminator(preprocess_errors)) {
					auto execution_errors = declaration_statement->Execute(
							variant_context, variant_context);
					if (!ErrorList::IsTerminator(execution_errors)) {
						parameter_errors = ErrorList::Concatenate(
								parameter_errors, execution_errors);
					}
				} else {
					parameter_errors = ErrorList::Concatenate(parameter_errors,
							preprocess_errors);
				}
			}

			errors = ErrorList::Concatenate(errors, parameter_errors);

			parameter_subject = parameter_subject->GetNext();
		}

		if (ErrorList::IsTerminator(errors)) {
			auto return_type_specifier = declaration->GetReturnTypeSpecifier();
			auto body_process_result = body->Preprocess(variant_context,
					return_type_specifier);
			errors = ErrorList::Concatenate(errors,
					body_process_result.GetErrors());

			if (ErrorList::IsTerminator(errors)
					&& return_type_specifier->AnalyzeAssignmentTo(
							TypeTable::GetNilTypeSpecifier(),
							execution_context->GetTypeTable()) != EQUIVALENT) {
				// we have a non-nil return type and no other errors; check return coverage
				if (body_process_result.GetReturnCoverage()
						!= PreprocessResult::ReturnCoverage::FULL) {
					errors = ErrorList::From(
							make_shared<Error>(Error::SEMANTIC,
									Error::MISSING_RETURN_COVERAGE,
									GetLocation().end.line,
									GetLocation().end.column), errors);
				}
			}
		}

		subject = subject->GetNext();
	}

	return errors;
}


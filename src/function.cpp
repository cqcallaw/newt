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

#include <function.h>
#include <expression.h>
#include <execution_context.h>
#include <function_declaration.h>
#include <error.h>
#include <statement_block.h>
#include <constant_expression.h>
#include <declaration_statement.h>
#include <defaults.h>
#include <sum_type_specifier.h>
#include <maybe_type_specifier.h>
#include <sum.h>
#include <sum_type.h>
#include <unit_type.h>
#include <basic_variable.h>

Function::Function(const_shared_ptr<FunctionDeclaration> declaration,
		const_shared_ptr<StatementBlock> body,
		const shared_ptr<ExecutionContext> closure) :
		m_declaration(declaration), m_body(body), m_closure(closure), m_weak_closure(
				shared_ptr<ExecutionContext>(nullptr)) {
}

Function::Function(const_shared_ptr<FunctionDeclaration> declaration,
		const_shared_ptr<StatementBlock> body,
		const weak_ptr<ExecutionContext> weak_closure) :
		m_declaration(declaration), m_body(body), m_closure(nullptr), m_weak_closure(
				weak_closure) {
}

Function::~Function() {
}

const_shared_ptr<Result> Function::Evaluate(ArgumentListRef argument_list,
		const shared_ptr<ExecutionContext> invocation_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	auto closure_reference = GetClosureReference();

	assert(closure_reference);

	auto parent_context = SymbolContextList::From(invocation_context,
			invocation_context->GetParent());
	shared_ptr<ExecutionContext> function_execution_context = make_shared<
			ExecutionContext>(Modifier::MUTABLE, parent_context,
			closure_reference->GetTypeTable(), EPHEMERAL);

	//populate evaluation context with results of argument evaluation
	ArgumentListRef argument = argument_list;
	DeclarationListRef parameter = m_declaration->GetParameterList();
	while (!ArgumentList::IsTerminator(argument)) {
		const_shared_ptr<Expression> argument_expression = argument->GetData();
		if (!DeclarationList::IsTerminator(parameter)) {
			const_shared_ptr<DeclarationStatement> parameter_declaration =
					parameter->GetData();

			auto parameter_preprocess_errors =
					parameter_declaration->Preprocess(
							function_execution_context, closure_reference);
			if (ErrorList::IsTerminator(parameter_preprocess_errors)) {
				auto parameter_execute_errors = parameter_declaration->Execute(
						function_execution_context, closure_reference);
				if (ErrorList::IsTerminator(parameter_execute_errors)) {
					auto argument_variable = make_shared<const BasicVariable>(
							parameter_declaration->GetName(),
							argument_expression->GetPosition());

					auto assign_errors = argument_variable->AssignValue(
							invocation_context, closure_reference,
							argument_expression, ASSIGN,
							function_execution_context);
					if (!ErrorList::IsTerminator(assign_errors)) {
						errors = ErrorList::Concatenate(errors, assign_errors);
					}
				} else {
					errors = ErrorList::Concatenate(errors,
							parameter_execute_errors);
				}
			} else {
				errors = ErrorList::Concatenate(errors,
						parameter_preprocess_errors);
			}

			argument = argument->GetNext();
			parameter = parameter->GetNext();
		} else {
			//argument list is longer than parameter list
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::TOO_MANY_ARGUMENTS,
							argument_expression->GetPosition().begin.line,
							argument_expression->GetPosition().begin.column,
							m_declaration->ToString()), errors);
			break;
		}
	}

	//handle any remaining parameter declarations. if any parameter declarations don't have default values, generate an error
	while (!DeclarationList::IsTerminator(parameter)) {
		const_shared_ptr<DeclarationStatement> declaration =
				parameter->GetData();

		if (declaration->GetInitializerExpression()) {
			errors = ErrorList::Concatenate(errors,
					declaration->Preprocess(function_execution_context,
							function_execution_context));
			errors = ErrorList::Concatenate(errors,
					declaration->Execute(function_execution_context,
							function_execution_context));
			parameter = parameter->GetNext();
		} else {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::NO_PARAMETER_DEFAULT,
							declaration->GetLocation().begin.line,
							declaration->GetLocation().begin.column,
							*declaration->GetName()), errors);
			break;
		}
	}

	//juggle the references so the evaluation context is a child of the closure context
	parent_context = SymbolContextList::From(closure_reference,
			closure_reference->GetParent());
	auto final_execution_context = function_execution_context->WithParent(
			parent_context);

	if (ErrorList::IsTerminator(errors)) {
		//performing preprocessing here duplicates work with the function expression processing,
		//but the context setup in the function preprocessing is currently discarded.
		//TODO: consider cloning function expression preprocess context instead of discarding it
		errors = ErrorList::Concatenate(errors,
				m_body->Preprocess(final_execution_context,
						m_declaration->GetReturnTypeSpecifier()));
		if (ErrorList::IsTerminator(errors)) {
			errors = ErrorList::Concatenate(errors,
					m_body->Execute(final_execution_context));
			plain_shared_ptr<Symbol> evaluation_result =
					final_execution_context->GetReturnValue();
			final_execution_context->SetReturnValue(nullptr); //clear return value to avoid reference cycles

			plain_shared_ptr<void> result = evaluation_result->GetValue();
			auto invocation_type_table = invocation_context->GetTypeTable();

			auto return_type_specifier =
					m_declaration->GetReturnTypeSpecifier();
			auto evaluation_result_type = evaluation_result->GetTypeSpecifier();

			auto assignment_analysis =
					evaluation_result_type->AnalyzeAssignmentTo(
							return_type_specifier, invocation_type_table);
			switch (assignment_analysis) {
			case UNAMBIGUOUS:
			case UNAMBIGUOUS_NESTED: {
				//we're returning a narrower type than the return type; perform widening

				auto return_type_result = return_type_specifier->GetType(
						invocation_context->GetTypeTable(), RESOLVE);

				auto return_type_errors = return_type_result->GetErrors();
				if (ErrorList::IsTerminator(return_type_errors)) {
					auto return_type = return_type_result->GetData<
							TypeDefinition>();

					auto as_sum = dynamic_pointer_cast<const SumType>(
							return_type);
					if (as_sum) {
						auto return_type_specifier_as_complex =
								dynamic_pointer_cast<const ComplexTypeSpecifier>(
										return_type_specifier);
						assert(return_type_specifier_as_complex);

						auto as_sum_specifier = SumTypeSpecifier(
								return_type_specifier_as_complex);
						plain_shared_ptr<string> tag =
								as_sum->MapSpecifierToVariant(as_sum_specifier,
										*evaluation_result_type);

						result = make_shared<Sum>(tag,
								evaluation_result->GetValue());
					}

					auto as_maybe_specifier = dynamic_pointer_cast<
							const MaybeTypeSpecifier>(return_type_specifier);
					if (as_maybe_specifier) {
						if (*evaluation_result_type
								== *TypeTable::GetNilTypeSpecifier()) {
							result = make_shared<Sum>(
									MaybeTypeSpecifier::EMPTY_NAME,
									evaluation_result->GetValue());
						} else {
							result = make_shared<Sum>(
									MaybeTypeSpecifier::VARIANT_NAME,
									evaluation_result->GetValue());
						}
					}
					break;
				} else {
					errors = ErrorList::Concatenate(errors, return_type_errors);
				}
			}
			case AMBIGUOUS:
				assert(false); // our semantic analysis should have caught this already
				break;
			case EQUIVALENT:
			default:
				break;
			}

			return make_shared<Result>(result, errors);
		} else {
			return make_shared<Result>(nullptr, errors);
		}
	} else {
		return make_shared<Result>(nullptr, errors);
	}
}

const string Function::ToString(const TypeTable& type_table,
		const Indent& indent) const {
	ostringstream buffer;
	buffer << indent << "Body Location: ";
	if (m_body->GetLocation() != GetDefaultLocation()) {
		buffer << m_body->GetLocation();
	} else {
		buffer << "[default location]";
	}
//	buffer << indent << "Address: " << this << endl;
//	if (m_closure) {
//		buffer << indent << "Strongly referenced closure address: "
//				<< m_closure.get() << endl;
//	} else {
//		buffer << indent << "Weakly referenced closure address: "
//				<< m_weak_closure.lock().get() << endl;
//	}

	return buffer.str();
}

const shared_ptr<ExecutionContext> Function::GetClosureReference() const {
	if (m_closure) {
		return m_closure;
	} else {
		return m_weak_closure.lock();
	}
}

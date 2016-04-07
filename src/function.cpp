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
#include <sum.h>
#include <sum_type.h>


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
			ExecutionContext>(Modifier::NONE, parent_context,
			closure_reference->GetTypeTable(), EPHEMERAL);

	//populate evaluation context with results of argument evaluation
	ArgumentListRef argument = argument_list;
	DeclarationListRef parameter = m_declaration->GetParameterList();
	while (!ArgumentList::IsTerminator(argument)) {
		const_shared_ptr<Expression> argument_expression = argument->GetData();
		if (!DeclarationList::IsTerminator(parameter)) {
			const_shared_ptr<DeclarationStatement> declaration =
					parameter->GetData();

			const_shared_ptr<Result> argument_evaluation =
					ConstantExpression::GetConstantExpression(
							argument_expression, invocation_context);
			auto evaluation_errors = argument_evaluation->GetErrors();
			if (ErrorList::IsTerminator(evaluation_errors)) {
				//generate a declaration statement for the function execution context.
				//it's tempting to just stuff a value into the symbol table,
				//but this simplistic approach ignores widening conversions
				auto evaluated_expression = argument_evaluation->GetData<
						Expression>();
				const DeclarationStatement* argument_declaration =
						declaration->WithInitializerExpression(
								evaluated_expression);

				auto preprocessing_errors = ErrorList::Concatenate(errors,
						argument_declaration->preprocess(
								function_execution_context));
				if (ErrorList::IsTerminator(preprocessing_errors)) {
					errors = ErrorList::Concatenate(errors,
							argument_declaration->execute(
									function_execution_context));
				} else {
					errors = ErrorList::Concatenate(errors,
							preprocessing_errors);
				}

				delete argument_declaration;
			} else {
				errors = ErrorList::Concatenate(errors, evaluation_errors);
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
					declaration->preprocess(function_execution_context));
			errors = ErrorList::Concatenate(errors,
					declaration->execute(function_execution_context));
			parameter = parameter->GetNext();
		} else {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::NO_PARAMETER_DEFAULT,
							declaration->GetPosition().begin.line,
							declaration->GetPosition().begin.column,
							*declaration->GetName()), errors);
			break;
		}
	}

	//juggle the references so the evaluation context is a child of the closure context
	parent_context = SymbolContextList::From(closure_reference,
			closure_reference->GetParent());
	auto final_execution_context = function_execution_context->WithParent(
			parent_context);

	//TODO: determine if it is necessary to merge type tables

	if (ErrorList::IsTerminator(errors)) {
		//performing preprocessing here duplicates work with the function express processing,
		//but the context setup in the function preprocessing is currently discarded.
		//TODO: consider cloning function expression preprocess context instead of discarding it
		errors = ErrorList::Concatenate(errors,
				m_body->preprocess(final_execution_context));
		if (ErrorList::IsTerminator(errors)) {
			errors = ErrorList::Concatenate(errors,
					m_body->execute(final_execution_context));
			plain_shared_ptr<Symbol> evaluation_result =
					final_execution_context->GetReturnValue();
			final_execution_context->SetReturnValue(nullptr); //clear return value to avoid reference cycles

			plain_shared_ptr<void> result = evaluation_result->GetValue();
			auto return_type = ComplexType::ToActualType(
					m_declaration->GetReturnType(),
					*invocation_context->GetTypeTable());

			auto as_sum_specifier =
					dynamic_pointer_cast<const SumTypeSpecifier>(return_type);
			if (as_sum_specifier) {
				auto evaluation_result_type = evaluation_result->GetType();
				if (*as_sum_specifier != *evaluation_result_type) {
					auto sum_type_name = *as_sum_specifier->GetTypeName();
					auto sum_type_definition =
							invocation_context->GetTypeTable()->GetType<SumType>(
									sum_type_name);
					if (sum_type_definition) {
						//we're returning a narrower type than the return type; perform widening
						plain_shared_ptr<string> tag =
								sum_type_definition->MapSpecifierToVariant(
										*evaluation_result_type, sum_type_name);

						result = make_shared<Sum>(tag,
								evaluation_result->GetValue());
					} else {
						errors =
								ErrorList::From(
										make_shared<Error>(Error::SEMANTIC,
												Error::UNDECLARED_TYPE,
												m_declaration->GetReturnTypeLocation().begin.line,
												m_declaration->GetReturnTypeLocation().begin.column,
												sum_type_name), errors);
					}
				}
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

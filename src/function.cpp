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
#include <variant_function_specifier.h>
#include <sum.h>
#include <sum_type.h>
#include <maybe_type.h>
#include <unit_type.h>
#include <basic_variable.h>

const_shared_ptr<Function> Function::Build(const yy::location location,
		FunctionVariantListRef variant_list,
		const shared_ptr<ExecutionContext> closure) {
	switch (closure->GetLifeTime()) {
	case PERSISTENT:
	case TEMPORARY:
	case ROOT: {
		return make_shared<Function>(
				Function(location, variant_list,
						weak_ptr<ExecutionContext>(closure)));
	}
	case EPHEMERAL:
	default: {
		return make_shared<Function>(Function(location, variant_list, closure));
	}
	}
}

const_shared_ptr<Function> Function::Build(const yy::location location,
		const_shared_ptr<FunctionDeclaration> declaration,
		const_shared_ptr<StatementBlock> statement_block,
		const shared_ptr<ExecutionContext> closure) {
	auto variant = make_shared<FunctionVariant>(location, declaration,
			statement_block);
	auto list = FunctionVariantList::From(variant,
			FunctionVariantList::GetTerminator());
	return Build(location, list, closure);
}

const_shared_ptr<Function> Function::Build(const yy::location location,
		const_shared_ptr<FunctionDeclaration> declaration,
		const_shared_ptr<StatementBlock> statement_block,
		const weak_ptr<ExecutionContext> closure) {
	auto variant = make_shared<FunctionVariant>(location, declaration,
			statement_block);
	auto list = FunctionVariantList::From(variant,
			FunctionVariantList::GetTerminator());
	return Build(location, list, closure);
}

const_shared_ptr<Function> Function::Build(const yy::location location,
		FunctionVariantListRef variant_list,
		const weak_ptr<ExecutionContext> closure) {
	return make_shared<Function>(Function(location, variant_list, closure));
}

Function::Function(const yy::location location,
		const FunctionVariantListRef variant_list,
		const shared_ptr<ExecutionContext> closure) :
		m_location(location), m_variant_list(variant_list), m_closure(closure), m_weak_closure(
				shared_ptr<ExecutionContext>(nullptr)) {
}

Function::Function(const yy::location location,
		const FunctionVariantListRef variant_list,
		const weak_ptr<ExecutionContext> weak_closure) :
		m_location(location), m_variant_list(variant_list), m_closure(nullptr), m_weak_closure(
				weak_closure) {
}

Function::~Function() {
}

const_shared_ptr<Result> Function::Evaluate(ArgumentListRef argument_list,
		const yy::location argument_list_location,
		const shared_ptr<ExecutionContext> invocation_context) const {
	auto errors = ErrorList::GetTerminator();
	if (invocation_context->GetDepth() > INVOCATION_DEPTH) {
		std::stringstream ss;
		ss << INVOCATION_DEPTH;
		std::string as_string = ss.str();
		errors = ErrorList::From(
				make_shared<Error>(Error::RUNTIME, Error::MAX_INVOCATION_DEPTH,
						GetLocation().begin, as_string), errors);
		return make_shared<Result>(nullptr, errors);
	}
	auto closure_reference = GetClosureReference();

	assert(closure_reference);

	auto variant_result = GetVariant(argument_list, argument_list_location,
			m_variant_list, invocation_context);
	errors = ErrorList::Concatenate(errors, variant_result.GetErrors());
	if (!ErrorList::IsTerminator(errors)) {
		return make_shared<Result>(nullptr, errors);
	}

	auto variant = variant_result.GetData();
	auto declaration = variant->GetDeclaration();
	auto body = variant->GetBody();
	auto variant_context = variant->GetContext();

	// clone variant context to create the function execution context
	// so repeated invocations of the function will use discrete child contexts
	// this clone is EPHEMERAL so that strong references will be made to it by functions that return functions, e.g. partial applicators
	auto parent_context = ExecutionContextList::From(closure_reference,
			closure_reference->GetParent());
	auto function_execution_context = make_shared<ExecutionContext>(
			variant_context->Clone(), Modifier::MUTABLE, parent_context,
			make_shared<TypeTable>(closure_reference->GetTypeTable()),
			EPHEMERAL, invocation_context->GetDepth() + 1);

	//populate evaluation context with results of argument evaluation
	ArgumentListRef argument = argument_list;
	DeclarationListRef parameter = declaration->GetParameterList();
	while (!ArgumentList::IsTerminator(argument)) {
		const_shared_ptr<Expression> argument_expression = argument->GetData();
		if (!DeclarationList::IsTerminator(parameter)) {
			auto parameter_declaration = parameter->GetData();

			auto argument_variable = make_shared<const BasicVariable>(
					parameter_declaration->GetName(),
					argument_expression->GetLocation());

			auto assign_errors = argument_variable->AssignValue(
					invocation_context, closure_reference, argument_expression,
					ASSIGN, function_execution_context);
			if (!ErrorList::IsTerminator(assign_errors)) {
				errors = ErrorList::Concatenate(errors, assign_errors);
			}

			argument = argument->GetNext();
			parameter = parameter->GetNext();
		} else {
			//argument list is longer than parameter list
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::TOO_MANY_ARGUMENTS,
							argument_expression->GetLocation().begin,
							declaration->ToString()), errors);
			break;
		}
	}

	//handle any remaining parameter declarations. if any parameter declarations don't have default values, generate an error
	while (!DeclarationList::IsTerminator(parameter)) {
		const_shared_ptr<DeclarationStatement> declaration =
				parameter->GetData();

		if (!declaration->GetInitializerExpression()) {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::NO_PARAMETER_DEFAULT,
							declaration->GetLocation().begin,
							*declaration->GetName()), errors);
			break;
		} else {
			parameter = parameter->GetNext();
		}
	}

	if (ErrorList::IsTerminator(errors)) {
		// the use of the function context as a closure is required for functions that yield functions, e.g. partial applicators
		auto execute_result = body->Execute(function_execution_context,
				function_execution_context);
		auto execute_errors = execute_result.GetErrors();
		if (ErrorList::IsTerminator(execute_errors)) {
			if (*declaration->GetReturnTypeSpecifier()
					== *TypeTable::GetNilTypeSpecifier()) {
				return make_shared<Result>(
						TypeTable::GetNilType()->GetDefaultValue(
								closure_reference->GetTypeTable()), errors);
			} else {
				plain_shared_ptr<Symbol> evaluation_result =
						execute_result.GetReturnValue();
				assert(evaluation_result);

				auto final_return_result = GetFinalReturnValue(
						evaluation_result->GetValue(),
						evaluation_result->GetTypeSpecifier(),
						declaration->GetReturnTypeSpecifier(),
						invocation_context->GetTypeTable());

				auto final_return_result_errors =
						final_return_result->GetErrors();
				if (ErrorList::IsTerminator(final_return_result_errors)) {
					return make_shared<Result>(
							final_return_result->GetRawData(), errors);
				} else {
					errors = ErrorList::Concatenate(errors,
							final_return_result_errors);
				}
			}
		} else {
			errors = ErrorList::Concatenate(errors, execute_errors);
		}
	}

	// default behavior: we have no result
	return make_shared<Result>(nullptr, errors);
}

const string Function::ToString(const TypeTable& type_table,
		const Indent& indent) const {
	ostringstream buffer;
	auto subject = m_variant_list;

	if (FunctionVariantList::IsTerminator(subject->GetNext())) {
		auto variant = subject->GetData();
		auto body = variant->GetBody();
		buffer << indent << "Body Location: ";
		if (body->GetLocation() != GetDefaultLocation()) {
			buffer << body->GetLocation();
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
	} else {
		while (!FunctionVariantList::IsTerminator(subject)) {
			auto variant = subject->GetData();
			buffer << variant->ToString(indent) << endl;
			subject = subject->GetNext();
		}
	}

	return buffer.str();
}

const_shared_ptr<TypeSpecifier> Function::GetTypeSpecifier() const {
	if (FunctionVariantList::IsTerminator(m_variant_list->GetNext())) {
		return m_variant_list->GetData()->GetDeclaration();
	} else {
		return make_shared<VariantFunctionSpecifier>(m_location, m_variant_list);
	}
}

const TypedResult<FunctionVariant> Function::GetVariant(
		const ArgumentListRef argument_list,
		const yy::location argument_list_location,
		const FunctionVariantListRef variant_list,
		const shared_ptr<ExecutionContext> context) {
// ref: http://stackoverflow.com/a/14336566/577298

	auto errors = ErrorList::From(
			make_shared<Error>(Error::SEMANTIC,
					Error::NO_FUNCTION_VARIANT_MATCH,
					argument_list_location.begin), ErrorList::GetTerminator());

	auto variant_subject = variant_list;

	uint best_variant_score = std::numeric_limits<unsigned int>::max();
	auto same_score = false;

	shared_ptr<const FunctionVariant> best_variant = nullptr;
	while (!FunctionVariantList::IsTerminator(variant_subject)) {
		auto variant = variant_subject->GetData();
		uint variant_score = std::numeric_limits<unsigned int>::max();
		auto variant_errors = ErrorList::GetTerminator();

		auto argument_subject = argument_list;
		auto parameter_subject = variant->GetDeclaration()->GetParameterList();
		if (ArgumentList::IsTerminator(argument_subject)) {
			if (DeclarationList::IsTerminator(parameter_subject)) {
				// trivial accept with empty argument & parameter lists
				variant_score = 0;
			} else {
				auto parameter = parameter_subject->GetData();
				if (parameter->GetInitializerExpression()) {
					// trivial accept with empty argument & all default parameters
					variant_score = 3;
				} else {
					// non-default parameters encountered
					while (!DeclarationList::IsTerminator(parameter_subject)) {
						auto parameter = parameter_subject->GetData();
						if (parameter->GetInitializerExpression()) {
							variant_score += 3;
							parameter_subject = parameter_subject->GetNext();
						} else {
							auto declaration = parameter->GetName();

							variant_errors = ErrorList::From(
									make_shared<Error>(Error::SEMANTIC,
											Error::NO_PARAMETER_DEFAULT,
											argument_list_location.end,
											*parameter->GetName()),
									variant_errors);
						}

						parameter_subject = parameter_subject->GetNext();
					}

					variant_score += 10;
				}
			}
		} else if (DeclarationList::IsTerminator(parameter_subject)) {
			if (ArgumentList::IsTerminator(argument_subject)) {
				// trivial accept with empty argument & parameter lists
				variant_score = 0;
			} else {
				auto argument = argument_subject->GetData();
				variant_errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::TOO_MANY_ARGUMENTS,
								argument->GetLocation().begin,
								variant->GetDeclaration()->ToString()),
						variant_errors);
				variant_score = 100;
			}
		} else {
			while (!ArgumentList::IsTerminator(argument_subject)
					&& !DeclarationList::IsTerminator(parameter_subject)) {
				auto argument = argument_subject->GetData();

				auto argument_type_specifier_result =
						argument->GetTypeSpecifier(context, RESOLVE);
				auto argument_type_errors =
						argument_type_specifier_result.GetErrors();
				if (ErrorList::IsTerminator(argument_type_errors)) {
					auto argument_type_specifier =
							argument_type_specifier_result.GetData();

					auto parameter = parameter_subject->GetData();
					auto parameter_type_specifier =
							parameter->GetTypeSpecifier();

					auto argument_assignment_compatibility =
							argument_type_specifier->AnalyzeAssignmentTo(
									parameter_type_specifier,
									context->GetTypeTable());

					switch (argument_assignment_compatibility) {
					case AnalysisResult::INCOMPATIBLE: {
						variant_score += 20;
						variant_errors =
								ErrorList::From(
										make_shared<Error>(Error::SEMANTIC,
												Error::FUNCTION_PARAMETER_TYPE_MISMATCH_INCOMPATIBLE,
												argument->GetLocation().begin,
												argument_type_specifier->ToString(),
												parameter_type_specifier->ToString()),
										variant_errors);
						break; // function variant does not match.
					}
					case AnalysisResult::AMBIGUOUS: {
						variant_score += 30;
						variant_errors =
								ErrorList::From(
										make_shared<Error>(Error::SEMANTIC,
												Error::FUNCTION_PARAMETER_TYPE_MISMATCH_AMBIGUOUS,
												argument->GetLocation().begin,
												argument_type_specifier->ToString(),
												parameter_type_specifier->ToString()),
										variant_errors);
						break; // function variant does not match.
					}
					case AnalysisResult::UNAMBIGUOUS:
					case AnalysisResult::UNAMBIGUOUS_NESTED: {
						variant_score += 2;
						break;
					}
					default: {
						// EQUIVALENT
						if (variant_score
								== std::numeric_limits<unsigned int>::max()) {
							variant_score = 0;
						} // otherwise, do nothing
					}
					}
				} else {
					errors = ErrorList::Concatenate(errors,
							argument_type_errors);
					variant_score = std::numeric_limits<unsigned int>::max();
					break;
				}

				argument_subject = argument_subject->GetNext();
				parameter_subject = parameter_subject->GetNext();
			}

			if (variant_score < std::numeric_limits<unsigned int>::max()) {
				// we have what might be a legitimate match; perform additional checks

				// check for too many arguments
				if (!ArgumentList::IsTerminator(argument_subject)) {
					auto argument = argument_subject->GetData();
					variant_errors = ErrorList::From(
							make_shared<Error>(Error::SEMANTIC,
									Error::TOO_MANY_ARGUMENTS,
									argument->GetLocation().begin,
									variant->GetDeclaration()->ToString()),
							variant_errors);
					variant_score += 100;
				}

				// handle default parameters
				while (!DeclarationList::IsTerminator(parameter_subject)) {
					auto parameter = parameter_subject->GetData();
					if (parameter->GetInitializerExpression()) {
						variant_score += 3;
					} else {
						auto declaration = parameter->GetName();
						// error: non-default parameter
						// variant_score = std::numeric_limits<unsigned int>::max();
						variant_errors = ErrorList::From(
								make_shared<Error>(Error::SEMANTIC,
										Error::NO_PARAMETER_DEFAULT,
										argument_list_location.end,
										*parameter->GetName()), variant_errors);

						variant_score += 200;
					}

					parameter_subject = parameter_subject->GetNext();
				}
			}
		}

		if (variant_score < std::numeric_limits<unsigned int>::max()) {
			if (variant_score < best_variant_score) {
				best_variant = variant;
				best_variant_score = variant_score;
				same_score = false;
				errors = variant_errors;
				variant_errors = ErrorList::GetTerminator();
			} else if (best_variant_score == variant_score) {
				same_score = true;
			}
		}

		variant_subject = variant_subject->GetNext();
	}

	if (same_score) {
		return TypedResult<FunctionVariant>(nullptr,
				ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::MULTIPLE_FUNCTION_VARIANT_MATCHES,
								argument_list_location.begin),
						ErrorList::GetTerminator()));
	} else if (errors) {
		return TypedResult<FunctionVariant>(nullptr, errors);
	} else {
		return TypedResult<FunctionVariant>(best_variant, errors);
	}
}

const_shared_ptr<Result> Function::GetFinalReturnValue(
		plain_shared_ptr<void> value,
		const_shared_ptr<TypeSpecifier> value_type_specifier,
		const_shared_ptr<TypeSpecifier> return_type_specifier,
		volatile_shared_ptr<TypeTable> type_table) {
	auto final_result = value;
	auto errors = ErrorList::GetTerminator();

	auto assignment_analysis = value_type_specifier->AnalyzeAssignmentTo(
			return_type_specifier, type_table);
	switch (assignment_analysis) {
	case UNAMBIGUOUS:
	case UNAMBIGUOUS_NESTED: {
		//we're returning a narrower type than the return type; perform widening
		auto return_type_result = return_type_specifier->GetType(type_table,
				RESOLVE);

		auto return_type_errors = return_type_result->GetErrors();
		if (ErrorList::IsTerminator(return_type_errors)) {
			auto return_type = return_type_result->GetData<TypeDefinition>();

			auto as_maybe = dynamic_pointer_cast<const MaybeType>(return_type);
			if (as_maybe) {
				if (*value_type_specifier
						== *TypeTable::GetNilTypeSpecifier()) {
					final_result = make_shared<Sum>(TypeTable::GetNilName(),
							value);
				} else {
					final_result = make_shared<Sum>(
							MaybeTypeSpecifier::VARIANT_NAME, value);
				}
				break;
			}

			auto as_sum = dynamic_pointer_cast<const SumType>(return_type);
			if (as_sum) {
				auto return_type_specifier_as_complex = dynamic_pointer_cast<
						const ComplexTypeSpecifier>(return_type_specifier);
				assert(return_type_specifier_as_complex);

				auto as_sum_specifier = SumTypeSpecifier(
						return_type_specifier_as_complex);
				plain_shared_ptr<string> tag = as_sum->MapSpecifierToVariant(
						as_sum_specifier, *value_type_specifier);

				final_result = make_shared<Sum>(tag, value);
				break;
			}
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

	return make_shared<Result>(final_result, errors);
}

const shared_ptr<ExecutionContext> Function::GetClosureReference() const {
	if (m_closure) {
		return m_closure;
	} else {
		return m_weak_closure.lock();
	}
}

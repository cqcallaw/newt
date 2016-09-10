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

#include <assignment_statement.h>
#include <sstream>
#include <expression.h>
#include <basic_variable.h>
#include <array_variable.h>
#include <member_variable.h>
#include <error.h>
#include <defaults.h>
#include <execution_context.h>
#include <typeinfo>
#include <variable_expression.h>
#include <record_type.h>
#include <record.h>
#include <specifiers/type_specifier.h>
#include <unit_type.h>

AssignmentStatement::AssignmentStatement(const_shared_ptr<Variable> variable,
		const AssignmentType op_type, const_shared_ptr<Expression> expression) :
		m_variable(variable), m_op_type(op_type), m_expression(expression) {
}

AssignmentStatement::~AssignmentStatement() {
}

const ErrorListRef AssignmentStatement::Preprocess(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure,
		const_shared_ptr<TypeSpecifier> return_type_specifier) const {
	const_shared_ptr<string> variable_name = m_variable->GetName();
	auto symbol = context->GetSymbol(variable_name, DEEP);

	int variable_line = m_variable->GetLocation().begin.line;
	int variable_column = m_variable->GetLocation().begin.column;

	auto errors = ErrorList::GetTerminator();
	if (symbol != Symbol::GetDefaultSymbol()) {
		const_shared_ptr<TypeSpecifier> symbol_type_specifier =
				symbol->GetTypeSpecifier();

		auto expression_type_specifier_result = m_expression->GetTypeSpecifier(
				context);

		errors = expression_type_specifier_result.GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto expression_type_specifier =
					expression_type_specifier_result.GetData();
			const_shared_ptr<BasicVariable> basic_variable =
					dynamic_pointer_cast<const BasicVariable>(m_variable);
			if (basic_variable) {
				if (!expression_type_specifier->AnalyzeAssignmentTo(
						symbol_type_specifier, context->GetTypeTable())) {
					yy::location expression_position =
							m_expression->GetPosition();
					errors = ErrorList::From(
							make_shared<Error>(Error::SEMANTIC,
									Error::ASSIGNMENT_TYPE_ERROR,
									expression_position.begin.line,
									expression_position.begin.column,
									symbol_type_specifier->ToString(),
									expression_type_specifier->ToString()),
							errors);
				}
			}

			const_shared_ptr<ArrayVariable> array_variable =
					dynamic_pointer_cast<const ArrayVariable>(m_variable);
			if (array_variable) {
				errors = array_variable->Validate(context);

				if (ErrorList::IsTerminator(errors)) {
					const_shared_ptr<TypeSpecifier> element_type =
							array_variable->GetElementType(context);
					if (!expression_type_specifier->AnalyzeAssignmentTo(
							element_type, context->GetTypeTable())) {
						yy::location expression_position =
								m_expression->GetPosition();
						errors = ErrorList::From(
								make_shared<Error>(Error::SEMANTIC,
										Error::ASSIGNMENT_TYPE_ERROR,
										expression_position.begin.line,
										expression_position.begin.column,
										element_type->ToString(),
										expression_type_specifier->ToString()),
								errors);
					}
				}
			}

			const_shared_ptr<MemberVariable> member_variable =
					dynamic_pointer_cast<const MemberVariable>(m_variable);
			if (member_variable) {
				auto container_specifier_result =
						member_variable->GetContainer()->GetTypeSpecifier(
								context);

				errors = ErrorList::Concatenate(errors,
						container_specifier_result.GetErrors());
				if (ErrorList::IsTerminator(errors)) {
					auto container_specifier =
							container_specifier_result.GetData();

					auto as_complex = dynamic_pointer_cast<
							const ComplexTypeSpecifier>(container_specifier);

					if (as_complex) {
						auto container_type = context->GetTypeTable()->GetType<
								TypeDefinition>(as_complex, DEEP, RESOLVE);

						if (container_type) {
							const_shared_ptr<RecordType> type =
									dynamic_pointer_cast<const RecordType>(
											container_type);

							if (type) {
								if (type->GetModifiers()
										& Modifier::Type::MUTABLE) {
									auto member_variable_type_specifer_result =
											member_variable->GetTypeSpecifier(
													context);

									auto member_variable_type_specifer_result_errors =
											member_variable_type_specifer_result.GetErrors();
									if (ErrorList::IsTerminator(
											member_variable_type_specifer_result_errors)) {
										auto member_variable_type_specifier =
												member_variable_type_specifer_result.GetData();

										auto expression_type_specifier_result =
												m_expression->GetTypeSpecifier(
														context);

										auto expression_type_specifier_result_errors =
												expression_type_specifier_result.GetErrors();

										if (ErrorList::IsTerminator(
												expression_type_specifier_result_errors)) {
											auto expression_type_specifier =
													expression_type_specifier_result.GetData();
											if (!expression_type_specifier->AnalyzeAssignmentTo(
													member_variable_type_specifier,
													context->GetTypeTable())) {
												errors =
														ErrorList::From(
																make_shared<
																		Error>(
																		Error::SEMANTIC,
																		Error::ASSIGNMENT_TYPE_ERROR,
																		member_variable->GetContainer()->GetLocation().begin.line,
																		member_variable->GetContainer()->GetLocation().begin.column,
																		member_variable_type_specifier->ToString(),
																		expression_type_specifier->ToString()),
																errors);
											}
										} else {
											errors =
													ErrorList::Concatenate(
															errors,
															expression_type_specifier_result_errors);
										}
									} else {
										errors =
												ErrorList::Concatenate(errors,
														member_variable_type_specifer_result_errors);
									}
								} else {
									errors =
											ErrorList::From(
													make_shared<Error>(
															Error::SEMANTIC,
															Error::READONLY,
															member_variable->GetContainer()->GetLocation().begin.line,
															member_variable->GetContainer()->GetLocation().begin.column,
															*variable_name),
													errors);
								}
							} else {
								errors =
										ErrorList::From(
												make_shared<Error>(
														Error::SEMANTIC,
														Error::NOT_A_COMPOUND_TYPE,
														member_variable->GetContainer()->GetLocation().begin.line,
														member_variable->GetContainer()->GetLocation().begin.column,
														*variable_name),
												errors);
							}
						} else {
							errors =
									ErrorList::From(
											make_shared<Error>(Error::SEMANTIC,
													Error::UNDECLARED_TYPE,
													member_variable->GetContainer()->GetLocation().begin.line,
													member_variable->GetContainer()->GetLocation().begin.column,
													*variable_name), errors);
						}
					} else {
						errors =
								ErrorList::From(
										make_shared<Error>(Error::SEMANTIC,
												Error::NOT_A_COMPOUND_TYPE,
												member_variable->GetContainer()->GetLocation().begin.line,
												member_variable->GetContainer()->GetLocation().begin.column,
												*variable_name), errors);
					}
				}
			}
		}
	} else {
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						variable_line, variable_column, *variable_name),
				errors);
	}

	return errors;
}

const ErrorListRef AssignmentStatement::do_op(
		const_shared_ptr<string> variable_name, const BasicType variable_type,
		int variable_line, int variable_column, const bool old_value,
		const bool expression_value, const AssignmentType op,
		const shared_ptr<ExecutionContext> execution_context, bool &out) {
	ErrorListRef errors = ErrorList::GetTerminator();
	switch (op) {
	case ASSIGN:
		out = expression_value;
		break;
	default:
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC,
						Error::INVALID_LHS_OF_ASSIGNMENT, variable_line,
						variable_column, *variable_name,
						TypeToString(variable_type)), errors);
		out = false;
	}

	return errors;
}

const ErrorListRef AssignmentStatement::do_op(
		const_shared_ptr<string> variable_name, const BasicType variable_type,
		int variable_line, int variable_column, const int old_value,
		const int expression_value, const AssignmentType op,
		const shared_ptr<ExecutionContext> execution_context, int& out) {
	ErrorListRef errors = ErrorList::GetTerminator();

	switch (op) {
	case PLUS_ASSIGN:
		out = old_value + expression_value;
		break;
	case MINUS_ASSIGN:
		out = old_value - expression_value;
		break;
	case ASSIGN:
		out = expression_value;
		break;
	default:
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC,
						Error::INVALID_LHS_OF_ASSIGNMENT, variable_line,
						variable_column, *variable_name,
						TypeToString(variable_type)), errors);
		out = 0;
	}

	return errors;
}

const ErrorListRef AssignmentStatement::do_op(
		const_shared_ptr<string> variable_name, const BasicType variable_type,
		int variable_line, int variable_column, const double old_value,
		const double expression_value, const AssignmentType op,
		const shared_ptr<ExecutionContext> execution_context, double &out) {
	ErrorListRef errors = ErrorList::GetTerminator();

	switch (op) {
	case PLUS_ASSIGN:
		out = old_value + expression_value;
		break;
	case MINUS_ASSIGN:
		out = old_value - expression_value;
		break;
	case ASSIGN:
		out = expression_value;
		break;
	default:
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC,
						Error::INVALID_LHS_OF_ASSIGNMENT, variable_line,
						variable_column, *variable_name,
						TypeToString(variable_type)), errors);
		out = 1.0;
	}

	return errors;
}

const ErrorListRef AssignmentStatement::do_op(
		const_shared_ptr<string> variable_name, const BasicType variable_type,
		int variable_line, int variable_column,
		const_shared_ptr<string> old_value, const bool expression_value,
		const AssignmentType op,
		const shared_ptr<ExecutionContext> execution_context, string* &out) {
	return do_op(variable_name, variable_type, variable_line, variable_column,
			old_value, AsString(expression_value), op, execution_context, out);
}

const ErrorListRef AssignmentStatement::do_op(
		const_shared_ptr<string> variable_name, const BasicType variable_type,
		int variable_line, int variable_column,
		const_shared_ptr<string> old_value, const int expression_value,
		const AssignmentType op,
		const shared_ptr<ExecutionContext> execution_context, string* &out) {
	return do_op(variable_name, variable_type, variable_line, variable_column,
			old_value, AsString(expression_value), op, execution_context, out);
}

const ErrorListRef AssignmentStatement::do_op(
		const_shared_ptr<string> variable_name, const BasicType variable_type,
		int variable_line, int variable_column,
		const_shared_ptr<string> old_value, const double expression_value,
		const AssignmentType op,
		const shared_ptr<ExecutionContext> execution_context, string* &out) {
	return do_op(variable_name, variable_type, variable_line, variable_column,
			old_value, AsString(expression_value), op, execution_context, out);
}

const ErrorListRef AssignmentStatement::do_op(
		const_shared_ptr<string> variable_name, const BasicType variable_type,
		int variable_line, int variable_column,
		const_shared_ptr<string> old_value,
		const_shared_ptr<string> expression_value, const AssignmentType op,
		const shared_ptr<ExecutionContext> execution_context, string* &out) {
	ErrorListRef errors = ErrorList::GetTerminator();

	ostringstream buffer;
	switch (op) {
	case PLUS_ASSIGN: {
		buffer << *old_value;
		buffer << *expression_value;
		out = new string(buffer.str());
		break;
	}
	case ASSIGN:
		out = new string(*expression_value);
		break;
	case MINUS_ASSIGN:
	default:
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC,
						Error::INVALID_LHS_OF_ASSIGNMENT, variable_line,
						variable_column, *(variable_name),
						TypeToString(variable_type)), errors);
	}

	return errors;
}
const_shared_ptr<Result> AssignmentStatement::do_op(
		const_shared_ptr<string> variable_name, const BasicType variable_type,
		int variable_line, int variable_column, const int value,
		const_shared_ptr<Expression> expression, const AssignmentType op,
		const shared_ptr<ExecutionContext> execution_context) {
	ErrorListRef errors;

	int new_value = 0;
	const_shared_ptr<Result> evaluation = expression->Evaluate(
			execution_context, execution_context);
	if (!ErrorList::IsTerminator(evaluation->GetErrors())) {
		return evaluation;
	}

	auto expression_type_specifier_result = expression->GetTypeSpecifier(
			execution_context);

	errors = expression_type_specifier_result.GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto expression_type_specifier =
				expression_type_specifier_result.GetData();

		const_shared_ptr<PrimitiveTypeSpecifier> as_primitive =
				dynamic_pointer_cast<const PrimitiveTypeSpecifier>(
						expression_type_specifier);

		if (as_primitive) {
			const BasicType basic_type = as_primitive->GetBasicType();
			switch (basic_type) {
			case BOOLEAN: {
				errors = do_op(variable_name, variable_type, variable_line,
						variable_column, value, *(evaluation->GetData<bool>()),
						op, execution_context, new_value);
				break;
			}
			case INT: {
				errors = do_op(variable_name, variable_type, variable_line,
						variable_column, value, *(evaluation->GetData<int>()),
						op, execution_context, new_value);
				break;
			}
			default:
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::ASSIGNMENT_TYPE_ERROR, variable_line,
								variable_column, TypeToString(INT),
								expression_type_specifier->ToString()), errors);
			}
		} else {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::ASSIGNMENT_TYPE_ERROR, variable_line,
							variable_column, TypeToString(INT),
							expression_type_specifier->ToString()), errors);
		}
	}

	const_shared_ptr<int> wrapper = make_shared<int>(new_value);
	return make_shared<Result>(wrapper, errors);
}

const_shared_ptr<Result> AssignmentStatement::do_op(
		const_shared_ptr<string> variable_name, const BasicType variable_type,
		int variable_line, int variable_column, const double value,
		const_shared_ptr<Expression> expression, AssignmentType op,
		const shared_ptr<ExecutionContext> execution_context) {
	ErrorListRef errors;

	double new_value = 0;
	const_shared_ptr<Result> evaluation = expression->Evaluate(
			execution_context, execution_context);
	if (!ErrorList::IsTerminator(evaluation->GetErrors())) {
		return evaluation;
	}

	auto expression_type_specifier_result = expression->GetTypeSpecifier(
			execution_context);

	errors = expression_type_specifier_result.GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto expression_type_specifier =
				expression_type_specifier_result.GetData();
		const_shared_ptr<PrimitiveTypeSpecifier> as_primitive =
				dynamic_pointer_cast<const PrimitiveTypeSpecifier>(
						expression_type_specifier);

		if (as_primitive) {
			const BasicType basic_type = as_primitive->GetBasicType();
			switch (basic_type) {
			case BOOLEAN:
				errors = do_op(variable_name, variable_type, variable_line,
						variable_column, value, *(evaluation->GetData<bool>()),
						op, execution_context, new_value);
				break;
			case INT: {
				errors = do_op(variable_name, variable_type, variable_line,
						variable_column, value, *(evaluation->GetData<int>()),
						op, execution_context, new_value);
				break;
			}
			case DOUBLE: {
				errors = do_op(variable_name, variable_type, variable_line,
						variable_column, value,
						*(evaluation->GetData<double>()), op, execution_context,
						new_value);
				break;
			}
			default:
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::ASSIGNMENT_TYPE_ERROR, variable_line,
								variable_column, TypeToString(DOUBLE),
								expression_type_specifier->ToString()), errors);
			}
		} else {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::ASSIGNMENT_TYPE_ERROR, variable_line,
							variable_column, TypeToString(DOUBLE),
							expression_type_specifier->ToString()), errors);
		}
	}

	const_shared_ptr<void> wrapper = make_shared<double>(new_value);
	return make_shared<Result>(wrapper, errors);
}

const_shared_ptr<Result> AssignmentStatement::do_op(
		const_shared_ptr<string> variable_name, const BasicType variable_type,
		int variable_line, int variable_column, const_shared_ptr<string> value,
		const_shared_ptr<Expression> expression, AssignmentType op,
		const shared_ptr<ExecutionContext> execution_context) {
	ErrorListRef errors;

	string* new_value = nullptr;
	const_shared_ptr<Result> evaluation = expression->Evaluate(
			execution_context, execution_context);
	if (!ErrorList::IsTerminator(evaluation->GetErrors())) {
		return evaluation;
	}

	auto expression_type_specifier_result = expression->GetTypeSpecifier(
			execution_context);

	errors = expression_type_specifier_result.GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto expression_type_specifier =
				expression_type_specifier_result.GetData();

		const_shared_ptr<PrimitiveTypeSpecifier> as_primitive =
				dynamic_pointer_cast<const PrimitiveTypeSpecifier>(
						expression_type_specifier);
		if (as_primitive) {
			const BasicType basic_type = as_primitive->GetBasicType();
			switch (basic_type) {
			case BOOLEAN:
				errors = do_op(variable_name, variable_type, variable_line,
						variable_column, value, *(evaluation->GetData<bool>()),
						op, execution_context, new_value);
				break;
			case INT: {
				errors = do_op(variable_name, variable_type, variable_line,
						variable_column, value, *(evaluation->GetData<int>()),
						op, execution_context, new_value);
				break;
			}
			case DOUBLE: {
				errors = do_op(variable_name, variable_type, variable_line,
						variable_column, value,
						*(evaluation->GetData<double>()), op, execution_context,
						new_value);
				break;
			}
			case STRING: {
				errors = do_op(variable_name, variable_type, variable_line,
						variable_column, value, evaluation->GetData<string>(),
						op, execution_context, new_value);
				break;
			}
			default:
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::ASSIGNMENT_TYPE_ERROR, variable_line,
								variable_column, TypeToString(STRING),
								expression_type_specifier->ToString()), errors);
			}
		} else {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::ASSIGNMENT_TYPE_ERROR, variable_line,
							variable_column, TypeToString(STRING),
							expression_type_specifier->ToString()), errors);
		}
	}

	auto wrapper = const_shared_ptr<void>(new_value);
	return make_shared<Result>(wrapper, errors);
}

const ErrorListRef AssignmentStatement::Execute(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	if (m_variable == nullptr || m_expression == nullptr) {
		assert(false);
	}

	const_shared_ptr<string> variable_name = m_variable->GetName();
	int variable_line = m_variable->GetLocation().begin.line;
	int variable_column = m_variable->GetLocation().begin.column;

	auto symbol = context->GetSymbol(variable_name, DEEP);

	if (symbol && symbol != Symbol::GetDefaultSymbol()) {
		errors = m_variable->AssignValue(context, closure, m_expression,
				m_op_type);
	} else {
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						variable_line, variable_column, *(variable_name)),
				errors);
	}

	return errors;
}

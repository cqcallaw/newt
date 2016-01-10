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
#include <compound_type_instance.h>
#include <error.h>
#include <defaults.h>
#include <execution_context.h>
#include <typeinfo>
#include <variable_expression.h>
#include <compound_type.h>
#include <type_specifier.h>

AssignmentStatement::AssignmentStatement(const_shared_ptr<Variable> variable,
		const AssignmentType op_type, const_shared_ptr<Expression> expression) :
		m_variable(variable), m_op_type(op_type), m_expression(expression) {
}

AssignmentStatement::~AssignmentStatement() {
}

const ErrorListRef AssignmentStatement::preprocess(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	shared_ptr<SymbolTable> symbol_table = static_pointer_cast<SymbolTable>(
			execution_context);
	const_shared_ptr<string> variable_name = m_variable->GetName();
	auto symbol = symbol_table->GetSymbol(variable_name, DEEP);
	const_shared_ptr<TypeSpecifier> symbol_type = symbol->GetType();

	int variable_line = m_variable->GetLocation().begin.line;
	int variable_column = m_variable->GetLocation().begin.column;

	if (symbol != Symbol::GetDefaultSymbol()) {
		const_shared_ptr<BasicVariable> basic_variable = dynamic_pointer_cast<
				const BasicVariable>(m_variable);
		if (basic_variable) {
			const_shared_ptr<PrimitiveTypeSpecifier> as_primitive =
					dynamic_pointer_cast<const PrimitiveTypeSpecifier>(
							symbol_type);
			const_shared_ptr<TypeSpecifier> expression_type =
					m_expression->GetType(execution_context);
			if (as_primitive) {
				if (expression_type->IsAssignableTo(symbol_type)) {
					errors = ErrorList::GetTerminator();
				} else {
					const yy::location variable_location =
							m_variable->GetLocation();
					errors = ErrorList::From(
							make_shared<Error>(Error::SEMANTIC,
									Error::ASSIGNMENT_TYPE_ERROR,
									variable_location.begin.line,
									variable_location.begin.column,
									symbol_type->ToString(),
									expression_type->ToString()), errors);
				}
			}

			const_shared_ptr<ArrayTypeSpecifier> as_array =
					dynamic_pointer_cast<const ArrayTypeSpecifier>(symbol_type);
			if (as_array) {
				//reassigning raw array reference, not an array element
				if (!expression_type->IsAssignableTo(symbol_type)) {
					yy::location expression_position =
							m_expression->GetPosition();
					errors = ErrorList::From(
							make_shared<Error>(Error::SEMANTIC,
									Error::ASSIGNMENT_TYPE_ERROR,
									expression_position.begin.line,
									expression_position.begin.column,
									as_array->ToString(),
									expression_type->ToString()), errors);
				}
			}

			const_shared_ptr<CompoundTypeSpecifier> as_compound =
					dynamic_pointer_cast<const CompoundTypeSpecifier>(
							symbol_type);
			if (as_compound) {
				//reassigning raw struct reference, not a member
				if (!expression_type->IsAssignableTo(symbol_type)) {
					yy::location expression_position =
							m_expression->GetPosition();
					const string struct_type_name = as_compound->GetTypeName();
					errors = ErrorList::From(
							make_shared<Error>(Error::SEMANTIC,
									Error::ASSIGNMENT_TYPE_ERROR,
									expression_position.begin.line,
									expression_position.begin.column,
									struct_type_name,
									expression_type->ToString()), errors);
				}
			}
		}

		const_shared_ptr<ArrayVariable> array_variable = dynamic_pointer_cast<
				const ArrayVariable>(m_variable);
		if (array_variable) {
			errors = array_variable->Validate(execution_context);

			const_shared_ptr<TypeSpecifier> expression_type =
					m_expression->GetType(execution_context);
			const_shared_ptr<TypeSpecifier> innermost_element_type =
					array_variable->GetInnerMostElementType(execution_context);
			if (!expression_type->IsAssignableTo(innermost_element_type)) {
				yy::location expression_position = m_expression->GetPosition();
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::ASSIGNMENT_TYPE_ERROR,
								expression_position.begin.line,
								expression_position.begin.column,
								innermost_element_type->ToString(),
								expression_type->ToString()), errors);
			}
		}

		const_shared_ptr<MemberVariable> member_variable = dynamic_pointer_cast<
				const MemberVariable>(m_variable);
		if (member_variable) {
			const_shared_ptr<CompoundTypeSpecifier> as_compound =
					dynamic_pointer_cast<const CompoundTypeSpecifier>(
							member_variable->GetContainer()->GetType(
									execution_context));

			if (as_compound) {
				const_shared_ptr<CompoundType> type =
						execution_context->GetTypeTable()->GetType(
								as_compound->GetTypeName());

				if (!(type->GetModifiers() & Modifier::Type::READONLY)) {
					const_shared_ptr<TypeSpecifier> member_variable_type =
							member_variable->GetType(execution_context);

					if (member_variable_type
							!= PrimitiveTypeSpecifier::GetNone()) {
						const_shared_ptr<TypeSpecifier> expression_type =
								m_expression->GetType(execution_context);

						if (!expression_type->IsAssignableTo(
								member_variable_type)) {
							errors =
									ErrorList::From(
											make_shared<Error>(Error::SEMANTIC,
													Error::ASSIGNMENT_TYPE_ERROR,
													member_variable->GetContainer()->GetLocation().begin.line,
													member_variable->GetContainer()->GetLocation().begin.column,
													member_variable_type->ToString(),
													expression_type->ToString()),
											errors);
						}
					} else {
						errors =
								ErrorList::From(
										make_shared<Error>(Error::SEMANTIC,
												Error::UNDECLARED_MEMBER,
												member_variable->GetMemberVariable()->GetLocation().begin.line,
												member_variable->GetMemberVariable()->GetLocation().begin.column,
												*member_variable->GetMemberVariable()->GetName(),
												member_variable->GetContainer()->GetType(
														execution_context)->ToString()),
										errors);
					}
				} else {
					errors =
							ErrorList::From(
									make_shared<Error>(Error::SEMANTIC,
											Error::READONLY,
											member_variable->GetContainer()->GetLocation().begin.line,
											member_variable->GetContainer()->GetLocation().begin.column,
											*variable_name), errors);
				}
			} else {
				errors =
						ErrorList::From(
								make_shared<Error>(Error::SEMANTIC,
										Error::VARIABLE_NOT_A_COMPOUND_TYPE,
										member_variable->GetContainer()->GetLocation().begin.line,
										member_variable->GetContainer()->GetLocation().begin.column,
										*variable_name), errors);
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
						type_to_string(variable_type)), errors);
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
						type_to_string(variable_type)), errors);
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
						type_to_string(variable_type)), errors);
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
						type_to_string(variable_type)), errors);
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
			execution_context);
	if (!ErrorList::IsTerminator(evaluation->GetErrors())) {
		return evaluation;
	}

	auto void_value = evaluation->GetData();

	const_shared_ptr<TypeSpecifier> expression_type_specifier =
			expression->GetType(execution_context);
	const_shared_ptr<PrimitiveTypeSpecifier> as_primitive =
			dynamic_pointer_cast<const PrimitiveTypeSpecifier>(
					expression_type_specifier);

	if (as_primitive) {
		const BasicType basic_type = as_primitive->GetBasicType();
		switch (basic_type) {
		case BOOLEAN: {
			errors = do_op(variable_name, variable_type, variable_line,
					variable_column, value,
					*(static_pointer_cast<const bool>(void_value)), op,
					execution_context, new_value);
			break;
		}
		case INT: {
			errors = do_op(variable_name, variable_type, variable_line,
					variable_column, value,
					*(static_pointer_cast<const int>(void_value)), op,
					execution_context, new_value);
			break;
		}
		default:
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::ASSIGNMENT_TYPE_ERROR, variable_line,
							variable_column, type_to_string(INT),
							expression_type_specifier->ToString()), errors);
		}
	} else {
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC,
						Error::ASSIGNMENT_TYPE_ERROR, variable_line,
						variable_column, type_to_string(INT),
						expression_type_specifier->ToString()), errors);
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
			execution_context);
	if (!ErrorList::IsTerminator(evaluation->GetErrors())) {
		return evaluation;
	}

	auto void_value = evaluation->GetData();

	const_shared_ptr<TypeSpecifier> expression_type_specifier =
			expression->GetType(execution_context);
	const_shared_ptr<PrimitiveTypeSpecifier> as_primitive =
			dynamic_pointer_cast<const PrimitiveTypeSpecifier>(
					expression_type_specifier);

	if (as_primitive) {
		const BasicType basic_type = as_primitive->GetBasicType();
		switch (basic_type) {
		case BOOLEAN:
			errors = do_op(variable_name, variable_type, variable_line,
					variable_column, value,
					*(static_pointer_cast<const bool>(void_value)), op,
					execution_context, new_value);
			break;
		case INT: {
			errors = do_op(variable_name, variable_type, variable_line,
					variable_column, value,
					*(static_pointer_cast<const int>(void_value)), op,
					execution_context, new_value);
			break;
		}
		case DOUBLE: {
			errors = do_op(variable_name, variable_type, variable_line,
					variable_column, value,
					*(static_pointer_cast<const double>(void_value)), op,
					execution_context, new_value);
			break;
		}
		default:
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::ASSIGNMENT_TYPE_ERROR, variable_line,
							variable_column, type_to_string(DOUBLE),
							expression_type_specifier->ToString()), errors);
		}
	} else {
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC,
						Error::ASSIGNMENT_TYPE_ERROR, variable_line,
						variable_column, type_to_string(DOUBLE),
						expression_type_specifier->ToString()), errors);
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
			execution_context);
	if (!ErrorList::IsTerminator(evaluation->GetErrors())) {
		return evaluation;
	}

	auto void_value = evaluation->GetData();

	const_shared_ptr<TypeSpecifier> expression_type_specifier =
			expression->GetType(execution_context);
	const_shared_ptr<PrimitiveTypeSpecifier> as_primitive =
			dynamic_pointer_cast<const PrimitiveTypeSpecifier>(
					expression_type_specifier);

	if (as_primitive) {
		const BasicType basic_type = as_primitive->GetBasicType();
		switch (basic_type) {
		case BOOLEAN:
			errors = do_op(variable_name, variable_type, variable_line,
					variable_column, value,
					*(static_pointer_cast<const bool>(void_value)), op,
					execution_context, new_value);
			break;
		case INT: {
			errors = do_op(variable_name, variable_type, variable_line,
					variable_column, value,
					*(static_pointer_cast<const int>(void_value)), op,
					execution_context, new_value);
			break;
		}
		case DOUBLE: {
			errors = do_op(variable_name, variable_type, variable_line,
					variable_column, value,
					*(static_pointer_cast<const double>(void_value)), op,
					execution_context, new_value);
			break;
		}
		case STRING: {
			errors = do_op(variable_name, variable_type, variable_line,
					variable_column, value,
					static_pointer_cast<const string>(void_value), op,
					execution_context, new_value);
			break;
		}
		default:
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::ASSIGNMENT_TYPE_ERROR, variable_line,
							variable_column, type_to_string(STRING),
							expression_type_specifier->ToString()), errors);
		}
	} else {
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC,
						Error::ASSIGNMENT_TYPE_ERROR, variable_line,
						variable_column, type_to_string(STRING),
						expression_type_specifier->ToString()), errors);
	}

	auto wrapper = const_shared_ptr<void>(new_value);
	return make_shared<Result>(wrapper, errors);
}

const ErrorListRef AssignmentStatement::execute(
		shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	if (m_variable == nullptr || m_expression == nullptr) {
		assert(false);
	}

	const_shared_ptr<string> variable_name = m_variable->GetName();
	int variable_line = m_variable->GetLocation().begin.line;
	int variable_column = m_variable->GetLocation().begin.column;

	const_shared_ptr<SymbolTable> symbol_table =
			static_pointer_cast<SymbolTable>(
					execution_context);
	auto symbol = symbol_table->GetSymbol(variable_name, DEEP);

	if (symbol && symbol != Symbol::GetDefaultSymbol()) {
		errors = m_variable->AssignValue(execution_context, m_expression,
				m_op_type);
	} else {
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						variable_line, variable_column, *(variable_name)),
				errors);
	}

	return errors;
}

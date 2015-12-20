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

const LinkedList<const Error*>* AssignmentStatement::preprocess(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::GetTerminator();

	SymbolTable* symbol_table =
			(SymbolTable*) execution_context->GetSymbolContext();
	const_shared_ptr<string> variable_name = m_variable->GetName();
	const Symbol* symbol = symbol_table->GetSymbol(variable_name, DEEP);
	const_shared_ptr<TypeSpecifier> symbol_type = symbol->GetType();

	int variable_line = m_variable->GetLocation().begin.line;
	int variable_column = m_variable->GetLocation().begin.column;

	if (symbol != Symbol::GetDefaultSymbol()) {
		const BasicVariable* basic_variable =
				dynamic_cast<const BasicVariable*>(m_variable.get());
		if (basic_variable != nullptr) {
			const_shared_ptr<PrimitiveTypeSpecifier> as_primitive =
					std::dynamic_pointer_cast<const PrimitiveTypeSpecifier>(
							symbol_type);
			const_shared_ptr<TypeSpecifier> expression_type =
					m_expression->GetType(execution_context);
			if (as_primitive != nullptr) {
				if (expression_type->IsAssignableTo(symbol_type)) {
					errors = LinkedList<const Error*>::GetTerminator();
				} else {
					const yy::location variable_location =
							m_variable->GetLocation();
					errors = new LinkedList<const Error*>(
							new Error(Error::SEMANTIC,
									Error::ASSIGNMENT_TYPE_ERROR,
									variable_location.begin.line,
									variable_location.begin.column,
									symbol_type->ToString(),
									expression_type->ToString()));
				}
			}

			const_shared_ptr<ArrayTypeSpecifier> as_array =
					std::dynamic_pointer_cast<const ArrayTypeSpecifier>(
							symbol_type);
			if (as_array != nullptr) {
				//reassigning raw array reference, not an array element
				if (!expression_type->IsAssignableTo(symbol_type)) {
					yy::location expression_position =
							m_expression->GetPosition();
					errors = new LinkedList<const Error*>(
							new Error(Error::SEMANTIC,
									Error::ASSIGNMENT_TYPE_ERROR,
									expression_position.begin.line,
									expression_position.begin.column,
									as_array->ToString(),
									expression_type->ToString()));
				}
			}

			const_shared_ptr<CompoundTypeSpecifier> as_compound =
					std::dynamic_pointer_cast<const CompoundTypeSpecifier>(
							symbol_type);
			if (as_compound != nullptr) {
				//reassigning raw struct reference, not a member
				if (!expression_type->IsAssignableTo(symbol_type)) {
					yy::location expression_position =
							m_expression->GetPosition();
					const string struct_type_name = as_compound->GetTypeName();
					errors = new LinkedList<const Error*>(
							new Error(Error::SEMANTIC,
									Error::ASSIGNMENT_TYPE_ERROR,
									expression_position.begin.line,
									expression_position.begin.column,
									struct_type_name,
									expression_type->ToString()));
				}
			}
		}

		const ArrayVariable* array_variable =
				dynamic_cast<const ArrayVariable*>(m_variable.get());
		if (array_variable != nullptr) {
			errors = array_variable->Validate(execution_context);

			const_shared_ptr<TypeSpecifier> expression_type =
					m_expression->GetType(execution_context);
			const_shared_ptr<TypeSpecifier> innermost_element_type =
					array_variable->GetInnerMostElementType(execution_context);
			if (!expression_type->IsAssignableTo(innermost_element_type)) {
				yy::location expression_position = m_expression->GetPosition();
				errors = new LinkedList<const Error*>(
						new Error(Error::SEMANTIC, Error::ASSIGNMENT_TYPE_ERROR,
								expression_position.begin.line,
								expression_position.begin.column,
								innermost_element_type->ToString(),
								expression_type->ToString()));
			}
		}

		const MemberVariable* member_variable =
				dynamic_cast<const MemberVariable*>(m_variable.get());
		if (member_variable != nullptr) {
			const_shared_ptr<CompoundTypeSpecifier> as_compound =
					std::dynamic_pointer_cast<const CompoundTypeSpecifier>(
							member_variable->GetContainer()->GetType(
									execution_context));

			if (as_compound) {
				const CompoundType* type =
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
									new LinkedList<const Error*>(
											new Error(Error::SEMANTIC,
													Error::ASSIGNMENT_TYPE_ERROR,
													member_variable->GetContainer()->GetLocation().begin.line,
													member_variable->GetContainer()->GetLocation().begin.column,
													member_variable_type->ToString(),
													expression_type->ToString()));
						}
					} else {
						errors =
								new LinkedList<const Error*>(
										new Error(Error::SEMANTIC,
												Error::UNDECLARED_MEMBER,
												member_variable->GetMemberVariable()->GetLocation().begin.line,
												member_variable->GetMemberVariable()->GetLocation().begin.column,
												*member_variable->GetMemberVariable()->GetName(),
												member_variable->GetContainer()->GetType(
														execution_context)->ToString()));
					}
				} else {
					errors =
							errors->With(
									new Error(Error::SEMANTIC, Error::READONLY,
											member_variable->GetContainer()->GetLocation().begin.line,
											member_variable->GetContainer()->GetLocation().begin.column,
											*variable_name));
				}
			} else {
				errors =
						errors->With(
								new Error(Error::SEMANTIC,
										Error::VARIABLE_NOT_A_COMPOUND_TYPE,
										member_variable->GetContainer()->GetLocation().begin.line,
										member_variable->GetContainer()->GetLocation().begin.column,
										*variable_name));
			}
		}
	} else {
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						variable_line, variable_column, *variable_name));
	}

	return errors;
}

const LinkedList<const Error*>* AssignmentStatement::do_op(
		const_shared_ptr<string> variable_name, const BasicType variable_type,
		int variable_line, int variable_column, const bool old_value,
		const bool expression_value, const AssignmentType op,
		const ExecutionContext* execution_context, bool &out) {
	const LinkedList<const Error*>* errors;
	switch (op) {
	case ASSIGN:
		errors = LinkedList<const Error*>::GetTerminator();
		out = expression_value;
		break;
	default:
		errors = new LinkedList<const Error*>(
				new Error(Error::SEMANTIC, Error::INVALID_LHS_OF_ASSIGNMENT,
						variable_line, variable_column, *variable_name,
						type_to_string(variable_type)));
		out = false;
	}

	return errors;
}

const LinkedList<const Error*>* AssignmentStatement::do_op(
		const_shared_ptr<string> variable_name, const BasicType variable_type,
		int variable_line, int variable_column, const int old_value,
		const int expression_value, const AssignmentType op,
		const ExecutionContext* execution_context, int& out) {
	const LinkedList<const Error*>* errors;

	switch (op) {
	case PLUS_ASSIGN:
		errors = LinkedList<const Error*>::GetTerminator();
		out = old_value + expression_value;
		break;
	case MINUS_ASSIGN:
		errors = LinkedList<const Error*>::GetTerminator();
		out = old_value - expression_value;
		break;
	case ASSIGN:
		errors = LinkedList<const Error*>::GetTerminator();
		out = expression_value;
		break;
	default:
		errors = new LinkedList<const Error*>(
				new Error(Error::SEMANTIC, Error::INVALID_LHS_OF_ASSIGNMENT,
						variable_line, variable_column, *variable_name,
						type_to_string(variable_type)));
		out = 0;
	}

	return errors;
}

const LinkedList<const Error*>* AssignmentStatement::do_op(
		const_shared_ptr<string> variable_name, const BasicType variable_type,
		int variable_line, int variable_column, const double old_value,
		const double expression_value, const AssignmentType op,
		const ExecutionContext* execution_context, double &out) {
	const LinkedList<const Error*>* errors;

	switch (op) {
	case PLUS_ASSIGN:
		errors = LinkedList<const Error*>::GetTerminator();
		out = old_value + expression_value;
		break;
	case MINUS_ASSIGN:
		errors = LinkedList<const Error*>::GetTerminator();
		out = old_value - expression_value;
		break;
	case ASSIGN:
		errors = LinkedList<const Error*>::GetTerminator();
		out = expression_value;
		break;
	default:
		errors = new LinkedList<const Error*>(
				new Error(Error::SEMANTIC, Error::INVALID_LHS_OF_ASSIGNMENT,
						variable_line, variable_column, *variable_name,
						type_to_string(variable_type)));
		out = 1.0;
	}

	return errors;
}

const LinkedList<const Error*>* AssignmentStatement::do_op(
		const_shared_ptr<string> variable_name, const BasicType variable_type,
		int variable_line, int variable_column, const string* old_value,
		const bool expression_value, const AssignmentType op,
		const ExecutionContext* execution_context, string* &out) {
	return do_op(variable_name, variable_type, variable_line, variable_column,
			old_value, AsString(expression_value), op, execution_context, out);
}

const LinkedList<const Error*>* AssignmentStatement::do_op(
		const_shared_ptr<string> variable_name, const BasicType variable_type,
		int variable_line, int variable_column, const string* old_value,
		const int expression_value, const AssignmentType op,
		const ExecutionContext* execution_context, string* &out) {
	return do_op(variable_name, variable_type, variable_line, variable_column,
			old_value, AsString(expression_value), op, execution_context, out);
}

const LinkedList<const Error*>* AssignmentStatement::do_op(
		const_shared_ptr<string> variable_name, const BasicType variable_type,
		int variable_line, int variable_column, const string* old_value,
		const double expression_value, const AssignmentType op,
		const ExecutionContext* execution_context, string* &out) {
	return do_op(variable_name, variable_type, variable_line, variable_column,
			old_value, AsString(expression_value), op, execution_context, out);
}

const LinkedList<const Error*>* AssignmentStatement::do_op(
		const_shared_ptr<string> variable_name, const BasicType variable_type,
		int variable_line, int variable_column, const string* old_value,
		const string* expression_value, const AssignmentType op,
		const ExecutionContext* execution_context, string* &out) {
	const LinkedList<const Error*>* errors;

	ostringstream buffer;
	switch (op) {
	case PLUS_ASSIGN: {
		errors = LinkedList<const Error*>::GetTerminator();
		buffer << *old_value;
		buffer << *expression_value;
		out = new string(buffer.str());
		break;
	}
	case ASSIGN:
		errors = LinkedList<const Error*>::GetTerminator();
		out = new string(*expression_value);
		break;
	case MINUS_ASSIGN:
	default:
		errors = new LinkedList<const Error*>(
				new Error(Error::SEMANTIC, Error::INVALID_LHS_OF_ASSIGNMENT,
						variable_line, variable_column, *(variable_name),
						type_to_string(variable_type)));
	}

	return errors;
}
const Result* AssignmentStatement::do_op(const_shared_ptr<string> variable_name,
		const BasicType variable_type, int variable_line, int variable_column,
		const int value, const Expression* expression, const AssignmentType op,
		const ExecutionContext* execution_context) {
	const LinkedList<const Error*>* errors;
	const void* result;

	int new_value = 0;
	const Result* evaluation = expression->Evaluate(execution_context);
	if (evaluation->GetErrors() != LinkedList<const Error*>::GetTerminator()) {
		return evaluation;
	}

	auto void_value = evaluation->GetData();
	delete (evaluation);

	const_shared_ptr<TypeSpecifier> expression_type_specifier =
			expression->GetType(execution_context);
	const_shared_ptr<PrimitiveTypeSpecifier> as_primitive =
			std::dynamic_pointer_cast<const PrimitiveTypeSpecifier>(
					expression_type_specifier);

	if (as_primitive != nullptr) {
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
			errors = new LinkedList<const Error*>(
					new Error(Error::SEMANTIC, Error::ASSIGNMENT_TYPE_ERROR,
							variable_line, variable_column, type_to_string(INT),
							expression_type_specifier->ToString()));
		}
	} else {
		errors = new LinkedList<const Error*>(
				new Error(Error::SEMANTIC, Error::ASSIGNMENT_TYPE_ERROR,
						variable_line, variable_column, type_to_string(INT),
						expression_type_specifier->ToString()));
	}

	result = new int(new_value);
	auto wrapper = const_shared_ptr<void>(result);
	return new Result(wrapper, errors);
}

const Result* AssignmentStatement::do_op(const_shared_ptr<string> variable_name,
		const BasicType variable_type, int variable_line, int variable_column,
		const double value, const Expression* expression, AssignmentType op,
		const ExecutionContext* execution_context) {
	const LinkedList<const Error*>* errors;
	const void* result;

	double new_value = 0;
	const Result* evaluation = expression->Evaluate(execution_context);
	if (evaluation->GetErrors() != LinkedList<const Error*>::GetTerminator()) {
		return evaluation;
	}

	auto void_value = evaluation->GetData();
	delete (evaluation);

	const_shared_ptr<TypeSpecifier> expression_type_specifier =
			expression->GetType(execution_context);
	const_shared_ptr<PrimitiveTypeSpecifier> as_primitive =
			std::dynamic_pointer_cast<const PrimitiveTypeSpecifier>(
					expression_type_specifier);

	if (as_primitive != nullptr) {
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
			errors = new LinkedList<const Error*>(
					new Error(Error::SEMANTIC, Error::ASSIGNMENT_TYPE_ERROR,
							variable_line, variable_column,
							type_to_string(DOUBLE),
							expression_type_specifier->ToString()));
		}
	} else {
		errors = new LinkedList<const Error*>(
				new Error(Error::SEMANTIC, Error::ASSIGNMENT_TYPE_ERROR,
						variable_line, variable_column, type_to_string(DOUBLE),
						expression_type_specifier->ToString()));
	}

	result = new double(new_value);
	auto wrapper = const_shared_ptr<void>(result);
	return new Result(wrapper, errors);
}

const Result* AssignmentStatement::do_op(const_shared_ptr<string> variable_name,
		const BasicType variable_type, int variable_line, int variable_column,
		const string* value, const Expression* expression, AssignmentType op,
		const ExecutionContext* execution_context) {
	const LinkedList<const Error*>* errors;

	string* new_value;
	const Result* evaluation = expression->Evaluate(execution_context);
	if (evaluation->GetErrors() != LinkedList<const Error*>::GetTerminator()) {
		return evaluation;
	}

	auto void_value = evaluation->GetData();
	delete (evaluation);

	const_shared_ptr<TypeSpecifier> expression_type_specifier =
			expression->GetType(execution_context);
	const_shared_ptr<PrimitiveTypeSpecifier> as_primitive =
			std::dynamic_pointer_cast<const PrimitiveTypeSpecifier>(
					expression_type_specifier);

	if (as_primitive != nullptr) {
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
					static_pointer_cast<const string>(void_value).get(), op,
					execution_context, new_value);
			break;
		}
		default:
			errors = new LinkedList<const Error*>(
					new Error(Error::SEMANTIC, Error::ASSIGNMENT_TYPE_ERROR,
							variable_line, variable_column,
							type_to_string(STRING),
							expression_type_specifier->ToString()));
		}
	} else {
		errors = new LinkedList<const Error*>(
				new Error(Error::SEMANTIC, Error::ASSIGNMENT_TYPE_ERROR,
						variable_line, variable_column, type_to_string(STRING),
						expression_type_specifier->ToString()));
	}

	auto wrapper = const_shared_ptr<void>(new_value);
	return new Result(wrapper, errors);
}

const LinkedList<const Error*>* AssignmentStatement::execute(
		ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::GetTerminator();

	if (m_variable == nullptr || m_expression == nullptr) {
		assert(false);
	}

	const_shared_ptr<string> variable_name = m_variable->GetName();
	int variable_line = m_variable->GetLocation().begin.line;
	int variable_column = m_variable->GetLocation().begin.column;

	SymbolTable* symbol_table =
			(SymbolTable*) execution_context->GetSymbolContext();
	const Symbol* symbol = symbol_table->GetSymbol(variable_name, DEEP);

	if (symbol == NULL || symbol == Symbol::GetDefaultSymbol()) {
		errors = new LinkedList<const Error*>(
				new Error(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						variable_line, variable_column, *(variable_name)));
	} else {
		errors = m_variable->AssignValue(execution_context, m_expression.get(),
				m_op_type);
	}

	return errors;
}

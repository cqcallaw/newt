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
#include <yyltype.h>
#include <type_specifier.h>
#include <compound_type.h>

AssignmentStatement::AssignmentStatement(const Variable* variable,
		const AssignmentType op_type, const Expression* expression) :
		m_variable(variable), m_op_type(op_type), m_expression(expression) {
}

AssignmentStatement::~AssignmentStatement() {
}

const LinkedList<const Error*>* AssignmentStatement::preprocess(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	SymbolTable* symbol_table =
			(SymbolTable*) execution_context->GetSymbolContext();
	const string* variable_name = m_variable->GetName();
	const Symbol* symbol = symbol_table->GetSymbol(variable_name);
	const TypeSpecifier* symbol_type = symbol->GetType();

	int variable_line = m_variable->GetLocation().first_line;
	int variable_column = m_variable->GetLocation().first_column;

	if (symbol != Symbol::DefaultSymbol) {
		const BasicVariable* basic_variable =
				dynamic_cast<const BasicVariable*>(m_variable);
		if (basic_variable != nullptr) {
			const PrimitiveTypeSpecifier* as_primitive =
					dynamic_cast<const PrimitiveTypeSpecifier*>(symbol_type);
			const TypeSpecifier* expression_type = m_expression->GetType(
					execution_context);
			if (as_primitive != nullptr) {
				if (expression_type->IsAssignableTo(symbol_type)) {
					errors = LinkedList<const Error*>::Terminator;
				} else {
					const YYLTYPE variable_location = m_variable->GetLocation();
					errors = new LinkedList<const Error*>(
							new Error(Error::SEMANTIC,
									Error::ASSIGNMENT_TYPE_ERROR,
									variable_location.first_line,
									variable_location.first_column,
									symbol_type->ToString(),
									expression_type->ToString()));
				}
			}

			const ArrayTypeSpecifier* as_array =
					dynamic_cast<const ArrayTypeSpecifier*>(symbol_type);
			if (as_array != nullptr) {
				//reassigning raw array reference, not an array element
				if (!expression_type->IsAssignableTo(symbol_type)) {
					YYLTYPE expression_position = m_expression->GetPosition();
					errors = new LinkedList<const Error*>(
							new Error(Error::SEMANTIC,
									Error::ASSIGNMENT_TYPE_ERROR,
									expression_position.first_line,
									expression_position.first_column,
									as_array->ToString(),
									expression_type->ToString()));
				}
			}

			const CompoundTypeSpecifier* as_compound =
					dynamic_cast<const CompoundTypeSpecifier*>(symbol_type);
			if (as_compound != nullptr) {
				//reassigning raw struct reference, not a member
				if (!expression_type->IsAssignableTo(symbol_type)) {
					YYLTYPE expression_position = m_expression->GetPosition();
					const string struct_type_name = as_compound->GetTypeName();
					errors = new LinkedList<const Error*>(
							new Error(Error::SEMANTIC,
									Error::ASSIGNMENT_TYPE_ERROR,
									expression_position.first_line,
									expression_position.first_column,
									struct_type_name,
									expression_type->ToString()));
				}
			}
		}

		const ArrayVariable* array_variable =
				dynamic_cast<const ArrayVariable*>(m_variable);
		if (array_variable != nullptr) {
			errors = array_variable->Validate(execution_context);

			const TypeSpecifier* expression_type = m_expression->GetType(
					execution_context);
			const TypeSpecifier* innermost_element_type =
					array_variable->GetInnerMostElementType(execution_context);
			if (!expression_type->IsAssignableTo(innermost_element_type)) {
				YYLTYPE expression_position = m_expression->GetPosition();
				errors = new LinkedList<const Error*>(
						new Error(Error::SEMANTIC, Error::ASSIGNMENT_TYPE_ERROR,
								expression_position.first_line,
								expression_position.first_column,
								innermost_element_type->ToString(),
								expression_type->ToString()));
			}
		}

		const MemberVariable* member_variable =
				dynamic_cast<const MemberVariable*>(m_variable);
		if (member_variable != nullptr) {
			const CompoundTypeSpecifier* as_compound =
					dynamic_cast<const CompoundTypeSpecifier*>(member_variable->GetContainer()->GetType(
							execution_context));

			if (as_compound) {
				const CompoundType* type =
						execution_context->GetTypeTable()->GetType(
								as_compound->GetTypeName());

				if (!(type->GetModifiers() & Modifier::Type::READONLY)) {
					const TypeSpecifier* member_variable_type =
							member_variable->GetType(execution_context);

					if (member_variable_type
							!= PrimitiveTypeSpecifier::GetNone()) {
						const TypeSpecifier* expression_type =
								m_expression->GetType(execution_context);

						if (!expression_type->IsAssignableTo(
								member_variable_type)) {
							errors =
									new LinkedList<const Error*>(
											new Error(Error::SEMANTIC,
													Error::ASSIGNMENT_TYPE_ERROR,
													member_variable->GetContainer()->GetLocation().first_line,
													member_variable->GetContainer()->GetLocation().first_column,
													member_variable_type->ToString(),
													expression_type->ToString()));
						}
					} else {
						errors =
								new LinkedList<const Error*>(
										new Error(Error::SEMANTIC,
												Error::UNDECLARED_MEMBER,
												member_variable->GetMemberVariable()->GetLocation().first_line,
												member_variable->GetMemberVariable()->GetLocation().first_column,
												*member_variable->GetMemberVariable()->GetName(),
												member_variable->GetContainer()->GetType(
														execution_context)->ToString()));
					}
				} else {
					errors =
							errors->With(
									new Error(Error::SEMANTIC, Error::READONLY,
											member_variable->GetContainer()->GetLocation().first_line,
											member_variable->GetContainer()->GetLocation().first_column,
											*variable_name));
				}
			} else {
				errors =
						errors->With(
								new Error(Error::SEMANTIC,
										Error::VARIABLE_NOT_A_COMPOUND_TYPE,
										member_variable->GetContainer()->GetLocation().first_line,
										member_variable->GetContainer()->GetLocation().first_column,
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
		const string* variable_name, const BasicType variable_type,
		int variable_line, int variable_column, const bool old_value,
		const bool expression_value, const AssignmentType op,
		const ExecutionContext* execution_context, bool &out) {
	const LinkedList<const Error*>* errors;
	switch (op) {
	case ASSIGN:
		errors = LinkedList<const Error*>::Terminator;
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
		const string* variable_name, const BasicType variable_type,
		int variable_line, int variable_column, const int old_value,
		const int expression_value, const AssignmentType op,
		const ExecutionContext* execution_context, int& out) {
	const LinkedList<const Error*>* errors;

	switch (op) {
	case PLUS_ASSIGN:
		errors = LinkedList<const Error*>::Terminator;
		out = old_value + expression_value;
		break;
	case MINUS_ASSIGN:
		errors = LinkedList<const Error*>::Terminator;
		out = old_value - expression_value;
		break;
	case ASSIGN:
		errors = LinkedList<const Error*>::Terminator;
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
		const string* variable_name, const BasicType variable_type,
		int variable_line, int variable_column, const double old_value,
		const double expression_value, const AssignmentType op,
		const ExecutionContext* execution_context, double &out) {
	const LinkedList<const Error*>* errors;

	switch (op) {
	case PLUS_ASSIGN:
		errors = LinkedList<const Error*>::Terminator;
		out = old_value + expression_value;
		break;
	case MINUS_ASSIGN:
		errors = LinkedList<const Error*>::Terminator;
		out = old_value - expression_value;
		break;
	case ASSIGN:
		errors = LinkedList<const Error*>::Terminator;
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
		const string* variable_name, const BasicType variable_type,
		int variable_line, int variable_column, const string* old_value,
		const bool expression_value, const AssignmentType op,
		const ExecutionContext* execution_context, string* &out) {
	return do_op(variable_name, variable_type, variable_line, variable_column,
			old_value, AsString(expression_value), op, execution_context, out);
}

const LinkedList<const Error*>* AssignmentStatement::do_op(
		const string* variable_name, const BasicType variable_type,
		int variable_line, int variable_column, const string* old_value,
		const int expression_value, const AssignmentType op,
		const ExecutionContext* execution_context, string* &out) {
	return do_op(variable_name, variable_type, variable_line, variable_column,
			old_value, AsString(expression_value), op, execution_context, out);
}

const LinkedList<const Error*>* AssignmentStatement::do_op(
		const string* variable_name, const BasicType variable_type,
		int variable_line, int variable_column, const string* old_value,
		const double expression_value, const AssignmentType op,
		const ExecutionContext* execution_context, string* &out) {
	return do_op(variable_name, variable_type, variable_line, variable_column,
			old_value, AsString(expression_value), op, execution_context, out);
}

const LinkedList<const Error*>* AssignmentStatement::do_op(
		const string* variable_name, const BasicType variable_type,
		int variable_line, int variable_column, const string* old_value,
		const string* expression_value, const AssignmentType op,
		const ExecutionContext* execution_context, string* &out) {
	const LinkedList<const Error*>* errors;

	ostringstream buffer;
	switch (op) {
	case PLUS_ASSIGN: {
		errors = LinkedList<const Error*>::Terminator;
		buffer << *old_value;
		buffer << *expression_value;
		out = new string(buffer.str());
		break;
	}
	case ASSIGN:
		errors = LinkedList<const Error*>::Terminator;
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
const Result* AssignmentStatement::do_op(const string* variable_name,
		const BasicType variable_type, int variable_line, int variable_column,
		const int value, const Expression* expression, const AssignmentType op,
		const ExecutionContext* execution_context) {
	const LinkedList<const Error*>* errors;
	const void* result;

	int new_value = 0;
	const Result* evaluation = expression->Evaluate(execution_context);
	if (evaluation->GetErrors() != LinkedList<const Error*>::Terminator) {
		return evaluation;
	}

	const void* void_value = evaluation->GetData();
	delete (evaluation);

	const TypeSpecifier* expression_type_specifier = expression->GetType(
			execution_context);
	const PrimitiveTypeSpecifier* as_primitive =
			dynamic_cast<const PrimitiveTypeSpecifier*>(expression_type_specifier);

	if (as_primitive != nullptr) {
		const BasicType basic_type = as_primitive->GetBasicType();
		switch (basic_type) {
		case BOOLEAN: {
			errors = do_op(variable_name, variable_type, variable_line,
					variable_column, value, *((bool*) void_value), op,
					execution_context, new_value);
			break;
		}
		case INT: {
			errors = do_op(variable_name, variable_type, variable_line,
					variable_column, value, *((int*) void_value), op,
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

	return new Result(result, errors);
}

const Result* AssignmentStatement::do_op(const string* variable_name,
		const BasicType variable_type, int variable_line, int variable_column,
		const double value, const Expression* expression, AssignmentType op,
		const ExecutionContext* execution_context) {
	const LinkedList<const Error*>* errors;
	const void* result;

	double new_value = 0;
	const Result* evaluation = expression->Evaluate(execution_context);
	if (evaluation->GetErrors() != LinkedList<const Error*>::Terminator) {
		return evaluation;
	}

	const void* void_value = evaluation->GetData();
	delete (evaluation);

	const TypeSpecifier* expression_type_specifier = expression->GetType(
			execution_context);
	const PrimitiveTypeSpecifier* as_primitive =
			dynamic_cast<const PrimitiveTypeSpecifier*>(expression_type_specifier);

	if (as_primitive != nullptr) {
		const BasicType basic_type = as_primitive->GetBasicType();
		switch (basic_type) {
		case BOOLEAN:
			errors = do_op(variable_name, variable_type, variable_line,
					variable_column, value, *((bool*) void_value), op,
					execution_context, new_value);
			break;
		case INT: {
			errors = do_op(variable_name, variable_type, variable_line,
					variable_column, value, *((int*) void_value), op,
					execution_context, new_value);
			break;
		}
		case DOUBLE: {
			errors = do_op(variable_name, variable_type, variable_line,
					variable_column, value, *((double*) void_value), op,
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

	return new Result(result, errors);
}

const Result* AssignmentStatement::do_op(const string* variable_name,
		const BasicType variable_type, int variable_line, int variable_column,
		const string* value, const Expression* expression, AssignmentType op,
		const ExecutionContext* execution_context) {
	const LinkedList<const Error*>* errors;

	string* new_value;
	const Result* evaluation = expression->Evaluate(execution_context);
	if (evaluation->GetErrors() != LinkedList<const Error*>::Terminator) {
		return evaluation;
	}

	const void* void_value = evaluation->GetData();
	delete (evaluation);

	const TypeSpecifier* expression_type_specifier = expression->GetType(
			execution_context);
	const PrimitiveTypeSpecifier* as_primitive =
			dynamic_cast<const PrimitiveTypeSpecifier*>(expression_type_specifier);

	if (as_primitive != nullptr) {
		const BasicType basic_type = as_primitive->GetBasicType();
		switch (basic_type) {
		case BOOLEAN:
			errors = do_op(variable_name, variable_type, variable_line,
					variable_column, value, *((bool*) void_value), op,
					execution_context, new_value);
			break;
		case INT: {
			errors = do_op(variable_name, variable_type, variable_line,
					variable_column, value, *((int*) void_value), op,
					execution_context, new_value);
			break;
		}
		case DOUBLE: {
			errors = do_op(variable_name, variable_type, variable_line,
					variable_column, value, *((double*) void_value), op,
					execution_context, new_value);
			break;
		}
		case STRING: {
			errors = do_op(variable_name, variable_type, variable_line,
					variable_column, value, (string*) void_value, op,
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

	return new Result(new_value, errors);
}

const LinkedList<const Error*>* AssignmentStatement::execute(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors;

	if (m_variable == nullptr || m_expression == nullptr) {
		assert(false);
	}

	const string* variable_name = m_variable->GetName();
	int variable_line = m_variable->GetLocation().first_line;
	int variable_column = m_variable->GetLocation().first_column;

	SymbolTable* symbol_table =
			(SymbolTable*) execution_context->GetSymbolContext();
	const Symbol* symbol = symbol_table->GetSymbol(variable_name);

	if (symbol == NULL || symbol == Symbol::DefaultSymbol) {
		errors = new LinkedList<const Error*>(
				new Error(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						variable_line, variable_column, *(variable_name)));
	} else {
		errors = m_variable->AssignValue(execution_context, m_expression,
				m_op_type);
	}

	return errors;
}

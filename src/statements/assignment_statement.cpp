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
#include <variable.h>
#include <array_variable.h>
#include <compound_type_instance.h>
#include <error.h>
#include <defaults.h>
#include <execution_context.h>
#include <typeinfo>
#include <member_variable.h>
#include <variable_expression.h>
#include <yyltype.h>
#include <type_specifier.h>

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
	const Symbol* symbol = symbol_table->GetSymbol(m_variable->GetName());
	const TypeSpecifier* symbol_type = symbol->GetType();

	int variable_line = m_variable->GetLocation().first_line;
	int variable_column = m_variable->GetLocation().first_column;

	if (symbol == Symbol::DefaultSymbol) {
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						variable_line, variable_column,
						*m_variable->GetName()));
	} else {
		if (m_variable->IsBasicReference()) {
			const PrimitiveTypeSpecifier* as_primitive =
					dynamic_cast<const PrimitiveTypeSpecifier*>(symbol_type);
			if (as_primitive != nullptr) {
				const TypeSpecifier* expression_type = m_expression->GetType(
						execution_context);
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

			//TODO: array variable re-assignment

			const CompoundTypeSpecifier* as_compound =
					dynamic_cast<const CompoundTypeSpecifier*>(symbol_type);

			if (as_compound != nullptr) {
				//reassigning struct reference
				YYLTYPE expression_position = m_expression->GetPosition();
				const CompoundTypeInstance* as_struct =
						(const CompoundTypeInstance*) symbol->GetValue();
				const string struct_type_name =
						as_struct->GetTypeSpecifier()->GetTypeName();
				const TypeSpecifier* expression_type = m_expression->GetType(
						execution_context);

				if (m_expression->IsConstant()) {
					errors = new LinkedList<const Error*>(
							new Error(Error::SEMANTIC,
									Error::ASSIGNMENT_TYPE_ERROR,
									expression_position.first_line,
									expression_position.first_column,
									struct_type_name,
									expression_type->ToString()));
				} else {
					const VariableExpression* variable_expression =
							(VariableExpression*) m_expression;
					errors = variable_expression->Validate(execution_context);

					if (errors == LinkedList<const Error*>::Terminator) {
						const CompoundTypeSpecifier* expression_type_as_compound =
								dynamic_cast<const CompoundTypeSpecifier*>(expression_type);

						if (expression_type_as_compound != nullptr) {
							const Result* evaluation =
									variable_expression->Evaluate(
											execution_context);

							errors = evaluation->GetErrors();
							if (errors
									== LinkedList<const Error*>::Terminator) {
								const CompoundTypeInstance* other_as_struct =
										(const CompoundTypeInstance*) evaluation->GetData();
								const string other_struct_type_name =
										other_as_struct->GetTypeSpecifier()->GetTypeName();
								if (struct_type_name.compare(
										other_struct_type_name) != 0) {
									errors =
											new LinkedList<const Error*>(
													new Error(Error::SEMANTIC,
															Error::ASSIGNMENT_TYPE_ERROR,
															expression_position.first_line,
															expression_position.first_column,
															struct_type_name,
															other_struct_type_name));
								}
							}
						} else {
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
			}
		} else {
			const ArrayVariable* array_variable =
					dynamic_cast<const ArrayVariable*>(m_variable);

			if (array_variable != nullptr) {
				const string* variable_name = m_variable->GetName();
				const Expression* array_index_expression =
						array_variable->GetIndexExpression();
				const YYLTYPE array_index_expression_position =
						array_index_expression->GetPosition();
				const TypeSpecifier* index_type =
						array_index_expression->GetType(execution_context);

				const PrimitiveTypeSpecifier* as_index_type_primitive =
						dynamic_cast<const PrimitiveTypeSpecifier*>(index_type);

				if (as_index_type_primitive == nullptr
						|| as_index_type_primitive->GetBasicType()
								!= BasicType::INT) {
					ostringstream os;
					os << "A " << index_type->ToString() << " expression";

					errors =
							new LinkedList<const Error*>(
									new Error(Error::SEMANTIC,
											Error::ARRAY_INDEX_MUST_BE_AN_INTEGER,
											array_index_expression_position.first_line,
											array_index_expression_position.first_column,
											*variable_name, os.str()));
				} else {
					errors = LinkedList<const Error*>::Terminator;
				}
			}

			const MemberVariable* member_variable =
					dynamic_cast<const MemberVariable*>(m_variable);

			if (member_variable != nullptr) {
				const MemberVariable* as_member_variable =
						(const MemberVariable*) m_variable;
				const YYLTYPE variable_location =
						as_member_variable->GetLocation();

				const TypeSpecifier* member_variable_type =
						as_member_variable->GetType(execution_context);
				const TypeSpecifier* expression_type = m_expression->GetType(
						execution_context);
				if (member_variable_type->IsAssignableTo(expression_type)) {
					errors = LinkedList<const Error*>::Terminator;
				} else {
					errors = new LinkedList<const Error*>(
							new Error(Error::SEMANTIC,
									Error::ASSIGNMENT_TYPE_ERROR,
									variable_location.first_line,
									variable_location.first_column,
									member_variable_type->ToString(),
									expression_type->ToString()));
				}
			}
		}
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

const LinkedList<const Error*>* AssignmentStatement::do_op(
		const Variable* variable, const Expression* expression,
		const AssignmentType op, const ExecutionContext* execution_context) {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;
	SetResult set_result = NO_SET_RESULT;

	const string* variable_name = variable->GetName();
	const int variable_line = variable->GetLocation().first_line;
	const int variable_column = variable->GetLocation().first_column;

	SymbolTable* symbol_table =
			(SymbolTable*) execution_context->GetSymbolContext();
	const Symbol* symbol = symbol_table->GetSymbol(variable_name);
	const TypeSpecifier* symbol_type = symbol->GetType();
	const void* symbol_value = symbol->GetValue();

	if (variable->IsBasicReference()) {
		const PrimitiveTypeSpecifier* as_primitive =
				dynamic_cast<const PrimitiveTypeSpecifier*>(symbol_type);
		if (as_primitive != nullptr) {
			const BasicType basic_type = as_primitive->GetBasicType();
			switch (basic_type) {
			case BOOLEAN: {
				const Result* result = do_op(variable_name, basic_type,
						variable_line, variable_column, *((int*) symbol_value),
						expression, op, execution_context);

				errors = result->GetErrors();
				if (errors == LinkedList<const Error*>::Terminator) {
					set_result = symbol_table->SetSymbol(*variable_name,
							(bool*) result->GetData());
				}
				break;
			}
			case INT: {
				const Result* result = do_op(variable_name, basic_type,
						variable_line, variable_column, *((int*) symbol_value),
						expression, op, execution_context);

				errors = result->GetErrors();
				if (errors == LinkedList<const Error*>::Terminator) {
					set_result = symbol_table->SetSymbol(*variable_name,
							(int*) result->GetData());
				}
				break;
			}
			case DOUBLE: {
				const Result* result = do_op(variable_name, basic_type,
						variable_line, variable_column,
						*((double*) symbol_value), expression, op,
						execution_context);

				errors = result->GetErrors();
				if (errors == LinkedList<const Error*>::Terminator) {
					set_result = symbol_table->SetSymbol(*variable_name,
							(double*) result->GetData());
				}
				break;
			}
			case STRING: {
				const Result* result = do_op(variable_name, basic_type,
						variable_line, variable_column, (string*) symbol_value,
						expression, op, execution_context);
				errors = result->GetErrors();
				if (errors == LinkedList<const Error*>::Terminator) {
					set_result = symbol_table->SetSymbol(*variable_name,
							(string*) result->GetData());
				}
				break;
			}
			default:
				assert(false);
			}
		}

		//TODO: basic array assignment

		const CompoundTypeSpecifier* as_compound =
				dynamic_cast<const CompoundTypeSpecifier*>(symbol_type);
		if (as_compound != nullptr) {
			const Result* expression_evaluation = expression->Evaluate(
					execution_context);

			errors = expression_evaluation->GetErrors();
			if (errors == LinkedList<const Error*>::Terminator) {
				const CompoundTypeInstance* new_instance =
						(const CompoundTypeInstance*) expression_evaluation->GetData();
				//we're assigning a struct reference
				SetResult set_result = symbol_table->SetSymbol(*variable_name,
						new_instance);

				switch (set_result) {
				case SET_SUCCESS:
					break;
				case UNDEFINED_SYMBOL: {
					errors = new LinkedList<const Error*>(
							new Error(Error::SEMANTIC,
									Error::UNDECLARED_VARIABLE, variable_line,
									variable_column, *variable_name));
					break;
				}
				case NO_SET_RESULT:
				case INCOMPATIBLE_TYPE:
				default:
					assert(false);
				}
			}
		}
	} else {
		const ArrayVariable* array_variable =
				dynamic_cast<const ArrayVariable*>(variable);
		if (array_variable != nullptr) {
			const ArraySymbol* array_symbol =
					dynamic_cast<const ArraySymbol*>(symbol);

			if (array_symbol == nullptr) {
				errors = errors->With(
						new Error(Error::SEMANTIC, Error::VARIABLE_NOT_AN_ARRAY,
								variable->GetLocation().first_line,
								variable->GetLocation().first_column,
								*variable->GetName()));
			} else {
				const Result* evaluation =
						array_variable->GetIndexExpression()->Evaluate(
								execution_context);

				errors = evaluation->GetErrors();
				if (errors == LinkedList<const Error*>::Terminator) {
					int index = *((int*) evaluation->GetData());
					delete (evaluation);

					if ((array_symbol->IsFixedSize()
							&& index >= array_symbol->GetSize()) || index < 0) {
						errors = new LinkedList<const Error*>(
								new Error(Error::SEMANTIC,
										Error::ARRAY_INDEX_OUT_OF_BOUNDS,
										variable_line, variable_column,
										array_symbol->GetName(),
										*AsString(index)));
					} else {
						const TypeSpecifier* element_type_specifier =
								array_variable->GetType(execution_context);
						const PrimitiveTypeSpecifier* element_as_primitive =
								dynamic_cast<const PrimitiveTypeSpecifier*>(element_type_specifier);

						if (element_as_primitive != nullptr) {
							const BasicType element_type =
									element_as_primitive->GetBasicType();

							switch (element_type) {
							case INT: {
								const Result* result = do_op(variable_name,
										element_type, variable_line,
										variable_column,
										*(((int*) symbol_value) + index),
										expression, op, execution_context);

								errors = result->GetErrors();
								if (errors
										== LinkedList<const Error*>::Terminator) {
									set_result = symbol_table->SetSymbol(
											*variable_name, index,
											(int*) result->GetData(),
											execution_context->GetTypeTable());
									break;
								}
								break;
							}
							case DOUBLE: {
								const Result* result = do_op(variable_name,
										element_type, variable_line,
										variable_column,
										*(((double*) symbol_value) + index),
										expression, op, execution_context);

								errors = result->GetErrors();
								if (errors
										== LinkedList<const Error*>::Terminator) {
									set_result = symbol_table->SetSymbol(
											*variable_name, index,
											(double*) result->GetData(),
											execution_context->GetTypeTable());
								}
								break;
							}
							case STRING: {
								const Result* result = do_op(variable_name,
										element_type, variable_line,
										variable_column,
										*(((string**) symbol_value) + index),
										expression, op, execution_context);

								errors = result->GetErrors();
								if (errors
										== LinkedList<const Error*>::Terminator) {
									set_result = symbol_table->SetSymbol(
											*variable_name, index,
											(string*) result->GetData(),
											execution_context->GetTypeTable());
								}
								break;
							}
							default:
								assert(false);
							}
						}
						//TODO: other array element types
					}
				}
			}
		}

		const MemberVariable* member_variable =
				dynamic_cast<const MemberVariable*>(variable);
		if (member_variable != nullptr) {
			const Result* expression_evaluation = expression->Evaluate(
					execution_context);
			errors = expression_evaluation->GetErrors();

			if (errors == LinkedList<const Error*>::Terminator) {
				//we're assigning a struct member reference
				const MemberVariable* member_variable =
						(const MemberVariable*) variable;

				const CompoundTypeInstance* struct_value =
						(const CompoundTypeInstance*) symbol_value;
				const ExecutionContext* new_context =
						execution_context->WithSymbolContext(
								struct_value->GetDefinition());
				const Variable* new_variable =
						member_variable->GetMemberVariable();

				errors = do_op(new_variable, expression, op, new_context);
			}
		}
	}

	if (set_result != NO_SET_RESULT) {
		assert(set_result == SET_SUCCESS);
	}

	return errors;
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
		/*} else if (dynamic_cast<const ArrayVariable*>(m_variable) == NULL
		 && (symbol->GetType() & (ARRAY))) {
		 //we're trying to reference an array variable without an index
		 //(this probably isn't legitimate and may need to be revised for the statement "var arr1 = arr2")
		 errors = new LinkedList<const Error*>(
		 new Error(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
		 variable_line, variable_column, *(variable_name)));*/
	} else if (dynamic_cast<const ArrayVariable*>(m_variable) != nullptr
			&& !(dynamic_cast<const ArrayTypeSpecifier*>(symbol->GetType())
					== nullptr)) {
//trying to reference a non-array symbol as an array.
		errors = new LinkedList<const Error*>(
				new Error(Error::SEMANTIC, Error::VARIABLE_NOT_AN_ARRAY,
						variable_line, variable_column, *(variable_name)));
	} else {
		errors = do_op(m_variable, m_expression, m_op_type, execution_context);
	}

	return errors;
}


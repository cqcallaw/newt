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

#include <array_variable.h>
#include <expression.h>
#include <index_list.h>
#include <sstream>

#include "yyltype.h"
#include "assert.h"
#include "execution_context.h"
#include <index_list.h>
#include <index.h>
#include <assignment_statement.h>

ArrayVariable::ArrayVariable(const string* name,
YYLTYPE location, const IndexList* index_list,
YYLTYPE expression_location) :
		Variable(name, location), m_index_list(index_list), m_index_list_location(
				expression_location) {
}

const TypeSpecifier* ArrayVariable::GetType(
		const ExecutionContext* context) const {
	const Symbol* symbol = context->GetSymbolContext()->GetSymbol(GetName());
	if (symbol != nullptr && symbol != Symbol::DefaultSymbol) {
		const Array* array = static_cast<const Array*>(symbol->GetValue());
		const TypeSpecifier* result = array->GetTypeSpecifier();

		//handle intermediate indices
		const LinkedList<const Index*>* subject = m_index_list;
		while (!subject->IsTerminator()) {
			const ArrayTypeSpecifier* as_array_specifier =
					dynamic_cast<const ArrayTypeSpecifier*>(result);
			if (as_array_specifier) {
				result = as_array_specifier->GetElementTypeSpecifier();
				subject = subject->GetNext();
			} else {
				return PrimitiveTypeSpecifier::GetNone();
			}
		}
		return array->GetElementType();
	} else {
		return PrimitiveTypeSpecifier::GetNone();
	}
}

const string* ArrayVariable::ToString(const ExecutionContext* context) const {
	ostringstream buffer;
	buffer << "<" << *GetName();
	const LinkedList<const Index*>* subject = m_index_list;
	while (!subject->IsTerminator()) {
		buffer << "[";
		const Index* index = subject->GetData();
		const Expression* index_expression = index->GetIndexExpression();
		const Result* evaluation = index_expression->Evaluate(context);

		auto errors = evaluation->GetErrors();
		if (errors->IsTerminator()) {
			buffer << *((int*) evaluation->GetData());
		} else {
			buffer << "EVALUATION ERROR";
			break;
		}
		delete (evaluation);

		buffer << "]";
		subject = subject->GetNext();
	}
	buffer << ">";
	return new string(buffer.str());
}

ArrayVariable::~ArrayVariable() {
}

const ArrayVariable::ValidationResult* ArrayVariable::ValidateOperation(
		const ExecutionContext* context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	const SymbolContext* symbol_context = context->GetSymbolContext();
	const Symbol* symbol = symbol_context->GetSymbol(GetName());
	const TypeTable* type_table = context->GetTypeTable();
	int array_index = -1;
	YYLTYPE index_location = DefaultLocation;
	//this cast may not be valid, but is guaranteed to be unused if it's not valid.
	const Array* array = nullptr;

	if (symbol != nullptr && symbol != Symbol::DefaultSymbol) {
		const TypeSpecifier* type_specifier = symbol->GetType();
		const LinkedList<const Index*>* subject = m_index_list;

		//consume all intermediate indices and return the inner-most array and the index.
		while (!subject->IsTerminator()) {
			const ArrayTypeSpecifier* as_array_specifier =
					dynamic_cast<const ArrayTypeSpecifier*>(type_specifier);
			if (as_array_specifier) {
				const Index* index = subject->GetData();
				const Expression* index_expression =
						index->GetIndexExpression();
				const TypeSpecifier* index_expression_type =
						index_expression->GetType(context);
				if (index_expression->GetType(context)->IsAssignableTo(
						PrimitiveTypeSpecifier::GetInt())) {
					const Result* index_expression_evaluation =
							index_expression->Evaluate(context);
					errors = index_expression_evaluation->GetErrors();
					if (errors == LinkedList<const Error*>::Terminator) {
						const int i =
								*((int*) index_expression_evaluation->GetData());

						if (i >= 0) {
							//our validations have passed. set everything up for the next level of indexing, or for termination
							type_specifier =
									as_array_specifier->GetElementTypeSpecifier();
							if (array == nullptr) {
								array =
										static_cast<const Array*>(symbol->GetValue());
							} else {
								array = array->GetValue<const Array*>(
										array_index, type_table);
							}
							array_index = i;
							index_location = index_expression->GetPosition();
							subject = subject->GetNext();
						} else {
							errors =
									errors->With(
											new Error(Error::SEMANTIC,
													Error::ARRAY_INDEX_OUT_OF_BOUNDS,
													index_expression->GetPosition().first_line,
													index_expression->GetPosition().first_column,
													symbol->GetName(),
													*AsString(index)));
							break;
						}
					}
					delete (index_expression_evaluation);
				} else {
					ostringstream buffer;
					buffer << "A " << index_expression_type->ToString()
							<< " expression";
					errors = errors->With(
							new Error(Error::SEMANTIC,
									Error::ARRAY_INDEX_MUST_BE_AN_INTEGER,
									index_location.first_line,
									index_location.first_column, *(GetName()),
									buffer.str()));
				}
			} else {
				errors = errors->With(
						new Error(Error::SEMANTIC, Error::VARIABLE_NOT_AN_ARRAY,
								GetLocation().first_line,
								GetLocation().first_column, *(GetName())));
			}
		}
	} else {
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						GetLocation().first_line, GetLocation().first_column,
						*(GetName())));
	}

	return new ValidationResult(array, array_index, index_location, errors);
}

const Result* ArrayVariable::Evaluate(const ExecutionContext* context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	const ValidationResult* validation_result = ValidateOperation(context);
	const void* result_value = nullptr;
	errors = validation_result->GetErrors();
	if (errors == LinkedList<const Error*>::Terminator) {
		const Array* array = validation_result->GetArray();
		const int index = validation_result->GetIndex();
		const int size = array->GetSize();
		if (index < size && index >= 0) {
			const TypeTable* type_table = context->GetTypeTable();
			const TypeSpecifier* element_type_specifier =
					array->GetElementType();
			if (element_type_specifier->IsAssignableTo(
					PrimitiveTypeSpecifier::GetBoolean())) {
				result_value = array->GetValue<const bool*>(index, type_table);
			} else if (element_type_specifier->IsAssignableTo(
					PrimitiveTypeSpecifier::GetInt())) {
				result_value = array->GetValue<const int*>(index, type_table);
			} else if (element_type_specifier->IsAssignableTo(
					PrimitiveTypeSpecifier::GetDouble())) {
				result_value = array->GetValue<const double*>(index,
						type_table);
			} else if (element_type_specifier->IsAssignableTo(
					PrimitiveTypeSpecifier::GetString())) {
				result_value = array->GetValue<const string*>(index,
						type_table);
			} else {
				const ArrayTypeSpecifier* as_array =
						dynamic_cast<const ArrayTypeSpecifier*>(element_type_specifier);
				if (as_array != nullptr) {
					//TODO
					assert(false);
				}

				const CompoundTypeSpecifier* as_compound =
						dynamic_cast<const CompoundTypeSpecifier*>(element_type_specifier);
				if (as_compound != nullptr) {
					result_value = array->GetValue<const CompoundTypeInstance*>(
							index, type_table);
				} else {
					//we should never get here
					assert(false);
				}
			}
		} else {
			const YYLTYPE index_location =
					validation_result->GetIndexLocation();
			ostringstream buffer;
			buffer << index;
			errors = errors->With(
					new Error(Error::SEMANTIC, Error::ARRAY_INDEX_OUT_OF_BOUNDS,
							index_location.first_line,
							index_location.first_column, *GetName(),
							buffer.str()));
		}
	}
	delete (validation_result);

	const Result* result = new Result(result_value, errors);
	return result;
}

const LinkedList<const Error*>* ArrayVariable::AssignValue(
		const ExecutionContext* context, const Expression* expression,
		const AssignmentType op) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	const string* variable_name = GetName();
	const int variable_line = GetLocation().first_line;
	const int variable_column = GetLocation().first_column;

	const ValidationResult* validation_result = ValidateOperation(context);
	errors = validation_result->GetErrors();
	if (errors == LinkedList<const Error*>::Terminator) {
		const Array* array = validation_result->GetArray();
		const int index = validation_result->GetIndex();
		delete (validation_result);

		if (index >= 0) {
			const TypeSpecifier* element_type_specifier =
					array->GetElementType();
			const PrimitiveTypeSpecifier* element_as_primitive =
					dynamic_cast<const PrimitiveTypeSpecifier*>(element_type_specifier);

			const TypeTable* type_table = context->GetTypeTable();
			if (element_as_primitive != nullptr) {
				const BasicType element_type =
						element_as_primitive->GetBasicType();

				switch (element_type) {
				case BOOLEAN: {
					const bool* value = array->GetValue<const bool*>(index,
							type_table);
					const Result* result = AssignmentStatement::do_op(
							variable_name, element_type, variable_line,
							variable_column, *value, expression, op, context);

					errors = result->GetErrors();
					if (errors == LinkedList<const Error*>::Terminator) {
						errors = SetSymbol(context, (bool*) result->GetData());
						break;
					}
					break;
				}
				case INT: {
					const int* value = array->GetValue<const int*>(index,
							type_table);
					const Result* result = AssignmentStatement::do_op(
							variable_name, element_type, variable_line,
							variable_column, *value, expression, op, context);

					errors = result->GetErrors();
					if (errors == LinkedList<const Error*>::Terminator) {
						errors = SetSymbol(context, (int*) result->GetData());
						break;
					}
					break;
				}
				case DOUBLE: {
					const double* value = array->GetValue<const double*>(index,
							type_table);
					const Result* result = AssignmentStatement::do_op(
							variable_name, element_type, variable_line,
							variable_column, *value, expression, op, context);

					errors = result->GetErrors();
					if (errors == LinkedList<const Error*>::Terminator) {
						errors = SetSymbol(context,
								(double*) result->GetData());
					}
					break;
				}
				case STRING: {
					const string* value = array->GetValue<const string*>(index,
							type_table);
					const Result* result = AssignmentStatement::do_op(
							variable_name, element_type, variable_line,
							variable_column, value, expression, op, context);

					errors = result->GetErrors();
					if (errors == LinkedList<const Error*>::Terminator) {
						errors = SetSymbol(context,
								(string*) result->GetData());
					}
					break;
				}
				default:
					assert(false);
				}
			} else {
				if (op == AssignmentType::ASSIGN) {
					const Result* result = expression->Evaluate(context);
					errors = result->GetErrors();
					if (errors->IsTerminator()) {
						const ArrayTypeSpecifier* element_as_array =
								dynamic_cast<const ArrayTypeSpecifier*>(element_type_specifier);
						if (element_as_array) {
							errors = SetSymbol(context,
									(const Array*) result->GetData());
						}

						const CompoundTypeSpecifier* element_as_compound =
								dynamic_cast<const CompoundTypeSpecifier*>(element_type_specifier);
						if (element_as_compound) {
							errors =
									SetSymbol(context,
											(const CompoundTypeInstance*) result->GetData());
						}
					}
				} else {
					errors = new LinkedList<const Error*>(
							new Error(Error::SEMANTIC,
									Error::ASSIGNMENT_TYPE_ERROR, variable_line,
									variable_column,
									expression->GetType(context)->ToString(),
									element_type_specifier->ToString()));
				}
			}
		} else {
			errors = new LinkedList<const Error*>(
					new Error(Error::SEMANTIC, Error::ARRAY_INDEX_OUT_OF_BOUNDS,
							variable_line, variable_column, *GetName(),
							*AsString(index)));
		}
	}

	return errors;
}

const LinkedList<const Error*>* ArrayVariable::SetSymbolCore(
		const ExecutionContext* context, const void* value) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	const ValidationResult* validation_result = ValidateOperation(context);
	errors = validation_result->GetErrors();
	if (errors == LinkedList<const Error*>::Terminator) {
		const Array* array = validation_result->GetArray();
		const int index = validation_result->GetIndex();

		const TypeTable* type_table = context->GetTypeTable();
		const TypeSpecifier* element_type_specifier = array->GetElementType();
		const Array* new_array = nullptr;

		if (element_type_specifier->IsAssignableTo(
				PrimitiveTypeSpecifier::GetBoolean())) {
			new_array = array->WithValue<const bool*>(index,
					(const bool*) value, type_table);
		} else if (element_type_specifier->IsAssignableTo(
				PrimitiveTypeSpecifier::GetInt())) {
			new_array = array->WithValue<const int*>(index, (const int*) value,
					type_table);
		} else if (element_type_specifier->IsAssignableTo(
				PrimitiveTypeSpecifier::GetDouble())) {
			new_array = array->WithValue<const double*>(index,
					(const double*) value, type_table);
		} else if (element_type_specifier->IsAssignableTo(
				PrimitiveTypeSpecifier::GetString())) {
			new_array = array->WithValue<const string*>(index,
					(const string*) value, type_table);
		} else {
			const ArrayTypeSpecifier* as_array =
					dynamic_cast<const ArrayTypeSpecifier*>(element_type_specifier);
			if (as_array != nullptr) {
				new_array = array->WithValue<const Array*>(index,
						(const Array*) value, type_table);
			}

			const CompoundTypeSpecifier* as_compound =
					dynamic_cast<const CompoundTypeSpecifier*>(element_type_specifier);
			if (as_compound != nullptr) {
				new_array = array->WithValue<const CompoundTypeInstance*>(index,
						(const CompoundTypeInstance*) value, type_table);
			}
		}

		const SetResult set_result = context->GetSymbolContext()->SetSymbol(
				*GetName(), new_array);

		errors = ToErrorList(set_result);
	}
	delete (validation_result);

	return errors;
}

const LinkedList<const Error*>* ArrayVariable::SetSymbol(
		const ExecutionContext* context, const bool* value) const {
	return SetSymbolCore(context, (const void*) value);
}

const LinkedList<const Error*>* ArrayVariable::SetSymbol(
		const ExecutionContext* context, const int* value) const {
	return SetSymbolCore(context, (const void*) value);
}

const LinkedList<const Error*>* ArrayVariable::SetSymbol(
		const ExecutionContext* context, const double* value) const {
	return SetSymbolCore(context, (const void*) value);
}

const LinkedList<const Error*>* ArrayVariable::SetSymbol(
		const ExecutionContext* context, const string* value) const {
	return SetSymbolCore(context, (const void*) value);
}

const LinkedList<const Error*>* ArrayVariable::SetSymbol(
		const ExecutionContext* context, const Array* value) const {
	return SetSymbolCore(context, (const void*) value);
}

const LinkedList<const Error*>* ArrayVariable::SetSymbol(
		const ExecutionContext* context,
		const CompoundTypeInstance* value) const {
	return SetSymbolCore(context, (const void*) value);
}

const LinkedList<const Error*>* ArrayVariable::Validate(
		const ExecutionContext* context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	const SymbolContext* symbol_context = context->GetSymbolContext();
	const Symbol* symbol = symbol_context->GetSymbol(GetName());

	if (symbol != nullptr && symbol != Symbol::DefaultSymbol) {
		const TypeSpecifier* type_specifier = symbol->GetType();

		const LinkedList<const Index*>* subject = m_index_list;
		while (!subject->IsTerminator()) {
			const Index* index = subject->GetData();
			const Expression* index_expression = index->GetIndexExpression();
			const TypeSpecifier* index_expression_type =
					index_expression->GetType(context);
			if (index_expression->GetType(context)->IsAssignableTo(
					PrimitiveTypeSpecifier::GetInt())) {
				const ArrayTypeSpecifier* as_array =
						dynamic_cast<const ArrayTypeSpecifier*>(type_specifier);
				if (as_array) {
					type_specifier = as_array->GetElementTypeSpecifier();
				} else {
					errors =
							errors->With(
									new Error(Error::SEMANTIC,
											Error::VARIABLE_NOT_AN_ARRAY,
											index_expression->GetPosition().first_line,
											index_expression->GetPosition().first_column,
											*(GetName())));
				}
			} else {
				ostringstream buffer;
				buffer << "A " << index_expression_type->ToString()
						<< " expression";
				errors = errors->With(
						new Error(Error::SEMANTIC,
								Error::ARRAY_INDEX_MUST_BE_AN_INTEGER,
								index_expression->GetPosition().first_line,
								index_expression->GetPosition().first_column,
								*(GetName()), buffer.str()));
			}

			subject = subject->GetNext();
		}
	} else {
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						GetLocation().first_line, GetLocation().first_column,
						*(GetName())));
	}

	return errors;
}

const TypeSpecifier* ArrayVariable::GetInnerMostElementType(
		const ExecutionContext* context) const {
	const Symbol* symbol = context->GetSymbolContext()->GetSymbol(GetName());
	const TypeSpecifier* type_specifier = PrimitiveTypeSpecifier::GetNone();
	if (symbol != nullptr && symbol != Symbol::DefaultSymbol) {
		type_specifier = symbol->GetType();
		//const string str = type_specifier->ToString();

		const LinkedList<const Index*>* subject = m_index_list;
		while (!subject->IsTerminator()) {
			const ArrayTypeSpecifier* as_array =
					dynamic_cast<const ArrayTypeSpecifier*>(type_specifier);
			if (as_array) {
				type_specifier = as_array->GetElementTypeSpecifier();
			} else {
				break;
			}
			subject = subject->GetNext();
		}
	}

//const string str = type_specifier->ToString();
	return type_specifier;
}

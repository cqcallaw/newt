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
#include <sstream>

#include "yyltype.h"
#include "assert.h"
#include "execution_context.h"

ArrayVariable::ArrayVariable(const string* name,
YYLTYPE location, const Expression* index_expression,
YYLTYPE expression_location) :
		Variable(name, location), m_index_expression(index_expression), m_expression_location(
				expression_location) {
	/*assert(index_expression != NULL && index_expression != nullptr);assert(
	 index_expression->GetType() == INT);*/
}

const TypeSpecifier* ArrayVariable::GetType(
		const ExecutionContext* context) const {
	const Symbol* symbol = context->GetSymbolContext()->GetSymbol(GetName());
	const ArraySymbol* as_array_symbol = (const ArraySymbol*) symbol;
	return as_array_symbol->GetElementType();
}

const string* ArrayVariable::ToString(const ExecutionContext* context) const {
	ostringstream buffer;
	const Result* evaluation = m_index_expression->Evaluate(context);
	buffer << "<" << *GetName() << "[" << *((int*) evaluation->GetData())
			<< "]>";
	delete (evaluation);
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
	int index = -1;
	const ArraySymbol* array_symbol = nullptr;

	if (symbol != nullptr && symbol != Symbol::DefaultSymbol) {
		array_symbol = dynamic_cast<const ArraySymbol*>(symbol);
		if (array_symbol != nullptr) {
			const TypeSpecifier* index_expression_type =
					m_index_expression->GetType(context);
			const YYLTYPE index_location = m_index_expression->GetPosition();
			if (m_index_expression->GetType(context)->IsAssignableTo(
					PrimitiveTypeSpecifier::GetInt())) {
				const Result* index_expression_evaluation =
						m_index_expression->Evaluate(context);
				errors = index_expression_evaluation->GetErrors();
				if (errors == LinkedList<const Error*>::Terminator) {
					index = *((int*) index_expression_evaluation->GetData());

					if (array_symbol->IsFixedSize()
							&& (index > array_symbol->GetSize() || index < 0)) {
						errors = errors->With(
								new Error(Error::SEMANTIC,
										Error::ARRAY_INDEX_OUT_OF_BOUNDS,
										index_location.first_line,
										index_location.first_column,
										symbol->GetName(), *AsString(index)));
					}
					delete (index_expression_evaluation);
				}
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
	} else {
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						GetLocation().first_line, GetLocation().first_column,
						*(GetName())));
	}

	return new ValidationResult(array_symbol, index, errors);
}

const Result* ArrayVariable::Evaluate(const ExecutionContext* context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	const ValidationResult* validation_result = ValidateOperation(context);
	const void* result_value = nullptr;
	errors = validation_result->GetErrors();
	if (errors == LinkedList<const Error*>::Terminator) {
		const ArraySymbol* symbol_as_array = validation_result->GetSymbol();
		const int index = validation_result->GetIndex();
		const int size = symbol_as_array->GetSize();
		if (index < size && index >= 0) {
			const TypeTable* type_table = context->GetTypeTable();
			const TypeSpecifier* element_type_specifier =
					symbol_as_array->GetElementType();
			if (element_type_specifier->IsAssignableTo(
					PrimitiveTypeSpecifier::GetBoolean())) {
				result_value = symbol_as_array->GetValue<const bool*>(index,
						type_table);
			} else if (element_type_specifier->IsAssignableTo(
					PrimitiveTypeSpecifier::GetInt())) {
				result_value = symbol_as_array->GetValue<const int*>(index,
						type_table);
			} else if (element_type_specifier->IsAssignableTo(
					PrimitiveTypeSpecifier::GetDouble())) {
				result_value = symbol_as_array->GetValue<const double*>(index,
						type_table);
			} else if (element_type_specifier->IsAssignableTo(
					PrimitiveTypeSpecifier::GetString())) {
				result_value = symbol_as_array->GetValue<const string*>(index,
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
					result_value = symbol_as_array->GetValue<
							const CompoundTypeInstance*>(index, type_table);
				} else {
					//we should never get here
					assert(false);
				}
			}
		} else {
			const YYLTYPE index_location = m_index_expression->GetPosition();
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

const LinkedList<const Error*>* ArrayVariable::SetSymbolCore(
		const ExecutionContext* context, const void* value) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	const ValidationResult* validation_result = ValidateOperation(context);
	errors = validation_result->GetErrors();
	if (errors == LinkedList<const Error*>::Terminator) {
		const ArraySymbol* array_symbol = validation_result->GetSymbol();
		const int index = validation_result->GetIndex();

		const TypeTable* type_table = context->GetTypeTable();
		const TypeSpecifier* element_type_specifier =
				array_symbol->GetElementType();
		const ArraySymbol* new_symbol = nullptr;

		if (element_type_specifier->IsAssignableTo(
				PrimitiveTypeSpecifier::GetBoolean())) {
			new_symbol = array_symbol->WithValue<const bool*>(index,
					(const bool*) value, type_table);
		} else if (element_type_specifier->IsAssignableTo(
				PrimitiveTypeSpecifier::GetInt())) {
			new_symbol = array_symbol->WithValue<const int*>(index,
					(const int*) value, type_table);
		} else if (element_type_specifier->IsAssignableTo(
				PrimitiveTypeSpecifier::GetDouble())) {
			new_symbol = array_symbol->WithValue<const double*>(index,
					(const double*) value, type_table);
		} else if (element_type_specifier->IsAssignableTo(
				PrimitiveTypeSpecifier::GetString())) {
			new_symbol = array_symbol->WithValue<const string*>(index,
					(const string*) value, type_table);
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
				new_symbol =
						array_symbol->WithValue<const CompoundTypeInstance*>(
								index, (const CompoundTypeInstance*) value,
								type_table);
			} else {
				//we should never get here
				assert(false);
			}
		}

		const SetResult set_result =
				context->GetSymbolContext()->SetArraySymbol(
						array_symbol->GetName(), new_symbol);

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
		const ArraySymbol* array_symbol =
				dynamic_cast<const ArraySymbol*>(symbol);
		if (array_symbol != nullptr) {
			const Expression* array_index_expression = GetIndexExpression();
			const YYLTYPE array_index_expression_position =
					array_index_expression->GetPosition();

			const TypeSpecifier* index_expression_type =
					m_index_expression->GetType(context);
			if (m_index_expression->GetType(context)->IsAssignableTo(
					PrimitiveTypeSpecifier::GetInt())) {
				if (array_symbol->IsFixedSize() && array_symbol->IsInitialized()
						&& array_index_expression->IsConstant()) {
					//index expression is a constant; validate it as appropriate
					const Result* evaluation = array_index_expression->Evaluate(
							context);

					const LinkedList<const Error*>* evaluation_errors =
							evaluation->GetErrors();

					if (evaluation_errors
							!= LinkedList<const Error*>::Terminator) {
						errors = errors->Concatenate(evaluation_errors, true);
					} else {
						int index = *((int *) evaluation->GetData());

						if (index > array_symbol->GetSize()) {
							ostringstream buffer;
							buffer << index;
							errors =
									errors->With(
											new Error(Error::SEMANTIC,
													Error::ARRAY_INDEX_OUT_OF_BOUNDS,
													array_index_expression_position.first_line,
													array_index_expression_position.first_column,
													*GetName(), buffer.str()));
						}
					}

					delete (evaluation);
				}
			} else {
				ostringstream buffer;
				buffer << "A " << index_expression_type->ToString()
						<< " expression";
				errors = errors->With(
						new Error(Error::SEMANTIC,
								Error::ARRAY_INDEX_MUST_BE_AN_INTEGER,
								m_index_expression->GetPosition().first_line,
								m_index_expression->GetPosition().first_column,
								*(GetName()), buffer.str()));
			}
		} else {
			errors = errors->With(
					new Error(Error::SEMANTIC, Error::VARIABLE_NOT_AN_ARRAY,
							GetLocation().first_line,
							GetLocation().first_column, *(GetName())));
		}
	} else {
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						GetLocation().first_line, GetLocation().first_column,
						*(GetName())));
	}

	return errors;
}

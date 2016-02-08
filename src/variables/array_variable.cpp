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

#include "assert.h"
#include "execution_context.h"
#include <assignment_statement.h>

ArrayVariable::ArrayVariable(const_shared_ptr<Variable> base_variable,
		const_shared_ptr<Expression> expression) :
		Variable(base_variable->GetName(), base_variable->GetLocation()), m_base_variable(
				base_variable), m_expression(expression) {
}

const_shared_ptr<TypeSpecifier> ArrayVariable::GetType(
		const shared_ptr<ExecutionContext> context) const {
	auto base_type_as_array = dynamic_pointer_cast<const ArrayTypeSpecifier>(
			m_base_variable->GetType(context));

	if (base_type_as_array) {
		auto base_evaluation = m_base_variable->Evaluate(context);

		if (ErrorList::IsTerminator(base_evaluation->GetErrors())) {
			auto array = std::static_pointer_cast<const Array>(
					base_evaluation->GetData());
			return array->GetElementType();
		} else {
			return PrimitiveTypeSpecifier::GetNone();
		}
	} else {
		return PrimitiveTypeSpecifier::GetNone();
	}
}

const_shared_ptr<string> ArrayVariable::ToString(
		const shared_ptr<ExecutionContext> context) const {
	ostringstream buffer;
	buffer << m_base_variable->ToString(context);
	buffer << "[";

	const_shared_ptr<Result> evaluation = m_expression->Evaluate(context);

	auto errors = evaluation->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		buffer << *(static_pointer_cast<const int>(evaluation->GetData()));
	} else {
		buffer << "EVALUATION ERROR";
	}
	buffer << "]";
	return make_shared<string>(buffer.str());
}

ArrayVariable::~ArrayVariable() {
}

const_shared_ptr<ArrayVariable::ValidationResult> ArrayVariable::ValidateOperation(
		const shared_ptr<ExecutionContext> context) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	int array_index = -1;
	yy::location index_location = GetDefaultLocation();
	plain_shared_ptr<Array> array;

	auto base_evaluation = m_base_variable->Evaluate(context);
	errors = base_evaluation->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto base_type_as_array =
				dynamic_pointer_cast<const ArrayTypeSpecifier>(
						m_base_variable->GetType(context));

		if (base_type_as_array) {
			array = static_pointer_cast<const Array>(
					base_evaluation->GetData());
			const_shared_ptr<TypeSpecifier> index_expression_type =
					m_expression->GetType(context);
			if (index_expression_type->IsAssignableTo(
					PrimitiveTypeSpecifier::GetInt())) {
				const_shared_ptr<Result> index_expression_evaluation =
						m_expression->Evaluate(context);
				errors = index_expression_evaluation->GetErrors();
				if (ErrorList::IsTerminator(errors)) {
					const int i = *(static_pointer_cast<const int>(
							index_expression_evaluation->GetData()));

					if (i >= 0) {
						array_index = i;
						index_location = m_expression->GetPosition();
					} else {
						errors =
								ErrorList::From(
										make_shared<Error>(Error::SEMANTIC,
												Error::ARRAY_INDEX_OUT_OF_BOUNDS,
												m_expression->GetPosition().begin.line,
												m_expression->GetPosition().begin.column,
												*(m_base_variable->ToString(
														context)),
												*AsString(i)), errors);
					}
				}
			} else {
				ostringstream buffer;
				buffer << "A " << index_expression_type->ToString()
						<< " expression";
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::ARRAY_INDEX_MUST_BE_AN_INTEGER,
								index_location.begin.line,
								index_location.begin.column,
								*(m_base_variable->ToString(context)),
								buffer.str()), errors);
			}
		} else {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::VARIABLE_NOT_AN_ARRAY,
							GetLocation().begin.line,
							GetLocation().begin.column,
							*(m_base_variable->ToString(context))), errors);
		}
	}

	return make_shared<ArrayVariable::ValidationResult>(array, array_index,
			index_location, errors);
}

const_shared_ptr<Result> ArrayVariable::Evaluate(
		const shared_ptr<ExecutionContext> context) const {
	ErrorListRef errors(ErrorList::GetTerminator());

	const_shared_ptr<ValidationResult> validation_result = ValidateOperation(
			context);
	plain_shared_ptr<void> result_value;
	errors = validation_result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto array = validation_result->GetArray();
		const int index = validation_result->GetIndex();
		const int size = array->GetSize();
		if (index < size && index >= 0) {
			auto type_table = context->GetTypeTable();
			const_shared_ptr<TypeSpecifier> element_type_specifier =
					array->GetElementType();
			if (element_type_specifier->IsAssignableTo(
					PrimitiveTypeSpecifier::GetBoolean())) {
				result_value = array->GetValue<bool>(index, *type_table);
			} else if (element_type_specifier->IsAssignableTo(
					PrimitiveTypeSpecifier::GetInt())) {
				result_value = array->GetValue<int>(index, *type_table);
			} else if (element_type_specifier->IsAssignableTo(
					PrimitiveTypeSpecifier::GetDouble())) {
				result_value = array->GetValue<double>(index, *type_table);
			} else if (element_type_specifier->IsAssignableTo(
					PrimitiveTypeSpecifier::GetString())) {
				result_value = array->GetValue<string>(index, *type_table);
			} else {
				const_shared_ptr<ArrayTypeSpecifier> as_array =
						std::dynamic_pointer_cast<const ArrayTypeSpecifier>(
								element_type_specifier);
				if (as_array) {
					//TODO
					assert(false);
				}

				const_shared_ptr<RecordTypeSpecifier> as_record =
						std::dynamic_pointer_cast<const RecordTypeSpecifier>(
								element_type_specifier);
				if (as_record) {
					result_value = array->GetValue<Record>(index,
							*type_table);
				} else {
					//we should never get here
					assert(false);
				}
			}
		} else {
			const yy::location index_location =
					validation_result->GetIndexLocation();
			ostringstream buffer;
			buffer << index;
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::ARRAY_INDEX_OUT_OF_BOUNDS,
							index_location.begin.line,
							index_location.begin.column,
							*(m_base_variable->ToString(context)),
							buffer.str()), errors);
		}
	}

	const_shared_ptr<Result> result = make_shared<Result>(result_value, errors);
	return result;
}

const ErrorListRef ArrayVariable::AssignValue(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<Expression> expression,
		const AssignmentType op) const {
	ErrorListRef errors(ErrorList::GetTerminator());

	auto variable_name = GetName();
	const int variable_line = GetLocation().begin.line;
	const int variable_column = GetLocation().begin.column;

	const_shared_ptr<ValidationResult> validation_result = ValidateOperation(
			context);
	errors = validation_result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto array = validation_result->GetArray();
		const int index = validation_result->GetIndex();

		if (index >= 0) {
			const_shared_ptr<TypeSpecifier> element_type_specifier =
					array->GetElementType();
			const_shared_ptr<PrimitiveTypeSpecifier> element_as_primitive =
					std::dynamic_pointer_cast<const PrimitiveTypeSpecifier>(
							element_type_specifier);

			auto type_table = context->GetTypeTable();
			if (element_as_primitive) {
				const BasicType element_type =
						element_as_primitive->GetBasicType();

				switch (element_type) {
				case BOOLEAN: {
					const_shared_ptr<bool> value = array->GetValue<bool>(index,
							*type_table);
					const_shared_ptr<Result> result =
							AssignmentStatement::do_op(variable_name,
									element_type, variable_line,
									variable_column, *value, expression, op,
									context);

					errors = result->GetErrors();
					if (ErrorList::IsTerminator(errors)) {
						errors = SetSymbolCore(context, result->GetData());
						break;
					}
					break;
				}
				case INT: {
					const_shared_ptr<int> value = array->GetValue<int>(index,
							*type_table);
					const_shared_ptr<Result> result =
							AssignmentStatement::do_op(variable_name,
									element_type, variable_line,
									variable_column, *value, expression, op,
									context);

					errors = result->GetErrors();
					if (ErrorList::IsTerminator(errors)) {
						errors = SetSymbolCore(context, result->GetData());
						break;
					}
					break;
				}
				case DOUBLE: {
					const_shared_ptr<double> value = array->GetValue<double>(
							index, *type_table);
					const_shared_ptr<Result> result =
							AssignmentStatement::do_op(variable_name,
									element_type, variable_line,
									variable_column, *value, expression, op,
									context);

					errors = result->GetErrors();
					if (ErrorList::IsTerminator(errors)) {
						errors = SetSymbolCore(context, result->GetData());
					}
					break;
				}
				case STRING: {
					const_shared_ptr<string> value = array->GetValue<string>(
							index, *type_table);
					const_shared_ptr<Result> result =
							AssignmentStatement::do_op(variable_name,
									element_type, variable_line,
									variable_column, value, expression, op,
									context);

					errors = result->GetErrors();
					if (ErrorList::IsTerminator(errors)) {
						errors = SetSymbolCore(context, result->GetData());
					}
					break;
				}
				default:
					assert(false);
				}
			} else {
				if (op == AssignmentType::ASSIGN) {
					const_shared_ptr<Result> result = expression->Evaluate(
							context);
					errors = result->GetErrors();
					if (ErrorList::IsTerminator(errors)) {
						const_shared_ptr<ArrayTypeSpecifier> element_as_array =
								std::dynamic_pointer_cast<
										const ArrayTypeSpecifier>(
										element_type_specifier);
						if (element_as_array) {
							errors = SetSymbolCore(context, result->GetData());
						}

						const_shared_ptr<RecordTypeSpecifier> element_as_record =
								std::dynamic_pointer_cast<
										const RecordTypeSpecifier>(
										element_type_specifier);
						if (element_as_record) {
							errors = SetSymbolCore(context, result->GetData());
						}
					}
				} else {
					errors = errors->From(
							make_shared<Error>(Error::SEMANTIC,
									Error::ASSIGNMENT_TYPE_ERROR, variable_line,
									variable_column,
									expression->GetType(context)->ToString(),
									element_type_specifier->ToString()),
							errors);
				}
			}
		} else {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::ARRAY_INDEX_OUT_OF_BOUNDS, variable_line,
							variable_column,
							*(m_base_variable->ToString(context)),
							*AsString(index)), errors);
		}
	}

	return errors;
}

const ErrorListRef ArrayVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<bool> value) const {
	return SetSymbolCore(context, static_pointer_cast<const void>(value));
}

const ErrorListRef ArrayVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<int> value) const {
	return SetSymbolCore(context, static_pointer_cast<const void>(value));
}

const ErrorListRef ArrayVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<double> value) const {
	return SetSymbolCore(context, static_pointer_cast<const void>(value));
}

const ErrorListRef ArrayVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<string> value) const {
	return SetSymbolCore(context, static_pointer_cast<const void>(value));
}

const ErrorListRef ArrayVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<Array> value) const {
	return SetSymbolCore(context, static_pointer_cast<const void>(value));
}

const ErrorListRef ArrayVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<Record> value) const {
	return SetSymbolCore(context, static_pointer_cast<const void>(value));
}

const_shared_ptr<TypeSpecifier> ArrayVariable::GetElementType(
		const shared_ptr<ExecutionContext> context) const {
	auto base_type_as_array = dynamic_pointer_cast<const ArrayTypeSpecifier>(
			m_base_variable->GetType(context));

	if (base_type_as_array) {
		return base_type_as_array->GetElementTypeSpecifier();
	} else {
		return PrimitiveTypeSpecifier::GetNone();
	}
}

const ErrorListRef ArrayVariable::SetSymbolCore(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<void> value) const {
	ErrorListRef errors(ErrorList::GetTerminator());

	const_shared_ptr<ValidationResult> validation_result = ValidateOperation(
			context);
	errors = validation_result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto array = validation_result->GetArray();
		const int index = validation_result->GetIndex();

		auto type_table = context->GetTypeTable();
		const_shared_ptr<TypeSpecifier> element_type_specifier =
				array->GetElementType();
		shared_ptr<const Array> new_array = nullptr;

		if (element_type_specifier->IsAssignableTo(
				PrimitiveTypeSpecifier::GetBoolean())) {
			new_array = array->WithValue<bool>(index,
					static_pointer_cast<const bool>(value), *type_table);
		} else if (element_type_specifier->IsAssignableTo(
				PrimitiveTypeSpecifier::GetInt())) {
			new_array = array->WithValue<int>(index,
					static_pointer_cast<const int>(value), *type_table);
		} else if (element_type_specifier->IsAssignableTo(
				PrimitiveTypeSpecifier::GetDouble())) {
			new_array = array->WithValue<double>(index,
					static_pointer_cast<const double>(value), *type_table);
		} else if (element_type_specifier->IsAssignableTo(
				PrimitiveTypeSpecifier::GetString())) {
			new_array = array->WithValue<string>(index,
					static_pointer_cast<const string>(value), *type_table);
		} else {
			const_shared_ptr<ArrayTypeSpecifier> as_array =
					std::dynamic_pointer_cast<const ArrayTypeSpecifier>(
							element_type_specifier);
			if (as_array) {
				new_array = array->WithValue<Array>(index,
						static_pointer_cast<const Array>(value), *type_table);
			}

			const_shared_ptr<RecordTypeSpecifier> as_record =
					std::dynamic_pointer_cast<const RecordTypeSpecifier>(
							element_type_specifier);
			if (as_record) {
				new_array = array->WithValue<Record>(index,
						static_pointer_cast<const Record>(value),
						*type_table);
			}
		}

		const SetResult set_result = context->SetSymbol(*GetName(), new_array);

		errors = ToErrorListRef(set_result,
				context->GetSymbol(*GetName(), DEEP)->GetType(),
				array->GetTypeSpecifier());
	}

	return errors;
}

const ErrorListRef ArrayVariable::Validate(
		const shared_ptr<ExecutionContext> context) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	auto base_evaluation = m_base_variable->Evaluate(context);
	errors = base_evaluation->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		const_shared_ptr<TypeSpecifier> index_expression_type =
				m_expression->GetType(context);
		if (index_expression_type->IsAssignableTo(
				PrimitiveTypeSpecifier::GetInt())) {
			auto base_type_as_array = dynamic_pointer_cast<
					const ArrayTypeSpecifier>(
					m_base_variable->GetType(context));

			if (!base_type_as_array) {
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::VARIABLE_NOT_AN_ARRAY,
								m_expression->GetPosition().begin.line,
								m_expression->GetPosition().begin.column,
								*(m_base_variable->ToString(context))), errors);
			}
		} else {
			ostringstream buffer;
			buffer << "A " << index_expression_type->ToString()
					<< " expression";
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::ARRAY_INDEX_MUST_BE_AN_INTEGER,
							m_expression->GetPosition().begin.line,
							m_expression->GetPosition().begin.column,
							*(m_base_variable->ToString(context)),
							buffer.str()), errors);
		}
	}

	return errors;
}

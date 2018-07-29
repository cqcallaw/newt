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

#include <array_type_specifier.h>
#include <array_variable.h>
#include <expression.h>
#include <sstream>

#include "assert.h"
#include "execution_context.h"
#include <assignment_statement.h>
#include <constant_expression.h>
#include <array_type.h>
#include <record_type.h>

ArrayVariable::ArrayVariable(const_shared_ptr<Variable> base_variable,
		const_shared_ptr<Expression> expression) :
		Variable(base_variable->GetName(), base_variable->GetLocation()), m_base_variable(
				base_variable), m_expression(expression) {
}

TypedResult<TypeSpecifier> ArrayVariable::GetTypeSpecifier(
		const shared_ptr<ExecutionContext> context,
		AliasResolution resolution) const {
	auto base_type_specifier_result = m_base_variable->GetTypeSpecifier(
			context);

	auto errors = base_type_specifier_result.GetErrors();

	if (ErrorList::IsTerminator(errors)) {
		auto base_type_as_array =
				dynamic_pointer_cast<const ArrayTypeSpecifier>(
						base_type_specifier_result.GetData());

		if (base_type_as_array) {
			auto base_evaluation = m_base_variable->Evaluate(context);

			if (ErrorList::IsTerminator(base_evaluation->GetErrors())) {
				auto array = base_evaluation->GetData<Array>();
				if (array) {
					return array->GetElementTypeSpecifier();
				} else {
					assert(false);
				}
			}
		}
	}

	return TypedResult<TypeSpecifier>(nullptr, errors);
}

const_shared_ptr<string> ArrayVariable::ToString(
		const shared_ptr<ExecutionContext> context) const {
	ostringstream buffer;
	buffer << *m_base_variable->ToString(context);
	buffer << "[";

	const_shared_ptr<Result> evaluation = m_expression->Evaluate(context,
			context);

	auto errors = evaluation->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		buffer << *(evaluation->GetData<int>());
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
	int array_index = -1;
	yy::location index_location = GetDefaultLocation();
	plain_shared_ptr<Array> array;

	auto base_evaluation = m_base_variable->Evaluate(context);
	ErrorListRef errors = base_evaluation->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto base_type_specifier_result = m_base_variable->GetTypeSpecifier(
				context);

		errors = base_type_specifier_result.GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto base_type_specifier = base_type_specifier_result.GetData();
			auto base_type_result = base_type_specifier->GetType(
					context->GetTypeTable());

			errors = base_type_result->GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				auto base_type = base_type_result->GetData<TypeDefinition>();
				auto base_type_as_array = dynamic_pointer_cast<const ArrayType>(
						base_type);
				if (base_type_as_array) {
					array = base_evaluation->GetData<Array>();
					auto index_expression_type_result =
							m_expression->GetTypeSpecifier(context);

					errors = index_expression_type_result.GetErrors();

					if (ErrorList::IsTerminator(errors)) {
						auto index_expression_type =
								index_expression_type_result.GetData();
						auto index_analysis =
								index_expression_type->AnalyzeAssignmentTo(
										PrimitiveTypeSpecifier::GetInt(),
										context->GetTypeTable(),
										TypeSpecifier::DefaultTypeSpecifierMap);
						auto expression_location = m_expression->GetLocation();
						if (index_analysis == EQUIVALENT
								|| index_analysis == UNAMBIGUOUS) {
							const_shared_ptr<Result> index_expression_evaluation =
									m_expression->Evaluate(context, context);
							errors = index_expression_evaluation->GetErrors();
							if (ErrorList::IsTerminator(errors)) {
								const int i =
										*(index_expression_evaluation->GetData<
												int>());

								if (i >= 0) {
									array_index = i;
									index_location = expression_location;
								} else {
									errors =
											ErrorList::From(
													make_shared<Error>(
															Error::SEMANTIC,
															Error::ARRAY_INDEX_OUT_OF_BOUNDS,
															expression_location.begin,
															*(m_base_variable->ToString(
																	context)),
															*AsString(i)),
													errors);
								}
							}
						} else {
							ostringstream buffer;
							buffer << "A " << index_expression_type->ToString()
									<< " expression";
							errors =
									ErrorList::From(
											make_shared<Error>(Error::SEMANTIC,
													Error::ARRAY_INDEX_MUST_BE_AN_INTEGER,
													expression_location.begin,
													*(m_base_variable->ToString(
															context)),
													buffer.str()), errors);
						}
					}
				} else {
					errors = ErrorList::From(
							make_shared<Error>(Error::SEMANTIC,
									Error::VARIABLE_NOT_AN_ARRAY,
									GetLocation().begin,
									*(m_base_variable->ToString(context))),
							errors);
				}
			}
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
					array->GetElementTypeSpecifier();
			if (element_type_specifier->AnalyzeAssignmentTo(
					PrimitiveTypeSpecifier::GetBoolean(),
					context->GetTypeTable(),
					TypeSpecifier::DefaultTypeSpecifierMap) == EQUIVALENT) {
				result_value = array->GetValue<bool>(index, *type_table);
			} else if (element_type_specifier->AnalyzeAssignmentTo(
					PrimitiveTypeSpecifier::GetInt(), context->GetTypeTable(),
					TypeSpecifier::DefaultTypeSpecifierMap) == EQUIVALENT) {
				result_value = array->GetValue<int>(index, *type_table);
			} else if (element_type_specifier->AnalyzeAssignmentTo(
					PrimitiveTypeSpecifier::GetDouble(),
					context->GetTypeTable(),
					TypeSpecifier::DefaultTypeSpecifierMap) == EQUIVALENT) {
				result_value = array->GetValue<double>(index, *type_table);
			} else if (element_type_specifier->AnalyzeAssignmentTo(
					PrimitiveTypeSpecifier::GetString(),
					context->GetTypeTable(),
					TypeSpecifier::DefaultTypeSpecifierMap) == EQUIVALENT) {
				result_value = array->GetValue<string>(index, *type_table);
			} else {
				auto element_type_result = element_type_specifier->GetType(
						context->GetTypeTable(), RESOLVE);

				errors = element_type_result->GetErrors();
				if (ErrorList::IsTerminator(errors)) {
					auto element_type = element_type_result->GetData<
							TypeDefinition>();
					auto as_array = std::dynamic_pointer_cast<const ArrayType>(
							element_type);
					if (as_array) {
						//TODO
						assert(false);
					}

					auto as_record =
							std::dynamic_pointer_cast<const RecordType>(
									element_type);
					if (as_record) {
						result_value = array->GetValue<Record>(index,
								*type_table);
					} else {
						//we should never get here
						assert(false);
					}
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
							index_location.begin,
							*(m_base_variable->ToString(context)),
							buffer.str()), errors);
		}
	}

	const_shared_ptr<Result> result = make_shared<Result>(result_value, errors);
	return result;
}

const ErrorListRef ArrayVariable::AssignValue(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure,
		const_shared_ptr<Expression> expression,
		const AssignmentType op) const {
	ErrorListRef errors(ErrorList::GetTerminator());

	auto variable_name = GetName();

	const_shared_ptr<ValidationResult> validation_result = ValidateOperation(
			context);
	errors = validation_result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto array = validation_result->GetArray();
		const int index = validation_result->GetIndex();

		if (index >= 0) {
			const_shared_ptr<TypeSpecifier> element_type_specifier =
					array->GetElementTypeSpecifier();
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
									element_type, GetLocation().begin, *value,
									expression, op, context);

					errors = result->GetErrors();
					if (ErrorList::IsTerminator(errors)) {
						errors = SetSymbolCore(context,
								result->GetData<bool>());
						break;
					}
					break;
				}
				case INT: {
					const_shared_ptr<int> value = array->GetValue<int>(index,
							*type_table);
					const_shared_ptr<Result> result =
							AssignmentStatement::do_op(variable_name,
									element_type, GetLocation().begin, *value,
									expression, op, context);

					errors = result->GetErrors();
					if (ErrorList::IsTerminator(errors)) {
						errors = SetSymbolCore(context, result->GetData<int>());
						break;
					}
					break;
				}
				case DOUBLE: {
					const_shared_ptr<double> value = array->GetValue<double>(
							index, *type_table);
					const_shared_ptr<Result> result =
							AssignmentStatement::do_op(variable_name,
									element_type, GetLocation().begin, *value,
									expression, op, context);

					errors = result->GetErrors();
					if (ErrorList::IsTerminator(errors)) {
						errors = SetSymbolCore(context,
								result->GetData<double>());
					}
					break;
				}
				case STRING: {
					const_shared_ptr<string> value = array->GetValue<string>(
							index, *type_table);
					const_shared_ptr<Result> result =
							AssignmentStatement::do_op(variable_name,
									element_type, GetLocation().begin, value,
									expression, op, context);

					errors = result->GetErrors();
					if (ErrorList::IsTerminator(errors)) {
						errors = SetSymbolCore(context,
								result->GetData<string>());
					}
					break;
				}
				default:
					assert(false);
				}
			} else {
				if (op == AssignmentType::ASSIGN) {
					const_shared_ptr<Result> result = expression->Evaluate(
							context, context);
					errors = result->GetErrors();
					if (ErrorList::IsTerminator(errors)) {
						auto element_type_result =
								element_type_specifier->GetType(
										context->GetTypeTable(), RESOLVE);

						errors = element_type_result->GetErrors();
						if (ErrorList::IsTerminator(errors)) {

							auto element_type = element_type_result->GetData<
									TypeDefinition>();
							auto element_as_array = std::dynamic_pointer_cast<
									const ArrayType>(element_type);
							if (element_as_array) {
								errors = SetSymbolCore(context,
										result->GetData<Array>());
							}

							auto element_as_record = std::dynamic_pointer_cast<
									const RecordType>(element_type);
							if (element_as_record) {
								errors = SetSymbolCore(context,
										result->GetData<Record>());
							}
						}
					}
				} else {
					shared_ptr<const TypeSpecifier> expression_type_specifier =
							PrimitiveTypeSpecifier::GetNone();
					auto expression_type_specifier_result =
							expression->GetTypeSpecifier(context);

					auto expression_type_specifier_errors =
							expression_type_specifier_result.GetErrors();
					if (ErrorList::IsTerminator(
							expression_type_specifier_errors)) {
						expression_type_specifier =
								expression_type_specifier_result.GetData();
					}

					errors = errors->From(
							make_shared<Error>(Error::SEMANTIC,
									Error::ASSIGNMENT_TYPE_ERROR,
									GetLocation().begin,
									expression_type_specifier->ToString(),
									element_type_specifier->ToString()),
							errors);
				}
			}
		} else {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::ARRAY_INDEX_OUT_OF_BOUNDS,
							GetLocation().begin,
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
		const_shared_ptr<ComplexTypeSpecifier> type,
		const_shared_ptr<Record> value) const {
	return SetSymbolCore(context, static_pointer_cast<const void>(value));
}

const_shared_ptr<TypeSpecifier> ArrayVariable::GetElementType(
		const shared_ptr<ExecutionContext> context) const {
	//TODO: return Result<TypeSpecifier>
	auto base_type_specifier_result = m_base_variable->GetTypeSpecifier(
			context);
	auto errors = base_type_specifier_result.GetErrors();

	if (ErrorList::IsTerminator(errors)) {
		auto base_type_specifier = base_type_specifier_result.GetData();
		auto base_type_result = base_type_specifier->GetType(
				context->GetTypeTable());

		auto errors = base_type_result->GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto base_type = base_type_result->GetData<TypeDefinition>();
			auto base_type_as_array = dynamic_pointer_cast<const ArrayType>(
					base_type);
			if (base_type_as_array) {
				return base_type_as_array->GetMemberTypeSpecifier();
			}
		}
	}

	return PrimitiveTypeSpecifier::GetNone();
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
				array->GetElementTypeSpecifier();
		shared_ptr<const Array> new_array = nullptr;

		if (element_type_specifier->AnalyzeAssignmentTo(
				PrimitiveTypeSpecifier::GetBoolean(), context->GetTypeTable(),
				TypeSpecifier::DefaultTypeSpecifierMap)) {
			new_array = array->WithValue<bool>(index,
					static_pointer_cast<const bool>(value), *type_table);
		} else if (element_type_specifier->AnalyzeAssignmentTo(
				PrimitiveTypeSpecifier::GetInt(), context->GetTypeTable(),
				TypeSpecifier::DefaultTypeSpecifierMap)) {
			new_array = array->WithValue<int>(index,
					static_pointer_cast<const int>(value), *type_table);
		} else if (element_type_specifier->AnalyzeAssignmentTo(
				PrimitiveTypeSpecifier::GetDouble(), context->GetTypeTable(),
				TypeSpecifier::DefaultTypeSpecifierMap)) {
			new_array = array->WithValue<double>(index,
					static_pointer_cast<const double>(value), *type_table);
		} else if (element_type_specifier->AnalyzeAssignmentTo(
				PrimitiveTypeSpecifier::GetString(), context->GetTypeTable(),
				TypeSpecifier::DefaultTypeSpecifierMap)) {
			new_array = array->WithValue<string>(index,
					static_pointer_cast<const string>(value), *type_table);
		} else {
			auto element_type_result = element_type_specifier->GetType(
					context->GetTypeTable(), RESOLVE);

			errors = element_type_result->GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				auto element_type =
						element_type_result->GetData<TypeDefinition>();

				auto as_array = std::dynamic_pointer_cast<const ArrayType>(
						element_type);
				if (as_array) {
					new_array = array->WithValue<Array>(index,
							static_pointer_cast<const Array>(value),
							*type_table);
				}

				auto as_record = std::dynamic_pointer_cast<const RecordType>(
						element_type);
				if (as_record) {
					new_array = array->WithValue<Record>(index,
							static_pointer_cast<const Record>(value),
							*type_table);
				}
			}
		}

		//wrap result in constant expression and assign it to the base variable
		auto as_const_expression = make_shared<ConstantExpression>(
				m_expression->GetLocation(), new_array);
		errors = m_base_variable->AssignValue(context, context,
				as_const_expression, AssignmentType::ASSIGN);
	}

	return errors;
}

const ErrorListRef ArrayVariable::Validate(
		const shared_ptr<ExecutionContext> context) const {
	auto base_evaluation = m_base_variable->Validate(context);
	ErrorListRef errors = base_evaluation;
	if (ErrorList::IsTerminator(errors)) {
		auto index_expression_type_result = m_expression->GetTypeSpecifier(
				context);
		errors = index_expression_type_result.GetErrors();

		if (ErrorList::IsTerminator(errors)) {
			auto index_expression_type = index_expression_type_result.GetData();
			auto index_analysis = index_expression_type->AnalyzeAssignmentTo(
					PrimitiveTypeSpecifier::GetInt(), context->GetTypeTable(),
					TypeSpecifier::DefaultTypeSpecifierMap);
			if (index_analysis == EQUIVALENT || index_analysis == UNAMBIGUOUS) {
				auto base_type_specifier_result =
						m_base_variable->GetTypeSpecifier(context);
				errors = base_type_specifier_result.GetErrors();

				if (ErrorList::IsTerminator(errors)) {
					auto base_type_specifier =
							base_type_specifier_result.GetData();
					auto base_type_result = base_type_specifier->GetType(
							context->GetTypeTable());

					errors = base_type_result->GetErrors();
					if (ErrorList::IsTerminator(errors)) {
						auto base_type = base_type_result->GetData<
								TypeDefinition>();
						auto base_type_as_array = dynamic_pointer_cast<
								const ArrayType>(base_type);
						if (!base_type_as_array) {
							errors =
									ErrorList::From(
											make_shared<Error>(Error::SEMANTIC,
													Error::VARIABLE_NOT_AN_ARRAY,
													m_expression->GetLocation().begin,
													*(m_base_variable->ToString(
															context))), errors);
						}
					}
				}
			} else {
				ostringstream buffer;
				buffer << "A " << index_expression_type->ToString()
						<< " expression";
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::ARRAY_INDEX_MUST_BE_AN_INTEGER,
								m_expression->GetLocation().begin,
								*(m_base_variable->ToString(context)),
								buffer.str()), errors);
			}
		}
	}

	return errors;
}

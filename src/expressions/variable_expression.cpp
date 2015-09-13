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

#include <variable_expression.h>
#include <variable.h>
#include <array_variable.h>
#include <compound_type_instance.h>
#include <member_variable.h>
#include <error.h>
#include <sstream>
#include <execution_context.h>
#include <type.h>

VariableExpression::VariableExpression(const YYLTYPE position,
		const Variable* variable) :
		Expression(position), m_variable(variable) {
}

const TypeSpecifier* VariableExpression::GetType(
		const ExecutionContext* execution_context) const {
	return m_variable->GetType(execution_context);
}

const Result* VariableExpression::Evaluate(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;
	const void* result = nullptr;

	const string* variable_name = m_variable->GetName();
	const Symbol* symbol = execution_context->GetSymbolContext()->GetSymbol(
			variable_name);

	if (symbol == nullptr || symbol == Symbol::DefaultSymbol) {
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						m_variable->GetLocation().first_line,
						m_variable->GetLocation().first_column,
						*(m_variable->GetName())));
	} else {
		if (m_variable->IsBasicReference()) {
			result = symbol->GetValue();
		} else {
			const TypeSpecifier* symbol_type = symbol->GetType();
			const ArrayVariable* as_array_variable =
					dynamic_cast<const ArrayVariable*>(m_variable);
			if (as_array_variable != nullptr) {
				const ArraySymbol* as_array_symbol =
						dynamic_cast<const ArraySymbol*>(symbol);

				if (as_array_symbol != nullptr) {
					const Expression* array_index_expression =
							as_array_variable->GetIndexExpression();
					const TypeSpecifier* array_index_expression_type =
							array_index_expression->GetType(execution_context);

					if (array_index_expression_type->IsAssignableTo(
							PrimitiveTypeSpecifier::GetInt())) {
						const Result* evaluation =
								array_index_expression->Evaluate(
										execution_context);
						const LinkedList<const Error*>* evaluation_errors =
								evaluation->GetErrors();

						if (evaluation_errors
								!= LinkedList<const Error*>::Terminator) {
							errors = evaluation_errors;
						} else {
							int index = *((int *) evaluation->GetData());

							if (index > as_array_symbol->GetSize()) {
								ostringstream buffer;
								buffer << index;
								execution_context->GetSymbolContext()->print(
										cout, execution_context->GetTypeTable(),
										Indent(0));
								errors =
										errors->With(
												new Error(Error::SEMANTIC,
														Error::ARRAY_INDEX_OUT_OF_BOUNDS,
														m_variable->GetLocation().first_line,
														m_variable->GetLocation().first_column,
														*(m_variable->GetName()),
														buffer.str()));
							} else {
								const TypeSpecifier* element_type_specifier =
										as_array_symbol->GetElementType();
								const TypeTable* type_table =
										execution_context->GetTypeTable();

								if (element_type_specifier
										== PrimitiveTypeSpecifier::GetInt()) {
									result = as_array_symbol->GetValue<
											const int*>(index, type_table);
								} else if (element_type_specifier
										== PrimitiveTypeSpecifier::GetDouble()) {
									result = as_array_symbol->GetValue<
											const double*>(index, type_table);
								} else if (element_type_specifier
										== PrimitiveTypeSpecifier::GetString()) {
									result = as_array_symbol->GetValue<
											const string*>(index, type_table);
								} else if (dynamic_cast<const CompoundTypeSpecifier*>(element_type_specifier)
										!= nullptr) {
									result = as_array_symbol->GetValue<
											const CompoundType*>(index,
											type_table);
								} else {
									assert(false);
								}
							}
						}

						delete (evaluation);
					} else {
						ostringstream buffer;
						buffer << "A "
								<< array_index_expression_type->ToString()
								<< " expression";

						errors =
								errors->With(
										new Error(Error::SEMANTIC,
												Error::ARRAY_INDEX_MUST_BE_AN_INTEGER,
												array_index_expression->GetPosition().first_line,
												array_index_expression->GetPosition().first_column,
												*variable_name, buffer.str()));
					}
				} else {
					errors = errors->With(
							new Error(Error::SEMANTIC,
									Error::VARIABLE_NOT_AN_ARRAY,
									m_variable->GetLocation().first_line,
									m_variable->GetLocation().first_column));
				}
			}

			const MemberVariable* as_member_variable =
					dynamic_cast<const MemberVariable*>(m_variable);
			if (as_member_variable != nullptr) {
				if (dynamic_cast<const CompoundTypeSpecifier*>(symbol_type)
						!= nullptr) {
					const CompoundTypeInstance* as_struct =
							(const CompoundTypeInstance*) symbol->GetValue();
					const MemberVariable* as_member_variable =
							(MemberVariable*) m_variable;
					const Variable* member =
							as_member_variable->GetMemberVariable();
					const VariableExpression* member_expression =
							new VariableExpression(member->GetLocation(),
									member);
					const ExecutionContext* tmp_context =
							execution_context->WithSymbolContext(
									as_struct->GetDefinition());
					const Result* eval = member_expression->Evaluate(
							tmp_context);
					delete tmp_context;
					return eval;
				} else {
					errors = errors->With(
							new Error(Error::SEMANTIC,
									Error::VARIABLE_NOT_A_COMPOUND_TYPE,
									m_variable->GetLocation().first_line,
									m_variable->GetLocation().first_column));
				}
			}
		}
	}

	return new Result(result, errors);
}

const LinkedList<const Error*>* VariableExpression::Validate(
		const ExecutionContext* execution_context) const {
	const string* variable_name = m_variable->GetName();
	const Symbol* symbol = execution_context->GetSymbolContext()->GetSymbol(
			*variable_name);

	const LinkedList<const Error*>* result =
			LinkedList<const Error*>::Terminator;

	if (symbol == NULL || symbol == Symbol::DefaultSymbol) {
		result = result->With(
				new Error(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						m_variable->GetLocation().first_line,
						m_variable->GetLocation().first_column,
						*(variable_name)));
		return result;
	}

	const ArrayVariable* as_array_variable =
			dynamic_cast<const ArrayVariable*>(m_variable);
	if (as_array_variable != nullptr) {
		const Expression* array_index_expression =
				as_array_variable->GetIndexExpression();
		const YYLTYPE array_index_expression_position =
				array_index_expression->GetPosition();

		const TypeSpecifier* index_type = array_index_expression->GetType(
				execution_context);
		if (!index_type->IsAssignableTo(PrimitiveTypeSpecifier::GetInt())) {
			ostringstream os;
			os << "A " << index_type->ToString() << " expression";

			result = result->With(
					new Error(Error::SEMANTIC,
							Error::ARRAY_INDEX_MUST_BE_AN_INTEGER,
							array_index_expression_position.first_line,
							array_index_expression_position.first_column,
							*variable_name, os.str()));
		} else if (array_index_expression->IsConstant()) {
			//index expression is a constant; validate it as appropriate
			const Symbol* symbol =
					execution_context->GetSymbolContext()->GetSymbol(
							variable_name);
			ArraySymbol* as_array_symbol = (ArraySymbol*) symbol;

			if (as_array_symbol->IsInitialized()) {
				const Result* evaluation = array_index_expression->Evaluate(
						execution_context);

				const LinkedList<const Error*>* evaluation_errors =
						evaluation->GetErrors();

				if (evaluation_errors != LinkedList<const Error*>::Terminator) {
					result = result->Concatenate(evaluation_errors, true);
				} else {
					int index = *((int *) evaluation->GetData());

					if (index > as_array_symbol->GetSize()) {
						ostringstream buffer;
						buffer << index;
						result = result->With(
								new Error(Error::SEMANTIC,
										Error::ARRAY_INDEX_OUT_OF_BOUNDS,
										m_variable->GetLocation().first_line,
										m_variable->GetLocation().first_column,
										*variable_name, buffer.str()));
					}
				}

				delete (evaluation);
			}
		}
	}

	return result;
}

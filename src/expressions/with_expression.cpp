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

#include <with_expression.h>
#include <execution_context.h>
#include <compound_type_specifier.h>
#include <compound_type.h>
#include <member_definition.h>
#include <member_instantiation.h>
#include <member_instantiation_list.h>

WithExpression::WithExpression(const YYLTYPE position,
		const Expression* source_expression,
		const MemberInstantiationList* member_instantiation_list,
		const YYLTYPE member_instantiation_list_position) :
		Expression(position), m_source_expression(source_expression), m_member_instantiation_list(
				member_instantiation_list), m_member_instantiation_list_position(
				member_instantiation_list_position) {
}

WithExpression::~WithExpression() {
}

const TypeSpecifier* WithExpression::GetType(
		const ExecutionContext* execution_context) const {
	return m_source_expression->GetType(execution_context);
}

const Result* WithExpression::Evaluate(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;
	const Result* source_result = m_source_expression->Evaluate(
			execution_context);

	errors = source_result->GetErrors();
	if (errors == LinkedList<const Error*>::Terminator) {
		const void* new_value = nullptr;
		const TypeSpecifier* type_specifier = m_source_expression->GetType(
				execution_context);

		const CompoundTypeSpecifier* as_compound =
				dynamic_cast<const CompoundTypeSpecifier*>(type_specifier);
		if (as_compound) {
			const string type_name = as_compound->GetTypeName();
			const CompoundType* type =
					execution_context->GetTypeTable()->GetType(type_name);

			if (type != CompoundType::GetDefaultCompoundType()) {
				const void* raw_result = source_result->GetData();
				const CompoundTypeInstance* as_compound =
						(const CompoundTypeInstance*) raw_result;
				const SymbolContext* existing_context =
						as_compound->GetDefinition();

				auto new_values = new map<const string, const Symbol*,
						comparator>(existing_context->GetTable()->begin(),
						existing_context->GetTable()->end());

				const LinkedList<const MemberInstantiation*>* subject =
						m_member_instantiation_list;
				while (!subject->IsTerminator()) {
					const MemberInstantiation* instantiation =
							subject->GetData();
					const string* member_name = instantiation->GetName();
					const MemberDefinition* member_definition = type->GetMember(
							*member_name);

					if (member_definition
							!= MemberDefinition::GetDefaultMemberDefinition()) {
						const TypeSpecifier* member_type =
								member_definition->GetType();
						const TypeSpecifier* expression_type =
								instantiation->GetExpression()->GetType(
										execution_context);
						if (expression_type->IsAssignableTo(member_type)) {
							const MemberInstantiation* memberInstantiation =
									subject->GetData();
							const Result* evaluation_result =
									memberInstantiation->GetExpression()->Evaluate(
											execution_context);

							if (evaluation_result->GetErrors()
									== LinkedList<const Error*>::Terminator) {
								const void* void_value =
										evaluation_result->GetData();

								/*SetResult result = NO_SET_RESULT;*/
								if (member_type->IsAssignableTo(
										PrimitiveTypeSpecifier::GetBoolean())) {
									new_values->at(*member_name) = new Symbol(
											*member_name, (bool*) void_value);
								} else if (member_type->IsAssignableTo(
										PrimitiveTypeSpecifier::GetInt())) {
									new_values->at(*member_name) = new Symbol(
											*member_name, (int*) void_value);
								} else if (member_type->IsAssignableTo(
										PrimitiveTypeSpecifier::GetDouble())) {
									new_values->at(*member_name) = new Symbol(
											*member_name, (double*) void_value);
								} else if (member_type->IsAssignableTo(
										PrimitiveTypeSpecifier::GetString())) {
									new_values->at(*member_name) = new Symbol(
											*member_name, (string*) void_value);
								} else if (dynamic_cast<const CompoundTypeSpecifier*>(member_type)
										!= nullptr) {
									new_values->at(*member_name) = new Symbol(
											*member_name,
											(CompoundTypeInstance*) void_value);
								} else {
									assert(false);
								}

								/*errors = ToErrorList(result,
								 memberInstantiation->GetNamePosition(),
								 *member_name);*/
							} else {
								errors = errors->Concatenate(
										evaluation_result->GetErrors(), true);
							}
						} else {
							//undefined member
							errors =
									errors->With(
											new Error(Error::SEMANTIC,
													Error::ASSIGNMENT_TYPE_ERROR,
													instantiation->GetExpressionPosition().first_line,
													instantiation->GetExpressionPosition().first_column,
													member_type->ToString(),
													expression_type->ToString()));
						}
					} else {
						//undefined member
						errors =
								errors->With(
										new Error(Error::SEMANTIC,
												Error::UNDECLARED_MEMBER,
												instantiation->GetNamePosition().first_line,
												instantiation->GetNamePosition().first_column,
												*member_name,
												member_definition->GetType()->ToString()));

					}

					subject = subject->GetNext();
				}

				SymbolContext* new_symbol_context = new SymbolContext(
						existing_context->GetModifiers(),
						execution_context->GetSymbolContext()->GetParent(),
						new_values);

				const CompoundTypeInstance* new_instance =
						new CompoundTypeInstance(
								as_compound->GetTypeSpecifier(),
								new_symbol_context);

				new_value = (void*) new_instance;
			} else {
				errors = errors->With(
						new Error(Error::SEMANTIC, Error::UNDECLARED_TYPE,
								m_source_expression->GetPosition().first_line,
								m_source_expression->GetPosition().first_column,
								type_name));
			}
		} else {
			errors = errors->With(
					new Error(Error::SEMANTIC,
							Error::VARIABLE_NOT_A_COMPOUND_TYPE,
							m_source_expression->GetPosition().first_line,
							m_source_expression->GetPosition().first_column));
		}

		return new Result(new_value, errors);
	} else {
		return source_result;
	}
}

const LinkedList<const Error*>* WithExpression::Validate(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors = m_source_expression->Validate(
			execution_context);

	if (errors == LinkedList<const Error*>::Terminator) {
		const TypeSpecifier* type_specifier = m_source_expression->GetType(
				execution_context);

		const CompoundTypeSpecifier* as_compound =
				dynamic_cast<const CompoundTypeSpecifier*>(type_specifier);
		if (as_compound) {
			const string type_name = as_compound->GetTypeName();
			const CompoundType* type =
					execution_context->GetTypeTable()->GetType(type_name);

			if (type != CompoundType::GetDefaultCompoundType()) {
				const LinkedList<const MemberInstantiation*>* instantiation_list =
						m_member_instantiation_list;
				while (!instantiation_list->IsTerminator()) {
					const MemberInstantiation* instantiation =
							instantiation_list->GetData();
					const string* member_name = instantiation->GetName();
					const MemberDefinition* member_definition = type->GetMember(
							*member_name);

					if (member_definition
							!= MemberDefinition::GetDefaultMemberDefinition()) {
						const TypeSpecifier* member_type =
								member_definition->GetType();
						const TypeSpecifier* expression_type =
								instantiation->GetExpression()->GetType(
										execution_context);
						if (!expression_type->IsAssignableTo(member_type)) {
							//undefined member
							errors =
									errors->With(
											new Error(Error::SEMANTIC,
													Error::ASSIGNMENT_TYPE_ERROR,
													instantiation->GetExpressionPosition().first_line,
													instantiation->GetExpressionPosition().first_column,
													member_type->ToString(),
													expression_type->ToString()));
						}
					} else {
						//undefined member
						errors =
								errors->With(
										new Error(Error::SEMANTIC,
												Error::UNDECLARED_MEMBER,
												instantiation->GetNamePosition().first_line,
												instantiation->GetNamePosition().first_column,
												*member_name,
												member_definition->GetType()->ToString()));

					}

					instantiation_list = instantiation_list->GetNext();
				}
			} else {
				errors = errors->With(
						new Error(Error::SEMANTIC, Error::UNDECLARED_TYPE,
								m_source_expression->GetPosition().first_line,
								m_source_expression->GetPosition().first_column,
								type_name));
			}
		} else {
			errors = errors->With(
					new Error(Error::SEMANTIC,
							Error::VARIABLE_NOT_A_COMPOUND_TYPE,
							m_source_expression->GetPosition().first_line,
							m_source_expression->GetPosition().first_column));
		}
	}

	return errors;
}

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
#include <record_type.h>
#include <member_definition.h>
#include <member_instantiation.h>
#include <assignment_statement.h>
#include <basic_variable.h>
#include <record_type_specifier.h>
#include <sum_type_specifier.h>
#include <nested_type_specifier.h>
#include <sum_type.h>

WithExpression::WithExpression(const yy::location position,
		const_shared_ptr<Expression> source_expression,
		MemberInstantiationListRef member_instantiation_list,
		const yy::location member_instantiation_list_position) :
		Expression(position), m_source_expression(source_expression), m_member_instantiation_list(
				member_instantiation_list), m_member_instantiation_list_position(
				member_instantiation_list_position) {
}

WithExpression::~WithExpression() {
}

const_shared_ptr<TypeSpecifier> WithExpression::GetType(
		const shared_ptr<ExecutionContext> execution_context,
		AliasResolution resolution) const {
	return m_source_expression->GetType(execution_context, resolution);
}

const_shared_ptr<Result> WithExpression::Evaluate(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors(ErrorList::GetTerminator());
	const_shared_ptr<Result> source_result = m_source_expression->Evaluate(
			execution_context);

	errors = source_result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		plain_shared_ptr<void> new_value;
		const_shared_ptr<TypeSpecifier> type_specifier =
				m_source_expression->GetType(execution_context);

		string type_name;
		shared_ptr<const RecordType> type = nullptr;
		shared_ptr<const ConcreteType> member_definition = nullptr;

		const_shared_ptr<NestedTypeSpecifier> as_nested =
				std::dynamic_pointer_cast<const NestedTypeSpecifier>(
						type_specifier);

		if (as_nested) {
			auto parent = as_nested->GetParent();
			auto parent_type = execution_context->GetTypeTable()->GetType<
					SumType>(parent);

			if (parent_type) {
				type_name = *as_nested->GetMemberName();
				member_definition = parent_type->GetDefinition()->GetType<
						ConcreteType>(type_name);
			}
		} else {
			const_shared_ptr<RecordTypeSpecifier> as_record_specifier =
					std::dynamic_pointer_cast<const RecordTypeSpecifier>(
							type_specifier);
			if (as_record_specifier) {
				type_name = *as_record_specifier->GetTypeName();
				member_definition = execution_context->GetTypeTable()->GetType<
						ConcreteType>(type_name);
			}
		}

		if (member_definition) {
			auto as_record = std::dynamic_pointer_cast<const RecordType>(
					member_definition);

			if (as_record) {
				type = as_record;
			} else {
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::NOT_A_COMPOUND_TYPE,
								m_source_expression->GetPosition().begin.line,
								m_source_expression->GetPosition().begin.column,
								type_specifier->ToString()), errors);
			}
		} else {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC, Error::UNDECLARED_TYPE,
							m_source_expression->GetPosition().begin.line,
							m_source_expression->GetPosition().begin.column,
							type_name), errors);
		}

		if (ErrorList::IsTerminator(errors)) {
			auto as_record = source_result->GetData<Record>();
			const_shared_ptr<SymbolContext> definition =
					as_record->GetDefinition();

			//create a new context that isn't read-only and has no parent
			volatile_shared_ptr<SymbolContext> new_symbol_context =
					definition->Clone()->WithModifiers(
							Modifier::Type(
									definition->GetModifiers()
											& ~(Modifier::Type::READONLY)));

			volatile_shared_ptr<ExecutionContext> temp_execution_context =
					execution_context->WithContents(new_symbol_context);

			MemberInstantiationListRef subject = m_member_instantiation_list;
			while (!MemberInstantiationList::IsTerminator(subject)) {
				const_shared_ptr<MemberInstantiation> instantiation =
						subject->GetData();

				auto variable = make_shared<BasicVariable>(
						instantiation->GetName(),
						instantiation->GetNamePosition());
				errors = ErrorList::Concatenate(errors,
						variable->AssignValue(execution_context,
								instantiation->GetExpression(),
								temp_execution_context, ASSIGN));

				subject = subject->GetNext();
			}

			new_symbol_context = new_symbol_context->WithModifiers(
					definition->GetModifiers());

			new_value = as_record->WithDefinition(new_symbol_context);
		}

		return make_shared<Result>(new_value, errors);
	} else {
		return source_result;
	}
}

const bool WithExpression::IsConstant() const {
	if (m_source_expression->IsConstant()) {
		MemberInstantiationListRef subject = m_member_instantiation_list;
		while (!MemberInstantiationList::IsTerminator(subject)) {
			auto data = subject->GetData();
			if (!data->GetExpression()->IsConstant())
				return false;
			else
				subject = subject->GetNext();
		}
		return true;
	} else {
		return false;
	}
}

const ErrorListRef WithExpression::Validate(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = m_source_expression->Validate(execution_context);

	if (ErrorList::IsTerminator(errors)) {
		const_shared_ptr<TypeSpecifier> source_type_specifier =
				m_source_expression->GetType(execution_context);
		plain_shared_ptr<ComplexTypeSpecifier> complex_type_specifier = nullptr;

		string type_name;
		shared_ptr<const RecordType> type = nullptr;
		shared_ptr<const ConcreteType> member_definition = nullptr;

		const_shared_ptr<NestedTypeSpecifier> as_nested =
				std::dynamic_pointer_cast<const NestedTypeSpecifier>(
						source_type_specifier);
		if (as_nested) {
			auto parent = as_nested->GetParent();
			auto parent_type = execution_context->GetTypeTable()->GetType<
					SumType>(parent);

			if (parent_type) {
				complex_type_specifier = parent;
				type_name = *as_nested->GetMemberName();
				member_definition = parent_type->GetDefinition()->GetType<
						ConcreteType>(type_name);
			}
		} else {
			const_shared_ptr<RecordTypeSpecifier> as_record_specifier =
					std::dynamic_pointer_cast<const RecordTypeSpecifier>(
							source_type_specifier);
			if (as_record_specifier) {
				complex_type_specifier = as_record_specifier;
				type_name = *as_record_specifier->GetTypeName();
				member_definition = execution_context->GetTypeTable()->GetType<
						ConcreteType>(type_name);
			}
		}

		if (member_definition) {
			auto as_record = std::dynamic_pointer_cast<const RecordType>(
					member_definition);

			if (as_record) {
				type = as_record;
			} else {
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::NOT_A_COMPOUND_TYPE,
								m_source_expression->GetPosition().begin.line,
								m_source_expression->GetPosition().begin.column,
								source_type_specifier->ToString()), errors);
			}
		} else {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC, Error::UNDECLARED_TYPE,
							m_source_expression->GetPosition().begin.line,
							m_source_expression->GetPosition().begin.column,
							type_name), errors);
		}

		if (ErrorList::IsTerminator(errors)) {
			MemberInstantiationListRef instantiation_list =
					m_member_instantiation_list;
			while (!MemberInstantiationList::IsTerminator(instantiation_list)) {
				const_shared_ptr<MemberInstantiation> instantiation =
						instantiation_list->GetData();
				auto member_name = instantiation->GetName();
				const_shared_ptr<TypeSpecifier> member_type_specifier =
						make_shared<NestedTypeSpecifier>(complex_type_specifier,
								member_name);
				const_shared_ptr<TypeDefinition> member_definition =
						type->GetMember(*member_name);

				if (member_definition) {
					const_shared_ptr<TypeSpecifier> expression_type =
							instantiation->GetExpression()->GetType(
									execution_context);
					if (!expression_type->IsAssignableTo(member_type_specifier,
							execution_context->GetTypeTable())) {
						auto as_sum_specifier = dynamic_pointer_cast<
								const SumTypeSpecifier>(member_type_specifier);

						if (as_sum_specifier) {
							auto sum_type_name =
									as_sum_specifier->GetTypeName();
							auto sum_type =
									execution_context->GetTypeTable()->GetType<
											SumType>(sum_type_name);

							if (sum_type) {
								auto widening_analysis =
										sum_type->AnalyzeWidening(
												*expression_type,
												*sum_type_name);

								if (widening_analysis == AMBIGUOUS) {
									errors =
											ErrorList::From(
													make_shared<Error>(
															Error::SEMANTIC,
															Error::AMBIGUOUS_WIDENING_CONVERSION,
															instantiation->GetExpression()->GetPosition().begin.line,
															instantiation->GetExpression()->GetPosition().begin.column,
															member_type_specifier->ToString(),
															expression_type->ToString()),
													errors);
								} else if (widening_analysis == INCOMPATIBLE) {
									errors =
											ErrorList::From(
													make_shared<Error>(
															Error::SEMANTIC,
															Error::ASSIGNMENT_TYPE_ERROR,
															instantiation->GetExpression()->GetPosition().begin.line,
															instantiation->GetExpression()->GetPosition().begin.column,
															member_type_specifier->ToString(),
															expression_type->ToString()),
													errors);
								}
							} else {
								errors =
										ErrorList::From(
												make_shared<Error>(
														Error::SEMANTIC,
														Error::UNDECLARED_TYPE,
														instantiation->GetNamePosition().begin.line,
														instantiation->GetNamePosition().begin.column,
														member_type_specifier->ToString()),
												errors);
							}
						} else {
							//not a sum type; this is categorically an error
							errors =
									ErrorList::From(
											make_shared<Error>(Error::SEMANTIC,
													Error::ASSIGNMENT_TYPE_ERROR,
													instantiation->GetExpression()->GetPosition().begin.line,
													instantiation->GetExpression()->GetPosition().begin.column,
													member_type_specifier->ToString(),
													expression_type->ToString()),
											errors);
						}
					}
				} else {
					//undefined member
					errors =
							ErrorList::From(
									make_shared<Error>(Error::SEMANTIC,
											Error::UNDECLARED_MEMBER,
											instantiation->GetNamePosition().begin.line,
											instantiation->GetNamePosition().begin.column,
											*member_name,
											source_type_specifier->ToString()),
									errors);

				}

				instantiation_list = instantiation_list->GetNext();
			}
		}
	}

	return errors;
}

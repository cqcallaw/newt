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
#include <member_instantiation.h>
#include <assignment_statement.h>
#include <basic_variable.h>
#include <record_type_specifier.h>
#include <sum_type_specifier.h>
#include <nested_type_specifier.h>
#include <sum_type.h>
#include <record.h>
#include <alias_resolution.h>

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

TypedResult<TypeSpecifier> WithExpression::GetTypeSpecifier(
		const shared_ptr<ExecutionContext> execution_context,
		AliasResolution resolution) const {
	return m_source_expression->GetTypeSpecifier(execution_context, resolution);
}

const_shared_ptr<Result> WithExpression::Evaluate(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	ErrorListRef errors(ErrorList::GetTerminator());
	const_shared_ptr<Result> source_result = m_source_expression->Evaluate(
			context, closure);

	errors = source_result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		plain_shared_ptr<Record> new_value;
		auto source_type_specifier_result =
				m_source_expression->GetTypeSpecifier(context);

		errors = source_type_specifier_result.GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto source_type_specifier = source_type_specifier_result.GetData();

			shared_ptr<const RecordType> type = nullptr;
			auto source_type_result = source_type_specifier->GetType(
					context->GetTypeTable());

			errors = source_type_result->GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				auto source_type =
						source_type_result->GetData<TypeDefinition>();
				auto as_record = std::dynamic_pointer_cast<const RecordType>(
						source_type);

				if (as_record) {
					type = as_record;
				} else {
					errors =
							ErrorList::From(
									make_shared<Error>(Error::SEMANTIC,
											Error::INVALID_WITH_OPERAND,
											m_source_expression->GetLocation().begin.line,
											m_source_expression->GetLocation().begin.column,
											source_type_specifier->ToString()),
									errors);
				}
			}

			if (ErrorList::IsTerminator(errors)) {
				auto as_record = source_result->GetData<Record>();

				const_shared_ptr<SymbolContext> definition =
						as_record->GetDefinition();

				//create a new context that isn't read-only
				volatile_shared_ptr<SymbolContext> new_symbol_context =
						definition->Clone()->WithModifiers(
								Modifier::Type(
										definition->GetModifiers()
												| Modifier::Type::MUTABLE));
				auto type_definition = type->GetDefinition();
				auto temp_type_table = type_definition->Clone()->WithParent(
						context->GetTypeTable());

				volatile_shared_ptr<ExecutionContext> temp_execution_context =
						make_shared<ExecutionContext>(new_symbol_context,
								temp_type_table, EPHEMERAL);

				auto resolved_specifier_result = NestedTypeSpecifier::Resolve(
						source_type_specifier, context->GetTypeTable());

				errors = resolved_specifier_result.GetErrors();
				if (ErrorList::IsTerminator(errors)) {
					auto resolved_specifier =
							resolved_specifier_result.GetData();
					auto as_complex = dynamic_pointer_cast<
							const ComplexTypeSpecifier>(resolved_specifier);
					assert(as_complex);

					MemberInstantiationListRef subject =
							m_member_instantiation_list;
					while (!MemberInstantiationList::IsTerminator(subject)) {
						const_shared_ptr<MemberInstantiation> instantiation =
								subject->GetData();

						auto variable = make_shared<BasicVariable>(
								instantiation->GetName(),
								instantiation->GetNamePosition());
						errors = ErrorList::Concatenate(errors,
								variable->AssignValue(context, closure,
										instantiation->GetExpression(), ASSIGN,
										temp_execution_context, as_complex));

						subject = subject->GetNext();
					}

					new_symbol_context = new_symbol_context->WithModifiers(
							definition->GetModifiers());

					new_value = as_record->WithDefinition(new_symbol_context);
				}
			}
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
		auto source_type_specifier_result =
				m_source_expression->GetTypeSpecifier(execution_context,
						RETURN);

		errors = source_type_specifier_result.GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto source_type_specifier = source_type_specifier_result.GetData();
			shared_ptr<const TypeTable> type_table =
					execution_context->GetTypeTable();
			plain_shared_ptr<RecordType> source_type = nullptr;
			plain_shared_ptr<RecordTypeSpecifier> record_type_specifier =
					nullptr;
			auto source_type_result = source_type_specifier->GetType(
					*type_table, RESOLVE);

			errors = source_type_result->GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				auto base_type = source_type_result->GetData<TypeDefinition>();
				auto as_record = std::dynamic_pointer_cast<const RecordType>(
						base_type);
				if (as_record) {
					source_type = as_record;
					record_type_specifier = static_pointer_cast<
							const RecordTypeSpecifier>(source_type_specifier);
				} else {
					errors =
							ErrorList::From(
									make_shared<Error>(Error::SEMANTIC,
											Error::INVALID_WITH_OPERAND,
											m_source_expression->GetLocation().begin.line,
											m_source_expression->GetLocation().begin.column,
											source_type_specifier->ToString()),
									errors);
				}
			}

			if (source_type && ErrorList::IsTerminator(errors)) {
				MemberInstantiationListRef instantiation_list =
						m_member_instantiation_list;
				while (!MemberInstantiationList::IsTerminator(
						instantiation_list)) {
					const_shared_ptr<MemberInstantiation> instantiation =
							instantiation_list->GetData();
					auto member_name = instantiation->GetName();

					const_shared_ptr<TypeDefinition> member_definition =
							source_type->GetMember(*member_name);
					if (member_definition) {
						auto expression = instantiation->GetExpression();

						auto member_errors = expression->Validate(
								execution_context);
						if (ErrorList::IsTerminator(member_errors)) {
							auto expression_type_specifier_result =
									expression->GetTypeSpecifier(
											execution_context);

							auto expression_type_errors =
									expression_type_specifier_result.GetErrors();
							if (ErrorList::IsTerminator(
									expression_type_errors)) {
								auto expression_type_specifier =
										expression_type_specifier_result.GetData();

								const_shared_ptr<TypeSpecifier> member_type_specifier =
										member_definition->GetTypeSpecifier(
												member_name,
												record_type_specifier,
												GetDefaultLocation());

								auto assignability =
										expression_type_specifier->AnalyzeAssignmentTo(
												member_type_specifier,
												*type_table);
								if (assignability
										== AnalysisResult::AMBIGUOUS) {
									errors =
											ErrorList::From(
													make_shared<Error>(
															Error::SEMANTIC,
															Error::AMBIGUOUS_WIDENING_CONVERSION,
															instantiation->GetExpression()->GetLocation().begin.line,
															instantiation->GetExpression()->GetLocation().begin.column,
															member_type_specifier->ToString(),
															expression_type_specifier->ToString()),
													errors);
								} else if (assignability == INCOMPATIBLE) {
									errors =
											ErrorList::From(
													make_shared<Error>(
															Error::SEMANTIC,
															Error::ASSIGNMENT_TYPE_ERROR,
															instantiation->GetExpression()->GetLocation().begin.line,
															instantiation->GetExpression()->GetLocation().begin.column,
															member_type_specifier->ToString(),
															expression_type_specifier->ToString()),
													errors);
								}
							} else {
								errors = ErrorList::Concatenate(errors,
										expression_type_errors);
							}
						} else {
							errors = ErrorList::Concatenate(errors,
									member_errors);
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
	}

	return errors;
}

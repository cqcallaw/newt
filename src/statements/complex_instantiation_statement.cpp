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

#include <complex_instantiation_statement.h>
#include <map>
#include <execution_context.h>
#include <type_table.h>
#include <result.h>
#include <member_instantiation.h>
#include <expression.h>
#include <member_definition.h>
#include <record_type.h>
#include <symbol.h>
#include <sum_type.h>
#include <sum.h>
#include <complex_type_specifier.h>
#include <record.h>

ComplexInstantiationStatement::ComplexInstantiationStatement(
		const yy::location position,
		const_shared_ptr<ComplexTypeSpecifier> type_specifier,
		const yy::location type_position, const_shared_ptr<string> name,
		const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) :
		DeclarationStatement(position, name, name_position,
				initializer_expression, ModifierList::GetTerminator(),
				GetDefaultLocation()), m_type_specifier(type_specifier), m_type_position(
				type_position) {
}

ComplexInstantiationStatement::~ComplexInstantiationStatement() {
}

const ErrorListRef ComplexInstantiationStatement::preprocess(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();
	//TODO: validate that all members are initialized for readonly structs (?)

	const_shared_ptr<TypeDefinition> type =
			execution_context->GetTypeTable()->GetType<TypeDefinition>(
					m_type_specifier->GetTypeName());
	if (type) {
		auto existing = execution_context->GetSymbol(GetName(), SHALLOW);
		if (existing == Symbol::GetDefaultSymbol()) {
			const_shared_ptr<RecordType> as_record = dynamic_pointer_cast<
					const RecordType>(type);
			if (as_record) {
				plain_shared_ptr<const Record> instance;
				if (GetInitializerExpression()) {
					const_shared_ptr<TypeSpecifier> initializer_expression_type =
							GetInitializerExpression()->GetType(
									execution_context);
					errors = GetInitializerExpression()->Validate(
							execution_context);

					if (ErrorList::IsTerminator(errors)) {
						const_shared_ptr<RecordTypeSpecifier> as_record_specifier =
								std::dynamic_pointer_cast<
										const RecordTypeSpecifier>(
										initializer_expression_type);

						if (as_record_specifier
								&& initializer_expression_type->IsAssignableTo(
										m_type_specifier)) {
							if (GetInitializerExpression()->IsConstant()) {
								const_shared_ptr<Result> result =
										GetInitializerExpression()->Evaluate(
												execution_context);
								errors = result->GetErrors();
								if (ErrorList::IsTerminator(errors)) {
									instance =
											static_pointer_cast<const Record>(
													result->GetData());
								}
							} else {
								//generate default instance
								instance = Record::GetDefaultInstance(
										m_type_specifier->GetTypeName(),
										as_record);
							}
						} else {
							errors =
									ErrorList::From(
											make_shared<Error>(Error::SEMANTIC,
													Error::ASSIGNMENT_TYPE_ERROR,
													GetInitializerExpression()->GetPosition().begin.line,
													GetInitializerExpression()->GetPosition().begin.column,
													m_type_specifier->GetTypeName(),
													initializer_expression_type->ToString()),
											errors);
						}
					}
				} else {
					instance = Record::GetDefaultInstance(
							m_type_specifier->GetTypeName(), as_record);
				}

				if (ErrorList::IsTerminator(errors)) {
					//we've been able to get a good initial value (that is, no errors have occurred)
					auto symbol = make_shared<Symbol>(instance);
					const InsertResult insert_result =
							execution_context->InsertSymbol(*GetName(), symbol);

					if (insert_result != INSERT_SUCCESS) {
						assert(false);
					}
				}
			}

			const_shared_ptr<SumType> as_sum = dynamic_pointer_cast<
					const SumType>(type);
			if (as_sum) {
				auto sum_type_specifier = make_shared<SumTypeSpecifier>(
						m_type_specifier);
				plain_shared_ptr<const Sum> instance;
				if (GetInitializerExpression()) {
					const_shared_ptr<TypeSpecifier> initializer_expression_type =
							GetInitializerExpression()->GetType(
									execution_context);
					errors = GetInitializerExpression()->Validate(
							execution_context);

					if (ErrorList::IsTerminator(errors)) {
						auto widening_analysis = as_sum->AnalyzeWidening(
								*initializer_expression_type);
						if (initializer_expression_type->IsAssignableTo(
								sum_type_specifier)) {
							//generate default instance; actual assignment must be done in execute stage
							//this is because assignment of constant expressions to sum types is only valid if the constant expression is narrower than the sum type
							instance = Sum::GetDefaultInstance(
									sum_type_specifier, as_sum);
						} else if (widening_analysis == UNAMBIGUOUS) {
							if (GetInitializerExpression()->IsConstant()) {
								//assignment of constant expressions to sum types is only valid if constant expression is narrower than the sum type
								//we can therefore assume that a new Sum must be created if we've hit this branch
								//widening conversions of non-constant initializers must be handled in the execute stage
								const_shared_ptr<Result> result =
										GetInitializerExpression()->Evaluate(
												execution_context);
								errors = result->GetErrors();
								if (ErrorList::IsTerminator(errors)) {
									auto variant_name =
											as_sum->MapSpecifierToVariant(
													*initializer_expression_type);
									instance = make_shared<Sum>(
											sum_type_specifier, variant_name,
											result->GetData());
								}
							}
						} else if (widening_analysis == AMBIGUOUS) {
							errors =
									ErrorList::From(
											make_shared<Error>(Error::SEMANTIC,
													Error::AMBIGUOUS_WIDENING_CONVERSION,
													GetInitializerExpression()->GetPosition().begin.line,
													GetInitializerExpression()->GetPosition().begin.column,
													m_type_specifier->GetTypeName(),
													initializer_expression_type->ToString()),
											errors);
						} else {
							errors =
									ErrorList::From(
											make_shared<Error>(Error::SEMANTIC,
													Error::ASSIGNMENT_TYPE_ERROR,
													GetInitializerExpression()->GetPosition().begin.line,
													GetInitializerExpression()->GetPosition().begin.column,
													m_type_specifier->GetTypeName(),
													initializer_expression_type->ToString()),
											errors);
						}
					}
				} else {
					instance = Sum::GetDefaultInstance(sum_type_specifier,
							as_sum);
				}

				if (ErrorList::IsTerminator(errors)) {
					//we've been able to get a good initial value (that is, no errors have occurred)
					auto symbol = make_shared<Symbol>(instance);
					const InsertResult insert_result =
							execution_context->InsertSymbol(*GetName(), symbol);

					if (insert_result != INSERT_SUCCESS) {
						assert(false);
					}
				}
			}
		} else {
			//symbol already exists
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::PREVIOUS_DECLARATION,
							m_type_position.begin.line,
							m_type_position.begin.column, *GetName()), errors);
		}
	} else {
		//type does not exist
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC, Error::UNDECLARED_TYPE,
						m_type_position.begin.line,
						m_type_position.begin.column,
						m_type_specifier->GetTypeName()), errors);
	}

	return errors;
}

const_shared_ptr<TypeSpecifier> ComplexInstantiationStatement::GetType() const {
	return m_type_specifier;
}

const ErrorListRef ComplexInstantiationStatement::execute(
		shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	if (GetInitializerExpression()
			&& !GetInitializerExpression()->IsConstant()) {
		const_shared_ptr<TypeDefinition> type =
				execution_context->GetTypeTable()->GetType<TypeDefinition>(
						m_type_specifier->GetTypeName());

		const_shared_ptr<RecordType> as_record = dynamic_pointer_cast<
				const RecordType>(type);
		if (as_record) {
			const_shared_ptr<Result> evaluation =
					GetInitializerExpression()->Evaluate(execution_context);

			errors = evaluation->GetErrors();

			if (ErrorList::IsTerminator(errors)) {
				auto void_value = evaluation->GetData();
				const_shared_ptr<Record> instance = static_pointer_cast<
						const Record>(void_value);
				execution_context->SetSymbol(*GetName(), instance);
			}
		}

		const_shared_ptr<SumType> as_sum = dynamic_pointer_cast<const SumType>(
				type);
		if (as_sum) {
			const_shared_ptr<Result> evaluation =
					GetInitializerExpression()->Evaluate(execution_context);

			errors = evaluation->GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				auto void_value = evaluation->GetData();
				const_shared_ptr<Sum> instance = static_pointer_cast<const Sum>(
						void_value);
				execution_context->SetSymbol(*GetName(), instance);
			}
		}
	}

	return errors;
}

const DeclarationStatement* ComplexInstantiationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
	return new ComplexInstantiationStatement(GetPosition(), m_type_specifier,
			m_type_position, GetName(), GetNamePosition(), expression);
}

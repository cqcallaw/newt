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

#include <maybe_declaration_statement.h>
#include <type_alias_declaration_statement.h>
#include <maybe_type_specifier.h>
#include <nested_type_specifier.h>
#include <sum_type_specifier.h>
#include <execution_context.h>
#include <sum_type.h>
#include <sum.h>
#include <unit_type.h>

MaybeDeclarationStatement::MaybeDeclarationStatement(
		const yy::location location,
		const_shared_ptr<MaybeTypeSpecifier> type_specifier,
		const yy::location type_specifier_location,
		const_shared_ptr<string> name, const yy::location name_location,
		const_shared_ptr<Expression> initializer_expression) :
		DeclarationStatement(location, name, name_location,
				initializer_expression, ModifierList::GetTerminator(),
				GetDefaultLocation()), m_type_specifier(type_specifier), m_type_specifier_location(
				type_specifier_location) {
}

MaybeDeclarationStatement::~MaybeDeclarationStatement() {
}

const ErrorListRef MaybeDeclarationStatement::preprocess(
		const std::shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	auto type_table = execution_context->GetTypeTable();
	auto root_specifier = m_type_specifier->GetTypeSpecifier();
	auto root_type = root_specifier->GetType(type_table);
	if (root_type) {
		//build up our sum type representation of the Maybe
		auto unit_declaration = make_shared<TypeAliasDeclarationStatement>(
				m_type_specifier_location, TypeTable::GetNilTypeSpecifier(),
				GetDefaultLocation(), MaybeTypeSpecifier::EMPTY_NAME,
				GetDefaultLocation());

		auto type_declaration = make_shared<TypeAliasDeclarationStatement>(
				m_type_specifier_location, root_specifier, GetDefaultLocation(),
				MaybeTypeSpecifier::VARIANT_NAME, GetDefaultLocation());

		auto declaration_list = DeclarationList::From(unit_declaration,
				DeclarationList::GetTerminator());
		declaration_list = DeclarationList::From(type_declaration,
				declaration_list);

		auto maybe_type_name = make_shared<std::string>(
				m_type_specifier->ToString());
		auto specifier = make_shared<SumTypeSpecifier>(maybe_type_name);
		auto build_result = SumType::Build(execution_context, declaration_list,
				specifier);

		errors = build_result->GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto maybe_type = build_result->GetData<SumType>();

			//create a new Maybe type with our built type definition
			//this technique is disadvantageous in that every time we have a new Maybe declaration,
			//a new sum type is created, even if the Maybes are of the same type
			auto maybe_type_specifier = make_shared<MaybeTypeSpecifier>(
					m_type_specifier->GetTypeSpecifier(), maybe_type);

			plain_shared_ptr<Sum> value = make_shared<Sum>(
					MaybeTypeSpecifier::EMPTY_NAME,
					TypeTable::GetNilType()->GetValue());
			auto initializer = GetInitializerExpression();
			if (initializer) {
				auto initializer_type_specifier = initializer->GetTypeSpecifier(
						execution_context, RESOLVE);
				if (initializer_type_specifier->AnalyzeAssignmentTo(root_specifier,
						*type_table)) {
					if (initializer->IsConstant()) {
						auto result = initializer->Evaluate(execution_context);
						errors = result->GetErrors();
						if (ErrorList::IsTerminator(errors)) {
							if (*initializer_type_specifier
									== *maybe_type_specifier) {
								//direct assignment
								value = result->GetData<Sum>();
							} else {
								//widening conversion
								value = make_shared<Sum>(
										MaybeTypeSpecifier::VARIANT_NAME,
										result->GetRawData());
							}
						}
					} else {
						value = static_pointer_cast<const Sum>(
								maybe_type->GetDefaultValue(type_table));
					}
				} else if (initializer_type_specifier->AnalyzeAssignmentTo(
						TypeTable::GetNilTypeSpecifier(), *type_table)) {
					//do nothing; our default value is fine
				} else {
					errors =
							ErrorList::From(
									make_shared<Error>(Error::SEMANTIC,
											Error::INVALID_INITIALIZER_TYPE,
											GetInitializerExpression()->GetPosition().begin.line,
											GetInitializerExpression()->GetPosition().begin.column,
											*GetName(),
											maybe_type_specifier->ToString(),
											initializer_type_specifier->ToString()),
									errors);
				}
			}

			if (ErrorList::IsTerminator(errors)) {
				auto symbol = make_shared<Symbol>(maybe_type_specifier, value);

				auto insert_result = execution_context->InsertSymbol(*GetName(),
						symbol);
				if (insert_result == SYMBOL_EXISTS) {
					errors = ErrorList::From(
							make_shared<Error>(Error::SEMANTIC,
									Error::PREVIOUS_DECLARATION,
									GetNamePosition().begin.line,
									GetNamePosition().begin.column, *GetName()),
							errors);
				}
			}
		}
	} else {
		//type does not exist
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC, Error::UNDECLARED_TYPE,
						m_type_specifier_location.begin.line,
						m_type_specifier_location.begin.column,
						m_type_specifier->GetTypeSpecifier()->ToString()),
				errors);
	}
	return errors;
}

const ErrorListRef MaybeDeclarationStatement::execute(
		std::shared_ptr<ExecutionContext> execution_context) const {
	auto errors = ErrorList::GetTerminator();

	auto type_table = execution_context->GetTypeTable();
	auto root_specifier = m_type_specifier->GetTypeSpecifier();

	auto existing = execution_context->GetSymbol(*GetName());
	assert(existing);
	auto maybe_type_specifier = dynamic_pointer_cast<const MaybeTypeSpecifier>(
			existing->GetTypeSpecifier());
	assert(maybe_type_specifier);

	auto initializer = GetInitializerExpression();
	if (initializer) {
		if (!initializer->IsConstant()) {
			plain_shared_ptr<Sum> value = nullptr;
			auto initializer_type_specifier = initializer->GetTypeSpecifier(
					execution_context, RESOLVE);
			if (initializer_type_specifier->AnalyzeAssignmentTo(root_specifier,
					*type_table)) {
				auto result = initializer->Evaluate(execution_context);
				errors = result->GetErrors();
				if (ErrorList::IsTerminator(errors)) {
					if (*initializer_type_specifier == *maybe_type_specifier) {
						//direct assignment
						value = result->GetData<Sum>();
					} else {
						//widening conversion
						value = make_shared<Sum>(
								MaybeTypeSpecifier::VARIANT_NAME,
								result->GetRawData());
					}
				}
			} else if (initializer_type_specifier->AnalyzeAssignmentTo(
					TypeTable::GetNilTypeSpecifier(), *type_table)) {
				//no-op: we don't need to re-assign the nil value
			} else {
				errors =
						ErrorList::From(
								make_shared<Error>(Error::RUNTIME,
										Error::INVALID_INITIALIZER_TYPE,
										GetInitializerExpression()->GetPosition().begin.line,
										GetInitializerExpression()->GetPosition().begin.column,
										*GetName(),
										maybe_type_specifier->ToString(),
										initializer_type_specifier->ToString()),
								errors);
			}

			if (ErrorList::IsTerminator(errors) && value) {
				auto set_result = execution_context->SetSymbol(*GetName(),
						maybe_type_specifier, value, *type_table);

				errors = ToErrorListRef(set_result, GetPosition(), GetName(),
						maybe_type_specifier, initializer_type_specifier);
			}
		}
	}

	return errors;
}

const ErrorListRef MaybeDeclarationStatement::GetReturnStatementErrors(
		const_shared_ptr<TypeSpecifier> type_specifier,
		const std::shared_ptr<ExecutionContext> execution_context) const {
	return ErrorList::GetTerminator();
}

const DeclarationStatement* MaybeDeclarationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
	return this; //no-op
}

const_shared_ptr<TypeSpecifier> MaybeDeclarationStatement::GetTypeSpecifier() const {
	return m_type_specifier;
}

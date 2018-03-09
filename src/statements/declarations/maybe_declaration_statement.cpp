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

const PreprocessResult MaybeDeclarationStatement::Preprocess(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure,
		const_shared_ptr<TypeSpecifier> return_type_specifier) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	auto type_table = context->GetTypeTable();
	auto root_specifier = m_type_specifier->GetBaseTypeSpecifier();
	auto root_type_result = root_specifier->GetType(type_table);
	errors = root_type_result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		plain_shared_ptr<Sum> value = make_shared<Sum>(TypeTable::GetNilName(),
				TypeTable::GetNilType()->GetValue());
		auto initializer = GetInitializerExpression();
		if (initializer) {
			auto initializer_type_specifier_result =
					initializer->GetTypeSpecifier(context, RESOLVE);

			errors = initializer_type_specifier_result.GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				auto initializer_type_specifier =
						initializer_type_specifier_result.GetData();

				if (initializer_type_specifier->AnalyzeAssignmentTo(
						m_type_specifier, *type_table)) {
					if (*initializer_type_specifier
							!= *TypeTable::GetNilTypeSpecifier()) {
						errors = initializer->Validate(context);
						if (ErrorList::IsTerminator(errors)) {
							if (initializer->IsConstant()) {
								auto result = initializer->Evaluate(context,
										closure);
								errors = result->GetErrors();
								if (ErrorList::IsTerminator(errors)) {
									if (*initializer_type_specifier
											== *m_type_specifier) {
										//direct assignment
										value = result->GetData<Sum>();
									} else {
										//widening conversion
										value =
												make_shared<Sum>(
														MaybeTypeSpecifier::VARIANT_NAME,
														result->GetRawData());

										auto type = m_type_specifier->GetType(
												type_table, RESOLVE)->GetData<
												SumType>();
									}
								}
							} else {
								value =
										static_pointer_cast<const Sum>(
												m_type_specifier->DefaultValue(
														type_table,
														ComplexType::DefaultTypeParameterMap));
							}
						}
					}
				} else {
					errors =
							ErrorList::From(
									make_shared<Error>(Error::SEMANTIC,
											Error::INVALID_INITIALIZER_TYPE,
											GetInitializerExpression()->GetLocation().begin,
											*GetName(),
											m_type_specifier->ToString(),
											initializer_type_specifier->ToString()),
									errors);
				}
			}
		}

		if (ErrorList::IsTerminator(errors)) {
			auto symbol = make_shared<Symbol>(m_type_specifier, value);

			auto insert_result = context->InsertSymbol(*GetName(), symbol);
			if (insert_result == SYMBOL_EXISTS) {
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::PREVIOUS_DECLARATION,
								GetNameLocation().begin, *GetName()), errors);
			}
		}

	}

	return PreprocessResult(PreprocessResult::ReturnCoverage::NONE, errors);
}

const ExecutionResult MaybeDeclarationStatement::Execute(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	auto errors = ErrorList::GetTerminator();

	auto type_table = context->GetTypeTable();
	auto root_specifier = m_type_specifier->GetBaseTypeSpecifier();

	auto existing = context->GetSymbol(*GetName());
	assert(existing);
	auto maybe_type_specifier = dynamic_pointer_cast<const MaybeTypeSpecifier>(
			existing->GetTypeSpecifier());
	assert(maybe_type_specifier);

	auto initializer = GetInitializerExpression();
	if (initializer) {
		if (!initializer->IsConstant()) {
			plain_shared_ptr<Sum> value = nullptr;
			auto initializer_type_specifier_result =
					initializer->GetTypeSpecifier(context, RESOLVE);

			errors = initializer_type_specifier_result.GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				auto initializer_type_specifier =
						initializer_type_specifier_result.GetData();
				if (initializer_type_specifier->AnalyzeAssignmentTo(
						m_type_specifier, *type_table)) {
					if (*initializer_type_specifier
							!= *TypeTable::GetNilTypeSpecifier()) {
						auto result = initializer->Evaluate(context, closure);
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
					}
				} else {
					errors =
							ErrorList::From(
									make_shared<Error>(Error::RUNTIME,
											Error::INVALID_INITIALIZER_TYPE,
											GetInitializerExpression()->GetLocation().begin,
											*GetName(),
											maybe_type_specifier->ToString(),
											initializer_type_specifier->ToString()),
									errors);
				}

				if (ErrorList::IsTerminator(errors) && value) {
					auto set_result = context->SetSymbol(*GetName(),
							maybe_type_specifier, value, *type_table);

					errors = ToErrorListRef(set_result, GetLocation(),
							GetName(), maybe_type_specifier,
							initializer_type_specifier);
				}
			}
		}
	}

	return ExecutionResult(errors);
}

const DeclarationStatement* MaybeDeclarationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
	return new MaybeDeclarationStatement(GetLocation(), m_type_specifier,
			m_type_specifier_location, GetName(), GetNameLocation(), expression);
}

const_shared_ptr<TypeSpecifier> MaybeDeclarationStatement::GetTypeSpecifier() const {
	return m_type_specifier;
}

const yy::location MaybeDeclarationStatement::GetTypeSpecifierLocation() const {
	return m_type_specifier_location;
}

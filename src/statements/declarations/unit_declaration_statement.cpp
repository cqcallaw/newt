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

#include <unit_declaration_statement.h>
#include <execution_context.h>
#include <unit_type.h>
#include <maybe_type.h>

UnitDeclarationStatement::UnitDeclarationStatement(const yy::location location,
		const_shared_ptr<TypeSpecifier> type_specifier,
		const yy::location type_specifier_location,
		const_shared_ptr<string> name, const yy::location name_location) :
		DeclarationStatement(location, name, name_location, nullptr,
				ModifierList::GetTerminator(), GetDefaultLocation()), m_type_specifier(
				type_specifier), m_type_specifier_location(
				type_specifier_location) {
}

UnitDeclarationStatement::~UnitDeclarationStatement() {
}

const PreprocessResult UnitDeclarationStatement::Preprocess(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure,
		const TypeSpecifierListRef type_parameter_list,
		const_shared_ptr<TypeSpecifier> return_type_specifier) const {
	auto errors = ErrorList::GetTerminator();

	auto type_table = context->GetTypeTable();

	auto complex_type_specifier = dynamic_pointer_cast<
			const ComplexTypeSpecifier>(m_type_specifier);
	if (complex_type_specifier) {
		auto type_name = complex_type_specifier->GetTypeName();
		plain_shared_ptr<TypeDefinition> type = nullptr;
		if (complex_type_specifier->GetTypeName()->compare(
				*TypeTable::GetNilName()) != 0) {
			if (!type_table->ContainsType(*complex_type_specifier)) {
				auto maybe_type_result = MaybeType::Build(context,
						complex_type_specifier);
				errors = maybe_type_result->GetErrors();
				if (ErrorList::IsTerminator(errors)) {
					auto maybe_type = maybe_type_result->GetData<MaybeType>();
					type = make_shared<UnitType>(maybe_type);
					type_table->AddType(*type_name, type);
				}
			} else {
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::PREVIOUS_DECLARATION,
								GetNameLocation().begin, *GetName()), errors);
			}
		}
	} else {
		// invalid type specifer. This should never happen, but we can't use an ComplexTypeSpecifier because of GetDeclarationStatement interface concerns
		assert(false);
	}

	return PreprocessResult(PreprocessResult::ReturnCoverage::NONE, errors);
}

const ExecutionResult UnitDeclarationStatement::Execute(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	return ExecutionResult();
}

const_shared_ptr<TypeSpecifier> UnitDeclarationStatement::GetTypeSpecifier() const {
	return m_type_specifier;
}

const yy::location UnitDeclarationStatement::GetTypeSpecifierLocation() const {
	return m_type_specifier_location;
}

const DeclarationStatement* UnitDeclarationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
	return this; //no-op
}

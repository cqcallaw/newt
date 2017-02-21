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
		const_shared_ptr<TypeSpecifier> return_type_specifier) const {
	auto errors = ErrorList::GetTerminator();

	auto type_table = context->GetTypeTable();

	auto as_complex = dynamic_pointer_cast<const ComplexTypeSpecifier>(
			m_type_specifier);
	if (as_complex) {
		auto type_name = as_complex->GetTypeName();
		plain_shared_ptr<TypeDefinition> type = nullptr;
		if (as_complex->GetTypeName()->compare(*TypeTable::GetNilName()) != 0) {
			auto type_result = m_type_specifier->GetType(type_table, RESOLVE);
			type = type_result->GetData<TypeDefinition>();
			if (!type) {
				type = make_shared<UnitType>();
				type_table->AddType(*type_name, type);
			}
		} else {
			type = TypeTable::GetNilType();
		}

		if (type) {
			auto symbol = type->GetSymbol(type_table, m_type_specifier,
					nullptr);
			InsertResult insert_result = context->InsertSymbol(*GetName(),
					symbol);
			if (insert_result == SYMBOL_EXISTS) {
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::PREVIOUS_DECLARATION,
								GetNameLocation().begin.line,
								GetNameLocation().begin.column, *GetName()),
						errors);
			}
		} else {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC, Error::UNDECLARED_TYPE,
							GetNameLocation().begin.line,
							GetNameLocation().begin.column,
							m_type_specifier->ToString()), errors);
		}
	} else {
		assert(false);
	}

	return PreprocessResult(PreprocessResult::ReturnCoverage::NONE, errors);
}

const ErrorListRef UnitDeclarationStatement::Execute(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	return ErrorList::GetTerminator();
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

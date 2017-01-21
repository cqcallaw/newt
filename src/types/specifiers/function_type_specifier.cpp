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

#include <function_type_specifier.h>
#include <variant_function_specifier.h>
#include <typeinfo>
#include <sstream>
#include <assert.h>
#include <function_expression.h>
#include <constant_expression.h>
#include <declaration_statement.h>
#include <function_declaration_statement.h>
#include <return_statement.h>
#include <statement_block.h>
#include <function.h>
#include <function_type.h>
#include <function_declaration.h>
#include <execution_context.h>
#include <type_table.h>
#include <sum_type.h>

FunctionTypeSpecifier::FunctionTypeSpecifier(
		TypeSpecifierListRef parameter_type_list,
		const_shared_ptr<TypeSpecifier> return_type,
		const yy::location return_type_location, const yy::location location) :
		TypeSpecifier(location), m_parameter_type_list(parameter_type_list), m_return_type(
				return_type), m_return_type_location(return_type_location) {
}

FunctionTypeSpecifier::FunctionTypeSpecifier(const FunctionTypeSpecifier& other) :
		m_parameter_type_list(other.m_parameter_type_list), m_return_type(
				other.m_return_type) {
}

FunctionTypeSpecifier::~FunctionTypeSpecifier() {
}

const string FunctionTypeSpecifier::ToString() const {
	ostringstream buffer;
	buffer << "(";
	TypeSpecifierListRef subject = m_parameter_type_list;
	while (!TypeSpecifierList::IsTerminator(subject)) {
		const_shared_ptr<TypeSpecifier> type = subject->GetData();
		buffer << type->ToString();
		subject = subject->GetNext();

		if (!TypeSpecifierList::IsTerminator(subject)) {
			//add separator
			buffer << ", ";
		}
	}
	buffer << ") -> " << m_return_type->ToString() << "";
	return buffer.str();
}

const AnalysisResult FunctionTypeSpecifier::AnalyzeAssignmentTo(
		const_shared_ptr<TypeSpecifier> other,
		const TypeTable& type_table) const {
	if (*this == *other) {
		return EQUIVALENT;
	} else {
		auto as_variant_specifier = dynamic_pointer_cast<
				const VariantFunctionSpecifier>(other);
		if (as_variant_specifier) {
			auto variant_list = as_variant_specifier->GetVariantList();
			if (FunctionVariantList::IsTerminator(variant_list->GetNext())) {
				//single-item variant list; test FunctionTypeSpecifier assignment
				auto variant = variant_list->GetData();
				auto declaration = variant->GetDeclaration();

				auto result = AnalyzeAssignmentTo(declaration, type_table);
				return result;
			}
		}
	}

	return INCOMPATIBLE;
}

bool FunctionTypeSpecifier::operator ==(const TypeSpecifier& other) const {
	try {
		const FunctionTypeSpecifier& as_function =
				dynamic_cast<const FunctionTypeSpecifier&>(other);
		if (*m_return_type == *as_function.GetReturnTypeSpecifier()) {
			TypeSpecifierListRef subject = m_parameter_type_list;
			TypeSpecifierListRef other_subject =
					as_function.GetParameterTypeList();
			while (!TypeSpecifierList::IsTerminator(subject)
					&& !TypeSpecifierList::IsTerminator(other_subject)) {
				const_shared_ptr<TypeSpecifier> type = subject->GetData();
				const_shared_ptr<TypeSpecifier> other_type =
						other_subject->GetData();
				if (*type == *other_type) {
					subject = subject->GetNext();
					other_subject = other_subject->GetNext();
				} else {
					return false;
				}
			}

			if (!TypeSpecifierList::IsTerminator(subject)
					|| !TypeSpecifierList::IsTerminator(other_subject)) {
				// parameter length mismatch
				return false;
			}

			return true;
		} else {
			return false;
		}
	} catch (std::bad_cast& e) {
		return false;
	}
}

const_shared_ptr<Result> FunctionTypeSpecifier::GetType(
		const TypeTable& type_table, AliasResolution resolution) const {
	auto default_declaration_result = FunctionDeclaration::FromTypeSpecifier(
			*this, type_table);
	auto errors = default_declaration_result->GetErrors();

	plain_shared_ptr<FunctionType> type = nullptr;
	if (ErrorList::IsTerminator(errors)) {
		auto default_declaration = default_declaration_result->GetData<
				FunctionDeclaration>();
		type = make_shared<const FunctionType>(
				default_declaration->GetParameterList(), m_return_type,
				m_return_type_location);
	}

	return make_shared<Result>(type, errors);
}

const ErrorListRef FunctionTypeSpecifier::ValidateDeclaration(
		const TypeTable& type_table, const yy::location position) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	auto subject = GetParameterTypeList();
	while (!TypeSpecifierList::IsTerminator(subject)) {
		auto parameter_type_specifier = subject->GetData();
		auto parameter_errors = parameter_type_specifier->ValidateDeclaration(
				type_table, parameter_type_specifier->GetLocation());
		errors = ErrorList::Concatenate(errors, parameter_errors);
		subject = subject->GetNext();
	}

	auto return_type_errors = m_return_type->ValidateDeclaration(type_table,
			m_return_type_location);
	errors = ErrorList::Concatenate(errors, return_type_errors);

	return errors;
}

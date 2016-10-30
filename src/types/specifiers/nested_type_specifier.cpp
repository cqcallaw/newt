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

#include <sstream>
#include <nested_type_specifier.h>
#include <complex_type_specifier.h>
#include <sum_type.h>
#include <record_type.h>
#include <primitive_type.h>
#include <unit_type.h>
#include <placeholder_type.h>
#include <record_type_specifier.h>

NestedTypeSpecifier::NestedTypeSpecifier(const_shared_ptr<TypeSpecifier> parent,
		const_shared_ptr<std::string> member_name, const yy::location location) :
		TypeSpecifier(location), m_parent(parent), m_member_name(member_name) {
}

NestedTypeSpecifier::~NestedTypeSpecifier() {
}

const std::string NestedTypeSpecifier::ToString() const {
	std::ostringstream buffer;
	buffer << m_parent->ToString() << "." << *m_member_name;
	return buffer.str();
}

const AnalysisResult NestedTypeSpecifier::AnalyzeAssignmentTo(
		const_shared_ptr<TypeSpecifier> other,
		const TypeTable& type_table) const {
	auto other_as_nested = dynamic_pointer_cast<const NestedTypeSpecifier>(
			other);

	if (other_as_nested) {
		if (*other_as_nested->GetParent() == *m_parent) {
			if (*other_as_nested->GetMemberName() == *m_member_name) {
				return AnalysisResult::EQUIVALENT;
			}
		}
	}

	return AnalysisResult::INCOMPATIBLE;
}

bool NestedTypeSpecifier::operator ==(const TypeSpecifier& other) const {
	try {
		const NestedTypeSpecifier& as_nested =
				dynamic_cast<const NestedTypeSpecifier&>(other);
		return *as_nested.m_parent == *m_parent
				&& *as_nested.m_member_name == *m_member_name;
	} catch (std::bad_cast& e) {
		return false;
	}
}

const_shared_ptr<Result> NestedTypeSpecifier::GetType(
		const TypeTable& type_table, AliasResolution resolution) const {
	auto parent_type_result = m_parent->GetType(type_table);

	plain_shared_ptr<TypeDefinition> type = nullptr;
	auto errors = parent_type_result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto parent_type = parent_type_result->GetData<TypeDefinition>();
		auto complex_parent = dynamic_pointer_cast<const ComplexType>(
				parent_type);
		if (complex_parent) {
			auto as_placeholder = dynamic_pointer_cast<const PlaceholderType>(
					complex_parent);
			if (as_placeholder) {
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::PARTIALLY_DECLARED_TYPE,
								GetLocation().begin.line,
								GetLocation().begin.column,
								m_parent->ToString()), errors);
			} else {
				type = complex_parent->GetDefinition()->GetType<TypeDefinition>(
						m_member_name, DEEP, resolution);

				if (!type) {
					errors = ErrorList::From(
							make_shared<Error>(Error::SEMANTIC,
									Error::UNDECLARED_MEMBER,
									m_parent->GetLocation().begin.line,
									m_parent->GetLocation().begin.column,
									*m_member_name, m_parent->ToString()),
							errors);

				}
			}
		} else {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::NOT_A_COMPOUND_TYPE,
							m_parent->GetLocation().begin.line,
							m_parent->GetLocation().begin.column,
							m_parent->ToString()), errors);
		}
	}

	return make_shared<Result>(type, errors);
}

const ErrorListRef NestedTypeSpecifier::ValidateDeclaration(
		const TypeTable& type_table, const yy::location position) const {
	auto existing_type_result = GetType(type_table);

	auto errors = existing_type_result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto type = existing_type_result->GetData<TypeDefinition>();
		auto as_placeholder = dynamic_pointer_cast<const PlaceholderType>(type);
		if (as_placeholder) {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::RAW_RECURSIVE_DECLARATION,
							position.begin.line, position.begin.column),
					errors);
		}
	}

	return errors;
}

TypedResult<TypeSpecifier> NestedTypeSpecifier::Resolve(
		const_shared_ptr<TypeSpecifier> source, const TypeTable& type_table) {
	auto as_nested = dynamic_pointer_cast<const NestedTypeSpecifier>(source);

	if (as_nested) {
		auto parent_type_result = as_nested->GetParent()->GetType(type_table,
				RESOLVE);
		if (ErrorList::IsTerminator(parent_type_result->GetErrors())) {
			auto resolved_parent_result = Resolve(as_nested->GetParent(),
					type_table);

			if (ErrorList::IsTerminator(resolved_parent_result.GetErrors())) {
				auto resolved_parent = resolved_parent_result.GetData();
				auto resolved_parent_as_complex = dynamic_pointer_cast<
						const ComplexTypeSpecifier>(resolved_parent);
				if (resolved_parent_as_complex) {
					auto parent_type_result =
							resolved_parent_as_complex->GetType(type_table);

					if (ErrorList::IsTerminator(
							parent_type_result->GetErrors())) {
						auto type =
								parent_type_result->GetData<TypeDefinition>();
						auto as_complex_type = dynamic_pointer_cast<
								const ComplexType>(type);
						if (as_complex_type) {
							auto as_placholder = dynamic_pointer_cast<
									const PlaceholderType>(as_complex_type);
							if (as_placholder) {
								return TypedResult<TypeSpecifier>(
										PrimitiveTypeSpecifier::GetNone());
							}

							auto type_definition =
									as_complex_type->GetDefinition()->GetType<
											TypeDefinition>(
											as_nested->m_member_name, DEEP,
											RETURN);
							auto as_alias = dynamic_pointer_cast<
									const AliasDefinition>(type_definition);
							if (as_alias) {
								return as_alias->GetOriginal();
							} else {
								auto type = parent_type_result->GetData<
										TypeDefinition>();
								return type->GetTypeSpecifier(
										as_nested->m_member_name,
										resolved_parent_as_complex,
										source->GetLocation());
							}
						}
					}
				}
			}
		}
	}

	return source;
}

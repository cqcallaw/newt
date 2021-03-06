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

#include <variant_function_specifier.h>
#include <algorithm>
#include <vector>
#include <set>
#include <sstream>
#include <function_declaration.h>
#include <iterator>
#include <variant_function_type.h>

VariantFunctionSpecifier::VariantFunctionSpecifier(const yy::location location,
		const FunctionVariantListRef variant_list) :
		TypeSpecifier(location), m_variant_list(variant_list) {
}

VariantFunctionSpecifier::~VariantFunctionSpecifier() {
}

const string VariantFunctionSpecifier::ToString() const {
	ostringstream buffer;

	auto subject = m_variant_list;

	if (FunctionVariantList::IsTerminator(subject->GetNext())) {
		// single variant specifier
		buffer << subject->GetData()->GetDeclaration()->ToString();
	} else {
		// multi-variant specifier
		set<string> return_value_set;
		set<string>::iterator return_values_iterator;
		buffer << "(...) -> ";

		while (!FunctionVariantList::IsTerminator(subject)) {
			auto variant = subject->GetData();
			auto declaration = variant->GetDeclaration();
			auto return_type_specifier = declaration->GetReturnTypeSpecifier();
			return_value_set.insert(return_type_specifier->ToString());
			subject = subject->GetNext();
		}

		if (!return_value_set.empty()) {
			if (return_value_set.size() == 1) {
				buffer << *return_value_set.begin();
			} else {
				auto it = return_value_set.begin();
				for (uint i = 0; i < return_value_set.size() - 1; ++i) {
					buffer << *it << ", ";
					++it;
				}
				buffer << *it;
			}
		}
	}

	return buffer.str();
}

const AnalysisResult VariantFunctionSpecifier::AnalyzeAssignmentTo(
		const_shared_ptr<TypeSpecifier> other,
		const TypeTable& type_table) const {
	if (*this == *other) {
		return EQUIVALENT;
	} else {
		if (FunctionVariantList::IsTerminator(m_variant_list->GetNext())) {
			//single-item variant list; test FunctionTypeSpecifier assignment
			auto as_variant_function_specifier = dynamic_pointer_cast<
					const FunctionTypeSpecifier>(other);

			if (as_variant_function_specifier) {
				auto variant = m_variant_list->GetData();
				auto declaration = variant->GetDeclaration();

				auto result =
						as_variant_function_specifier->AnalyzeAssignmentTo(
								declaration, type_table);
				return result;
			}
		}

		return INCOMPATIBLE;
	}
}

bool VariantFunctionSpecifier::operator ==(const TypeSpecifier& other) const {
	try {
		const VariantFunctionSpecifier& as_variant_function_specifier =
				dynamic_cast<const VariantFunctionSpecifier&>(other);

		auto subject = m_variant_list;
		while (!FunctionVariantList::IsTerminator(subject)) {
			auto variant = subject->GetData();
			auto declaration = variant->GetDeclaration();

			auto found = false;
			auto other_subject = as_variant_function_specifier.GetVariantList();
			while (!FunctionVariantList::IsTerminator(other_subject)) {
				auto other_variant = other_subject->GetData();
				auto other_declaration = other_variant->GetDeclaration();
				if (*declaration == *other_declaration) {
					if (!found) {
						found = true;
					} else {
						//other specifier is malformed; terminate routine
						assert(false);
						return false;
					}
				}

				other_subject = other_subject->GetNext();
			}

			if (!found) {
				return false;
			}

			subject = subject->GetNext();
		}

		return true;
	} catch (std::bad_cast& e) {
		return false;
	}
}

const_shared_ptr<Result> VariantFunctionSpecifier::GetType(
		const TypeTable& type_table, AliasResolution resolution) const {
	return make_shared<Result>(make_shared<VariantFunctionType>(m_variant_list),
			ErrorList::GetTerminator());
}

const ErrorListRef VariantFunctionSpecifier::ValidateDeclaration(
		const TypeTable& type_table, const yy::location location) const {
	auto errors = ErrorList::GetTerminator();

	auto subject = m_variant_list;
	while (!FunctionVariantList::IsTerminator(subject)) {
		auto variant = subject->GetData();
		auto declaration = variant->GetDeclaration();
		auto declaration_errors = declaration->ValidateDeclaration(type_table,
				location);
		errors = ErrorList::Concatenate(errors, declaration_errors);

		auto duplication_errors = ErrorList::GetTerminator();
		// check that this variant doesn't conflict with any subsequent definitions
		// for this, we assume the function variants are in order
		auto duplication_subject = subject->GetNext();
		while (!FunctionVariantList::IsTerminator(duplication_subject)) {
			auto duplication_subject_variant = duplication_subject->GetData();
			auto duplication_subject_declaration =
					duplication_subject_variant->GetDeclaration();
			auto assignment_result =
					duplication_subject_declaration->AnalyzeAssignmentTo(
							declaration, type_table);

			if (assignment_result == EQUIVALENT) {
				ostringstream out;
				out << declaration->GetLocation();
				duplication_errors =
						ErrorList::From(
								make_shared<Error>(Error::SEMANTIC,
										Error::FUNCTION_VARIANT_WITH_DUPLICATE_SIGNATURE,
										duplication_subject_variant->GetLocation().begin,
										declaration->ToString(), out.str()),
								duplication_errors);
			}

			duplication_subject = duplication_subject->GetNext();
		}
		errors = ErrorList::Concatenate(errors, duplication_errors);

		subject = subject->GetNext();
	}

	return errors;
}

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

#include <maybe_type_specifier.h>
#include <nested_type_specifier.h>
#include <maybe_type.h>
#include <unit_type.h>
#include <placeholder_type.h>

const_shared_ptr<std::string> MaybeTypeSpecifier::VARIANT_NAME = make_shared<
		std::string>("value");
const_shared_ptr<std::string> MaybeTypeSpecifier::EMPTY_NAME = make_shared<
		std::string>("empty");

MaybeTypeSpecifier::MaybeTypeSpecifier(
		const_shared_ptr<TypeSpecifier> base_type_specifier) :
		m_base_type_specifier(base_type_specifier), m_type(
				make_shared<MaybeType>(base_type_specifier)) {
}

MaybeTypeSpecifier::~MaybeTypeSpecifier() {
}

const std::string MaybeTypeSpecifier::ToString() const {
	return m_base_type_specifier->ToString() + "?";
}

const AnalysisResult MaybeTypeSpecifier::AnalyzeAssignmentTo(
		const_shared_ptr<TypeSpecifier> other,
		const TypeTable& type_table) const {
	if (*this == *other) {
		return AnalysisResult::EQUIVALENT;
	} else if (TypeTable::GetNilTypeSpecifier()->AnalyzeAssignmentTo(other,
			type_table) >= UNAMBIGUOUS) {
		return AnalysisResult::UNAMBIGUOUS;
	} else {
		return AnalysisResult::INCOMPATIBLE;
	}
}

bool MaybeTypeSpecifier::operator ==(const TypeSpecifier& other) const {
	try {
		const MaybeTypeSpecifier& as_maybe =
				dynamic_cast<const MaybeTypeSpecifier&>(other);
		return *GetBaseTypeSpecifier() == *as_maybe.GetBaseTypeSpecifier();
	} catch (std::bad_cast& e) {
		return false;
	}
}

const_shared_ptr<Result> MaybeTypeSpecifier::GetType(
		const TypeTable& type_table, AliasResolution resolution) const {
	return make_shared<Result>(m_type, ErrorList::GetTerminator());
}

const AnalysisResult MaybeTypeSpecifier::AnalyzeWidening(
		const TypeTable& type_table, const TypeSpecifier& other) const {
	auto resolved_result = NestedTypeSpecifier::Resolve(m_base_type_specifier,
			type_table);

	if (ErrorList::IsTerminator(resolved_result.GetErrors())) {
		auto resolved = resolved_result.GetData();
		auto base_equivalence = *resolved == other;
		auto empty_equivalence = *TypeTable::GetNilTypeSpecifier() == other;
		auto ambiguous_equivalence = *resolved
				== *TypeTable::GetNilTypeSpecifier();
		if (ambiguous_equivalence) {
			return AMBIGUOUS; //we have type "nil?"
		} else if (base_equivalence || empty_equivalence) {
			return UNAMBIGUOUS;
		}

		auto base_analysis = resolved->AnalyzeWidening(type_table, other);
		auto empty_analysis = other.AnalyzeAssignmentTo(
				TypeTable::GetNilTypeSpecifier(), type_table);
		if (base_analysis && empty_analysis) {
			return AMBIGUOUS;
		} else if (base_analysis) {
			return base_analysis;
		} else if (empty_analysis) {
			return empty_analysis;
		}
	}

	return INCOMPATIBLE;
}

const_shared_ptr<void> MaybeTypeSpecifier::DefaultValue(
		const TypeTable& type_table) const {
	return TypeTable::GetNilType()->GetDefaultValue(type_table);
}

const_shared_ptr<std::string> MaybeTypeSpecifier::MapSpecifierToVariant(
		const TypeSpecifier& type_specifier,
		const TypeTable& type_table) const {
	if (type_specifier.AnalyzeAssignmentTo(m_base_type_specifier, type_table)) {
		return VARIANT_NAME;
	} else if (type_specifier.AnalyzeAssignmentTo(
			TypeTable::GetNilTypeSpecifier(), type_table)) {
		return EMPTY_NAME;
	}
	return const_shared_ptr<std::string>();
}

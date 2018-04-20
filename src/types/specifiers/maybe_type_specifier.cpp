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
#include <primitive_type.h>
#include <unit_type.h>
#include <placeholder_type.h>
#include <sum.h>

const_shared_ptr<std::string> MaybeTypeSpecifier::VARIANT_NAME = make_shared<
		std::string>("value");

MaybeTypeSpecifier::MaybeTypeSpecifier(
		const_shared_ptr<TypeSpecifier> base_type_specifier,
		const yy::location location) :
		SumTypeSpecifier(
				make_shared<string>(base_type_specifier->ToString() + "?"),
				TypeSpecifierList::GetTerminator(), location), m_base_type_specifier(
				base_type_specifier) {
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

const AnalysisResult MaybeTypeSpecifier::AnalyzeWidening(
		const TypeTable& type_table, const TypeSpecifier& other) const {
	auto resolved_result = NestedTypeSpecifier::Resolve(m_base_type_specifier,
			type_table);

	if (ErrorList::IsTerminator(resolved_result.GetErrors())) {
		auto resolved = resolved_result.GetData();
		auto base_equivalence = *resolved == other;
		auto nil_equivalence = *TypeTable::GetNilTypeSpecifier() == other;
		auto ambiguous_equivalence = *resolved
				== *TypeTable::GetNilTypeSpecifier();
		if (ambiguous_equivalence) {
			return AMBIGUOUS; //we have type "nil?"
		} else if (base_equivalence || nil_equivalence) {
			return UNAMBIGUOUS;
		}

		auto base_analysis = resolved->AnalyzeWidening(type_table, other);
		auto nil_analysis = other.AnalyzeAssignmentTo(
				TypeTable::GetNilTypeSpecifier(), type_table);
		if (base_analysis && nil_analysis) {
			return AMBIGUOUS;
		} else if (base_analysis) {
			return base_analysis;
		} else if (nil_analysis) {
			return nil_analysis;
		}
	}

	return INCOMPATIBLE;
}

const_shared_ptr<void> MaybeTypeSpecifier::DefaultValue(
		const TypeTable& type_table,
		const_shared_ptr<type_parameter_map> type_mapping) const {
	return make_shared<Sum>(TypeTable::GetNilName(),
			TypeTable::GetNilType()->GetDefaultValue(type_table,
					ComplexType::DefaultTypeParameterMap));
}

const_shared_ptr<std::string> MaybeTypeSpecifier::MapSpecifierToVariant(
		const TypeSpecifier& type_specifier,
		const TypeTable& type_table) const {
	if (type_specifier.AnalyzeAssignmentTo(m_base_type_specifier, type_table)) {
		return VARIANT_NAME;
	} else if (type_specifier.AnalyzeAssignmentTo(
			TypeTable::GetNilTypeSpecifier(), type_table)) {
		return TypeTable::GetNilName();
	}
	return const_shared_ptr<std::string>();
}

const_shared_ptr<Result> MaybeTypeSpecifier::GetType(
		const TypeTable& type_table, AliasResolution resolution) const {
	auto base_type_result = GetBaseTypeSpecifier()->GetType(type_table,
			RESOLVE);

	shared_ptr<const void> result = nullptr;
	auto errors = base_type_result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto base_type = base_type_result->GetData<TypeDefinition>();

		auto as_complex = dynamic_pointer_cast<const ComplexType>(base_type);
		if (as_complex) {
			result = as_complex->GetMaybeType();
		}

		auto as_primitive = dynamic_pointer_cast<const PrimitiveType>(
				base_type);
		if (as_primitive) {
			auto type = as_primitive->GetType();
			switch (type) {
			case BOOLEAN: {
				static const_shared_ptr<MaybeType> boolean_maybe_type =
						MaybeType::Build(PrimitiveTypeSpecifier::GetBoolean())->GetData<
								MaybeType>();
				return make_shared<Result>(boolean_maybe_type,
						ErrorList::GetTerminator());
			}
			case BYTE: {
				static const_shared_ptr<MaybeType> byte_maybe_type =
						MaybeType::Build(PrimitiveTypeSpecifier::GetByte())->GetData<
								MaybeType>();
				return make_shared<Result>(byte_maybe_type,
						ErrorList::GetTerminator());
			}
			case INT: {
				static const_shared_ptr<MaybeType> int_maybe_type =
						MaybeType::Build(PrimitiveTypeSpecifier::GetInt())->GetData<
								MaybeType>();
				return make_shared<Result>(int_maybe_type,
						ErrorList::GetTerminator());
			}
			case DOUBLE: {
				static const_shared_ptr<MaybeType> double_maybe_type =
						MaybeType::Build(PrimitiveTypeSpecifier::GetDouble())->GetData<
								MaybeType>();
				return make_shared<Result>(double_maybe_type,
						ErrorList::GetTerminator());
			}
			case STRING:
				static const_shared_ptr<MaybeType> string_maybe_type =
						MaybeType::Build(PrimitiveTypeSpecifier::GetString())->GetData<
								MaybeType>();
				return make_shared<Result>(string_maybe_type,
						ErrorList::GetTerminator());
			default:
				return make_shared<Result>(result, errors);
			}
		}
	}
	return make_shared<Result>(result, errors);
}

const_shared_ptr<TypeSpecifier> MaybeTypeSpecifier::WithTypeArgumentList(
		TypeSpecifierListRef type_arguments) const {
	return make_shared<MaybeTypeSpecifier>(m_base_type_specifier,
			this->GetLocation());
}

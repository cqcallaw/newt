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

#include <complex_type_specifier.h>
#include <complex_type.h>
#include <nested_type_specifier.h>
#include <maybe_type_specifier.h>
#include <unit_type.h>
#include <stack>
#include <placeholder_type.h>

const_shared_ptr<ComplexTypeSpecifier> ComplexTypeSpecifier::Build(
		const_shared_ptr<ComplexTypeSpecifier> parent,
		const_shared_ptr<ComplexTypeSpecifier> child) {
	if (!parent) {
		return child;
	}

	if (!child) {
		return parent;
	}

	std::stack<shared_ptr<const std::string>> child_names;
	shared_ptr<const ComplexTypeSpecifier> start = child;
	child_names.push(start->GetTypeName());
	while (start->GetContainer()) {
		child_names.push(start->GetTypeName());
		start = start->GetContainer();
	}

	shared_ptr<const ComplexTypeSpecifier> result = parent;
	while (!child_names.empty()) {
		auto name = child_names.top();
		result = make_shared<ComplexTypeSpecifier>(name, result,
				NamespaceQualifierList::GetTerminator());
		child_names.pop();
	}

	return result;
}

const_shared_ptr<ComplexType> ComplexTypeSpecifier::GetContainerType(
		const TypeTable& type_table) const {
	auto container_specifier = GetContainer();
	if (container_specifier) {
		auto type = container_specifier->GetType(type_table);

		auto as_complex = dynamic_pointer_cast<const ComplexType>(type);
		if (as_complex) {
			return as_complex;
		}

	}

	return const_shared_ptr<ComplexType>();
}

const_shared_ptr<void> ComplexTypeSpecifier::DefaultValue(
		const TypeTable& type_table) const {
	auto type = GetType(type_table, RESOLVE);

	if (type) {
		return type->GetDefaultValue(type_table);
	}

	return nullptr;
}

bool ComplexTypeSpecifier::CompareContainers(
		const ComplexTypeSpecifier& other) const {
	auto containers = GetContainer();
	auto other_containers = other.GetContainer();
	if (!containers) {
		return !other_containers;
	}

	if (!other_containers) {
		return !containers;
	}

	return *containers == *other_containers;
}

const_shared_ptr<TypeDefinition> ComplexTypeSpecifier::GetType(
		const TypeTable& type_table, AliasResolution resolution) const {
	plain_shared_ptr<ComplexType> type = nullptr;

	auto container_type = GetContainerType(type_table);
	if (container_type) {
		type = container_type->GetDefinition()->GetType<ComplexType>(
				GetTypeName(), SHALLOW, resolution);
	} else {
		type = type_table.GetType<ComplexType>(GetTypeName(), DEEP, resolution);
	}

	return type;
}

const AnalysisResult ComplexTypeSpecifier::AnalyzeAssignmentTo(
		const_shared_ptr<TypeSpecifier> other,
		const TypeTable& type_table) const {
	auto resolved_other = NestedTypeSpecifier::Resolve(other, type_table);

	const_shared_ptr<ComplexTypeSpecifier> as_complex =
			std::dynamic_pointer_cast<const ComplexTypeSpecifier>(
					resolved_other);
	if (as_complex) {
		try {
			auto container = GetContainer();
			auto other_container = as_complex->GetContainer();
			if (container && other_container && *container == *other_container
					&& as_complex->GetTypeName()->compare(*GetTypeName())
							== 0) {
				return AnalysisResult::EQUIVALENT;
			} else if (as_complex->GetTypeName()->compare(*GetTypeName())
					== 0) {
				return AnalysisResult::EQUIVALENT;
			}

			return as_complex->AnalyzeWidening(type_table, *this);

		} catch (std::bad_cast& e) {
		}
	}

	const_shared_ptr<MaybeTypeSpecifier> as_maybe = std::dynamic_pointer_cast<
			const MaybeTypeSpecifier>(resolved_other);
	if (as_maybe) {
		if (*this == *TypeTable::GetNilTypeSpecifier()) {
			return UNAMBIGUOUS;
		}

		auto base_analysis = AnalyzeAssignmentTo(
				as_maybe->GetBaseTypeSpecifier(), type_table);
		if (base_analysis == EQUIVALENT) {
			return UNAMBIGUOUS;
		} else if (base_analysis == UNAMBIGUOUS) {
			return UNAMBIGUOUS_NESTED;
		}
	}

	return AnalysisResult::INCOMPATIBLE;
}

const std::string ComplexTypeSpecifier::ToString() const {
	return (GetContainer() ? GetContainer()->ToString() + "." : "")
			+ *m_type_name;
}

bool ComplexTypeSpecifier::operator ==(const TypeSpecifier& other) const {
	try {
		const ComplexTypeSpecifier& as_complex =
				dynamic_cast<const ComplexTypeSpecifier&>(other);
		return CompareContainers(as_complex)
				&& *GetTypeName() == *as_complex.GetTypeName();
	} catch (std::bad_cast& e) {
		return false;
	}
}

const AnalysisResult ComplexTypeSpecifier::AnalyzeWidening(
		const TypeTable& type_table, const TypeSpecifier& other) const {
	auto type = GetType(type_table);

	if (type) {
		auto as_complex = dynamic_pointer_cast<const ComplexType>(type);
		if (as_complex) {
			return as_complex->AnalyzeConversion(*this, other);
		}
	}

	return INCOMPATIBLE;
}

const ErrorListRef ComplexTypeSpecifier::ValidateDeclaration(
		const TypeTable& type_table, const yy::location position) const {
	auto existing_type = GetType(type_table);
	auto as_placeholder = dynamic_pointer_cast<const PlaceholderType>(
			existing_type);
	if (as_placeholder) {
		return ErrorList::From(
				make_shared<Error>(Error::SEMANTIC,
						Error::RAW_RECURSIVE_DECLARATION,
						position.begin.line, position.begin.column),
				ErrorList::GetTerminator());
	}

	return ErrorList::GetTerminator();
}

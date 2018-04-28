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

#include <alias_definition.h>
#include <type_table.h>
#include <primitive_type_specifier.h>
#include <array_type_specifier.h>
#include <record_type.h>
#include <function_type.h>
#include <variant_function_type.h>
#include <symbol.h>
#include <primitive_type.h>
#include <complex_type.h>

AliasDefinition::AliasDefinition(const_shared_ptr<TypeTable> origin_table,
		const_shared_ptr<TypeSpecifier> original_type_specifier,
		const AliasType alias_type, const_shared_ptr<void> default_value) :
		m_origin_table(origin_table), m_original_type_specifier(
				original_type_specifier), m_alias_type(alias_type), m_default_value(
				default_value) {
}

const_shared_ptr<void> AliasDefinition::GetDefaultValue(
		const TypeTable& type_table,
		const_shared_ptr<type_specifier_map> type_specifier_mapping) const {
	if (m_default_value)
		return m_default_value;
	else {
		auto origin = GetOriginalType(type_specifier_mapping);
		if (origin && m_alias_type == DIRECT) {
			auto remapping = Remap(type_specifier_mapping, m_original_type_specifier,
					origin, type_table);
			return origin->GetDefaultValue(type_table, remapping);
		}

		// default, un-desired behavior
		return const_shared_ptr<void>();
	}
}

const std::string AliasDefinition::ValueToString(const TypeTable& type_table,
		const Indent& indent, const_shared_ptr<void> value,
		const_shared_ptr<type_specifier_map> type_specifier_mapping) const {
	auto origin = GetOriginalType(type_specifier_mapping);
	if (origin && m_alias_type == DIRECT) {
		auto remapping = Remap(type_specifier_mapping, m_original_type_specifier, origin,
				type_table);
		return origin->ValueToString(type_table, indent, value, remapping);
	} else {
		return "<No origin found for alias '"
				+ m_original_type_specifier->ToString() + "'>";
	}
}

const std::string AliasDefinition::GetValueSeparator(const Indent& indent,
		const void* value,
		const_shared_ptr<type_specifier_map> type_specifier_mapping) const {
	auto origin = GetOriginalType(type_specifier_mapping);
	if (origin) {
		return origin->GetValueSeparator(indent, value, type_specifier_mapping);
	} else {
		return "<No origin found for alias '"
				+ m_original_type_specifier->ToString() + "'>";
	}
}

const std::string AliasDefinition::GetTagSeparator(const Indent& indent,
		const void* value,
		const_shared_ptr<type_specifier_map> type_specifier_mapping) const {
	auto origin = GetOriginalType(type_specifier_mapping);
	if (origin) {
		return origin->GetTagSeparator(indent, value, type_specifier_mapping);
	} else {
		return "<No origin found for alias '"
				+ m_original_type_specifier->ToString() + "'>";
	}
}

const_shared_ptr<TypeSpecifier> AliasDefinition::GetTypeSpecifier(
		const_shared_ptr<std::string> name,
		const_shared_ptr<ComplexTypeSpecifier> container,
		const_shared_ptr<type_specifier_map> type_specifier_mapping,
		yy::location location) const {
	if (type_specifier_mapping) {
		auto find_result = type_specifier_mapping->find(
				m_original_type_specifier->ToString());
		if (find_result != type_specifier_mapping->end()) {
			// direct, un-parameterized mapping
			return find_result->second;
		}

		auto new_type_arguments = ComplexType::TypeParameterSubstitution(
				m_original_type_specifier->GetTypeArgumentList(), type_specifier_mapping);
		auto new_type_specifier =
				m_original_type_specifier->WithTypeArgumentList(
						new_type_arguments);
		return new_type_specifier;
	} else {
		return m_original_type_specifier;
	}
}

const_shared_ptr<TypeDefinition> AliasDefinition::GetOriginalType(
		const_shared_ptr<type_specifier_map> type_specifier_mapping) const {
	const_shared_ptr<TypeTable> ptr = m_origin_table.lock();
	assert(ptr);

	auto type_specifier = m_original_type_specifier;

	// handle type parameter mapping if it's applicable
	auto original_as_string = m_original_type_specifier->ToString();
	if (type_specifier_mapping && type_specifier_mapping != ComplexType::DefaultTypeSpecifierMap) {
		for (auto const &type_map : *type_specifier_mapping) {
			auto const type_parameter_name = type_map.first;
			if (original_as_string == type_parameter_name) {
				type_specifier = type_map.second;
			}
		}
	}

	auto type_result = type_specifier->GetType(*ptr);
	if (ErrorList::IsTerminator(type_result->GetErrors())) {
		return type_result->GetData<TypeDefinition>();
	}

	return nullptr;
}

const_shared_ptr<Symbol> AliasDefinition::GetSymbol(const TypeTable& type_table,
		const_shared_ptr<TypeSpecifier> type_specifier,
		const_shared_ptr<void> value,
		const_shared_ptr<type_specifier_map> type_specifier_mapping) const {
	auto origin = GetOriginalType(type_specifier_mapping);
	if (origin) {
		return origin->GetSymbol(type_table, type_specifier, value,
				type_specifier_mapping);
	} else {
		return Symbol::GetDefaultSymbol();
	}
}

const std::string AliasDefinition::ToString(const TypeTable& type_table,
		const Indent& indent,
		const_shared_ptr<type_specifier_map> type_specifier_mapping) const {
	ostringstream os;
	Indent child_indent = indent + 1;
	os << child_indent;
	os << m_original_type_specifier->ToString();

	if (m_default_value) {
		os << " (";

		auto origin = GetOriginalType(type_specifier_mapping);
		auto as_record = dynamic_pointer_cast<const RecordType>(origin);
		if (as_record) {
			os
					<< GetValueSeparator(child_indent, m_default_value.get(),
							type_specifier_mapping);
		}

		auto as_function = dynamic_pointer_cast<const FunctionType>(origin);
		if (as_function) {
			os
					<< GetValueSeparator(child_indent, m_default_value.get(),
							type_specifier_mapping);
		}

		auto as_variant_function = dynamic_pointer_cast<
				const VariantFunctionType>(origin);
		if (as_variant_function) {
			os
					<< GetValueSeparator(child_indent, m_default_value.get(),
							type_specifier_mapping);
		}

		os
				<< ValueToString(type_table, child_indent, m_default_value,
						type_specifier_mapping);

		if (as_record) {
			os << child_indent;
		}

		if (as_function || as_variant_function) {
			os << endl;
			os << child_indent;
		}
		os << ")";
	}

	return os.str();
}

const_shared_ptr<DeclarationStatement> AliasDefinition::GetDeclarationStatement(
		const yy::location position, const_shared_ptr<TypeSpecifier> type,
		const yy::location type_position, const_shared_ptr<std::string> name,
		const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) const {
	auto origin = GetOriginalType(ComplexType::DefaultTypeSpecifierMap);
	if (origin) {
		return origin->GetDeclarationStatement(position, type, type_position,
				name, name_position, initializer_expression);
	} else {
		assert(false);
		return nullptr;
	}
}

const_shared_ptr<type_specifier_map> AliasDefinition::Remap(
		const_shared_ptr<type_specifier_map> type_specifier_mapping,
		const_shared_ptr<TypeSpecifier> type_specifier,
		const_shared_ptr<TypeDefinition> type, const TypeTable& type_table) {
	//			auto new_type_specifier = m_original_type_specifier;
	//			if (!TypeSpecifierList::IsTerminator(
	//					m_original_type_specifier->GetTypeArgumentList())) {
	//				new_type_specifier =
	//						m_original_type_specifier->WithTypeArgumentList(
	//								ComplexType::TypeParameterSubstitution(
	//										m_original_type_specifier->GetTypeArgumentList(),
	//										type_specifier_mapping));
	//			}
	//
	//			auto remapping_result = ComplexType::GetTypeParameterMap(
	//					origin->GetTypeParameterList(),
	//					new_type_specifier->GetTypeArgumentList(), type_table);
	//			if (ErrorList::IsTerminator(remapping_result.GetErrors())) {
	//				return origin->GetDefaultValue(type_table,
	//						remapping_result.GetData());
	//			}

	if (TypeSpecifierList::IsTerminator(
			type_specifier->GetTypeArgumentList())) {
		return type_specifier_mapping;
	}

	auto new_type_specifier = type_specifier;
	new_type_specifier = type_specifier->WithTypeArgumentList(
			ComplexType::TypeParameterSubstitution(
					type_specifier->GetTypeArgumentList(), type_specifier_mapping));

	auto remapping_result = ComplexType::GetTypeParameterMap(
			type->GetTypeParameterList(),
			new_type_specifier->GetTypeArgumentList(), type_table);
	if (ErrorList::IsTerminator(remapping_result.GetErrors())) {
		return remapping_result.GetData();
	} else {
		return ComplexType::DefaultTypeSpecifierMap;
	}
}

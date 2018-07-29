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

#ifndef SPECIFIERS_TYPE_SPECIFIER_H_
#define SPECIFIERS_TYPE_SPECIFIER_H_

#include <map>
#include <defaults.h>
#include <string>
#include <alias_resolution.h>
#include <analysis_result.h>
#include <result.h>

class Expression;
class DeclarationStatement;
class TypeTable;
class Symbol;

typedef const LinkedList<const TypeSpecifier, ALLOW_DUPLICATES> TypeSpecifierList;
typedef shared_ptr<const TypeSpecifierList> TypeSpecifierListRef;

// a map from string identifiers to type specifiers
// this map is not the same as a type table of type alias, because type alias include a reference to the type table in which the aliased type is defined
typedef map<const string, const_shared_ptr<TypeSpecifier>> type_specifier_map;

class TypeSpecifier {
public:
	TypeSpecifier(const yy::location location = GetDefaultLocation()) :
			m_location(location) {
	}

	virtual ~TypeSpecifier() {
	}

	virtual const std::string ToString() const = 0;

	/**
	 * Analyze assignment of this type specifier to another type specifier
	 *
	 * @other The other type specifier
	 * @type_table The type table hierarchy in which the other type is defined
	 * @type_specifier_mapping A mapping of type parameters to type arguments for the other type (TODO: determine if this for the source or target type)
	 */
	virtual const AnalysisResult AnalyzeAssignmentTo(
			const_shared_ptr<TypeSpecifier> other, const TypeTable& type_table,
			const_shared_ptr<type_specifier_map> type_specifier_mapping) const = 0;

	/**
	 * Validate a declaration
	 *
	 * @type_table The type table for the declaration's context
	 * @type_parameter_list A list of type parameters that may legally be referenced by the declaration
	 * @position The position of the declaration, for error reporting purposes
	 */
	virtual const ErrorListRef ValidateDeclaration(const TypeTable& type_table,
			const TypeSpecifierListRef type_parameter_list,
			const yy::location position) const {
		return ErrorList::GetTerminator();
	}

	virtual bool operator==(const TypeSpecifier &other) const = 0;

	virtual bool operator!=(const TypeSpecifier &other) const {
		return !(*this == other);
	}

	virtual const_shared_ptr<Result> GetType(const TypeTable& type_table,
			AliasResolution resolution = AliasResolution::RESOLVE) const = 0;

	virtual const AnalysisResult AnalyzeWidening(const TypeTable& type_table,
			const_shared_ptr<type_specifier_map> type_specifier_mapping,
			const TypeSpecifier& other) const {
		return INCOMPATIBLE;
	}

	virtual const_shared_ptr<void> DefaultValue(
			const TypeTable& type_table) const;

	virtual const TypeSpecifierListRef GetTypeArgumentList() const {
		return TypeSpecifierList::GetTerminator();
	}

	virtual const_shared_ptr<TypeSpecifier> WithTypeArgumentList(
			TypeSpecifierListRef type_arguments) const = 0;

	const yy::location GetLocation() const {
		return m_location;
	}

	/*static TypedResult<TypeTable> GetTypeParameterMap(
	 const TypeSpecifierListRef type_parameters,
	 const TypeSpecifierListRef type_arguments,
	 const shared_ptr<TypeTable> type_table);*/

	static const_shared_ptr<type_specifier_map> DefaultTypeSpecifierMap;

private:
	const yy::location m_location;
};

#endif /* SPECIFIERS_TYPE_SPECIFIER_H_ */

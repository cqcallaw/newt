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

#ifndef SPECIFIERS_COMPLEX_TYPE_SPECIFIER_H_
#define SPECIFIERS_COMPLEX_TYPE_SPECIFIER_H_

#include <specifiers/type_specifier.h>
#include <namespace_qualifier.h>

class ComplexType;

class ComplexTypeSpecifier: public TypeSpecifier {
public:
	ComplexTypeSpecifier(const_shared_ptr<std::string> type_name,
			const TypeSpecifierListRef type_argument_list,
			const yy::location location = GetDefaultLocation()) :
			ComplexTypeSpecifier(type_name, type_argument_list, nullptr,
					NamespaceQualifierList::GetTerminator(), location) {
	}

	ComplexTypeSpecifier(const_shared_ptr<std::string> type_name,
			const TypeSpecifierListRef type_argument_list,
			const_shared_ptr<ComplexTypeSpecifier> container,
			const NamespaceQualifierListRef space, const yy::location location =
					GetDefaultLocation()) :
			TypeSpecifier(location), m_type_name(type_name), m_type_argument_list(
					type_argument_list), m_container(container), m_space(space) {
	}

	~ComplexTypeSpecifier() {
	}

	static const_shared_ptr<ComplexTypeSpecifier> Build(
			const_shared_ptr<ComplexTypeSpecifier> parent,
			const_shared_ptr<ComplexTypeSpecifier> child);

	virtual const_shared_ptr<Result> GetType(const TypeTable& type_table,
			AliasResolution resolution = AliasResolution::RESOLVE) const;

	virtual const AnalysisResult AnalyzeAssignmentTo(
			const_shared_ptr<TypeSpecifier> other,
			const TypeTable& type_table) const;

	virtual const ErrorListRef ValidateDeclaration(const TypeTable& type_table,
			const TypeSpecifierListRef type_parameter_list,
			const yy::location position) const;

	const_shared_ptr<ComplexType> GetContainerType(
			const TypeTable& type_table) const;

	virtual const_shared_ptr<void> DefaultValue(
			const TypeTable& type_table) const;

	virtual const std::string ToString() const;

	virtual const AnalysisResult AnalyzeWidening(const TypeTable& type_table,
			const TypeSpecifier& other) const;

	virtual bool operator==(const TypeSpecifier& other) const;

	bool CompareContainers(const ComplexTypeSpecifier& other) const;

	virtual const_shared_ptr<std::string> GetTypeName() const {
		return m_type_name;
	}

	virtual const TypeSpecifierListRef GetTypeArgumentList() const {
		return m_type_argument_list;
	}

	virtual const_shared_ptr<TypeSpecifier> WithTypeArgumentList(
			TypeSpecifierListRef type_arguments) const;

	virtual const NamespaceQualifierListRef GetNamespace() const {
		return m_space;
	}

	virtual const_shared_ptr<ComplexTypeSpecifier> GetContainer() const {
		return m_container;
	}

private:
	const_shared_ptr<std::string> m_type_name;
	const TypeSpecifierListRef m_type_argument_list;
	const_shared_ptr<ComplexTypeSpecifier> m_container;
	const NamespaceQualifierListRef m_space;
};

#endif /* SPECIFIERS_COMPLEX_TYPE_SPECIFIER_H_ */

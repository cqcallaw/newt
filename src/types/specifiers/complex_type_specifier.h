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
	virtual const_shared_ptr<std::string> GetTypeName() const = 0;
	virtual const NamespaceQualifierListRef GetNamespace() const = 0;
	virtual const_shared_ptr<ComplexTypeSpecifier> GetContainer() const = 0;

	const_shared_ptr<ComplexType> GetContainerType(
			const TypeTable& type_table) const;

	virtual const_shared_ptr<void> DefaultValue(
			const TypeTable& type_table) const;

	bool CompareContainers(const ComplexTypeSpecifier& other) const;

	static const TypeSpecifier& ResolveAliasing(
			const ComplexTypeSpecifier& original, const TypeTable& type_table);

};

#endif /* SPECIFIERS_COMPLEX_TYPE_SPECIFIER_H_ */

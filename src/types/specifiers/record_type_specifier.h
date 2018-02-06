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

#ifndef SPECIFIERS_RECORD_TYPE_SPECIFIER_H_
#define SPECIFIERS_RECORD_TYPE_SPECIFIER_H_

#include <namespace_qualifier.h>
#include <specifiers/complex_type_specifier.h>
#include <type_table.h>

class Expression;
class Statement;
class SymbolContext;

class RecordTypeSpecifier: public ComplexTypeSpecifier {
public:
	RecordTypeSpecifier(const_shared_ptr<std::string> type_name,
			const TypeSpecifierListRef type_argument_list,
			const yy::location location = GetDefaultLocation()) :
			RecordTypeSpecifier(type_name, type_argument_list, nullptr,
					NamespaceQualifierList::GetTerminator(), location) {
	}

	RecordTypeSpecifier(const_shared_ptr<std::string> type_name,
			const TypeSpecifierListRef type_argument_list,
			const_shared_ptr<ComplexTypeSpecifier> container,
			const yy::location location = GetDefaultLocation()) :
			RecordTypeSpecifier(type_name, type_argument_list, container,
					NamespaceQualifierList::GetTerminator(), location) {
	}

	RecordTypeSpecifier(const_shared_ptr<std::string> type_name,
			const TypeSpecifierListRef type_argument_list,
			const NamespaceQualifierListRef space, const yy::location location =
					GetDefaultLocation()) :
			RecordTypeSpecifier(type_name, type_argument_list, nullptr, space,
					location) {
	}

	RecordTypeSpecifier(const_shared_ptr<std::string> type_name,
			const TypeSpecifierListRef type_argument_list,
			const_shared_ptr<ComplexTypeSpecifier> container,
			const NamespaceQualifierListRef space, const yy::location location =
					GetDefaultLocation()) :
			ComplexTypeSpecifier(type_name, type_argument_list, container,
					space, location) {
	}

	virtual ~RecordTypeSpecifier() {
	}
};

#endif /* SPECIFIERS_RECORD_TYPE_SPECIFIER_H_ */

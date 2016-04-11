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
	RecordTypeSpecifier(const_shared_ptr<std::string> type_name) :
			RecordTypeSpecifier(type_name, nullptr,
					NamespaceQualifierList::GetTerminator()) {
	}

	RecordTypeSpecifier(const_shared_ptr<std::string> type_name,
			const_shared_ptr<ComplexTypeSpecifier> container) :
			RecordTypeSpecifier(type_name, container,
					NamespaceQualifierList::GetTerminator()) {
	}

	RecordTypeSpecifier(const_shared_ptr<std::string> type_name,
			const NamespaceQualifierListRef space) :
			RecordTypeSpecifier(type_name, nullptr, space) {
	}

	RecordTypeSpecifier(const_shared_ptr<std::string> type_name,
			const_shared_ptr<ComplexTypeSpecifier> container,
			const NamespaceQualifierListRef space) :
			ComplexTypeSpecifier(type_name, container, space) {
	}

	virtual ~RecordTypeSpecifier() {
	}
};

#endif /* SPECIFIERS_RECORD_TYPE_SPECIFIER_H_ */

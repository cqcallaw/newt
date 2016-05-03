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

#ifndef SPECIFIERS_SUM_TYPE_SPECIFIER_H_
#define SPECIFIERS_SUM_TYPE_SPECIFIER_H_

#include <complex_type_specifier.h>

class RecordTypeSpecifier;

class SumTypeSpecifier: public ComplexTypeSpecifier {
public:
	SumTypeSpecifier(const_shared_ptr<std::string> type_name) :
			SumTypeSpecifier(type_name, nullptr,
					NamespaceQualifierList::GetTerminator()) {
	}

	SumTypeSpecifier(const_shared_ptr<std::string> type_name,
			const_shared_ptr<ComplexTypeSpecifier> container) :
			SumTypeSpecifier(type_name, container,
					NamespaceQualifierList::GetTerminator()) {
	}

	SumTypeSpecifier(const_shared_ptr<std::string> type_name,
			const NamespaceQualifierListRef space) :
			SumTypeSpecifier(type_name, nullptr, space) {
	}

	SumTypeSpecifier(const_shared_ptr<std::string> type_name,
			const_shared_ptr<ComplexTypeSpecifier> container,
			const NamespaceQualifierListRef space) :
			ComplexTypeSpecifier(type_name, container, space) {
	}

	SumTypeSpecifier(const ComplexTypeSpecifier& complex);
	SumTypeSpecifier(const_shared_ptr<ComplexTypeSpecifier> complex);

	virtual ~SumTypeSpecifier() {
	}

	virtual const AnalysisResult IsAssignableTo(
			const_shared_ptr<TypeSpecifier> other,
			const TypeTable& type_table) const;
};

#endif /* SPECIFIERS_SUM_TYPE_SPECIFIER_H_ */

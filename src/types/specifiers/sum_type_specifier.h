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
	SumTypeSpecifier(const_shared_ptr<std::string> type_name,
			const TypeSpecifierListRef type_argument_list,
			const yy::location location) :
			SumTypeSpecifier(type_name, type_argument_list, nullptr,
					NamespaceQualifierList::GetTerminator(), location) {
	}

	SumTypeSpecifier(const_shared_ptr<std::string> type_name,
			const TypeSpecifierListRef type_argument_list,
			const_shared_ptr<ComplexTypeSpecifier> container,
			const yy::location location = GetDefaultLocation()) :
			SumTypeSpecifier(type_name, type_argument_list, container,
					NamespaceQualifierList::GetTerminator(), location) {
	}

	SumTypeSpecifier(const_shared_ptr<std::string> type_name,
			const TypeSpecifierListRef type_argument_list,
			const NamespaceQualifierListRef space, const yy::location location =
					GetDefaultLocation()) :
			SumTypeSpecifier(type_name, type_argument_list, nullptr, space,
					location) {
	}

	SumTypeSpecifier(const_shared_ptr<std::string> type_name,
			const TypeSpecifierListRef type_argument_list,
			const_shared_ptr<ComplexTypeSpecifier> container,
			const NamespaceQualifierListRef space, const yy::location location =
					GetDefaultLocation()) :
			ComplexTypeSpecifier(type_name, type_argument_list, container,
					space, location) {
	}

	SumTypeSpecifier(const ComplexTypeSpecifier& complex);
	SumTypeSpecifier(const_shared_ptr<ComplexTypeSpecifier> complex);

	virtual ~SumTypeSpecifier() {
	}

	virtual const AnalysisResult AnalyzeAssignmentTo(
			const_shared_ptr<TypeSpecifier> other,
			const TypeTable& type_table) const;
};

#endif /* SPECIFIERS_SUM_TYPE_SPECIFIER_H_ */

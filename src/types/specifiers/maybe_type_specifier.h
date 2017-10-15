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

#ifndef TYPES_SPECIFIERS_MAYBE_TYPE_SPECIFIER_H_ORIG_
#define TYPES_SPECIFIERS_MAYBE_TYPE_SPECIFIER_H_ORIG_

#include <sum_type_specifier.h>

class MaybeType;

class MaybeTypeSpecifier: public SumTypeSpecifier {
public:
	MaybeTypeSpecifier(const_shared_ptr<TypeSpecifier> base_type_specifier,
			const yy::location location);
	virtual ~MaybeTypeSpecifier();

	virtual const std::string ToString() const;
	virtual const AnalysisResult AnalyzeAssignmentTo(
			const_shared_ptr<TypeSpecifier> other,
			const TypeTable& type_table) const;

	virtual bool operator==(const TypeSpecifier &other) const;

	virtual const AnalysisResult AnalyzeWidening(const TypeTable& type_table,
			const TypeSpecifier& other) const;

	const_shared_ptr<void> DefaultValue(const TypeTable& type_table) const;

	const_shared_ptr<TypeSpecifier> GetBaseTypeSpecifier() const {
		return m_base_type_specifier;
	}

	virtual const_shared_ptr<Result> GetType(const TypeTable& type_table,
			AliasResolution resolution = AliasResolution::RESOLVE) const;

	const_shared_ptr<std::string> MapSpecifierToVariant(
			const TypeSpecifier& type_specifier,
			const TypeTable& type_table) const;

	static const_shared_ptr<std::string> VARIANT_NAME;

private:
	const_shared_ptr<TypeSpecifier> m_base_type_specifier;
};

#endif /* TYPES_SPECIFIERS_MAYBE_TYPE_SPECIFIER_H_ORIG_ */

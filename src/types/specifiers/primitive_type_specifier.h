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

#ifndef SPECIFIERS_PRIMITIVE_TYPE_SPECIFIER_H_
#define SPECIFIERS_PRIMITIVE_TYPE_SPECIFIER_H_

#include <assert.h>
#include <sstream>
#include <defaults.h>
#include <specifiers/type_specifier.h>
#include <type.h>

class Expression;
class Statement;

class PrimitiveTypeSpecifier: public TypeSpecifier {
public:
	PrimitiveTypeSpecifier(const BasicType basic_type,
			const yy::location location) :
			TypeSpecifier(location), m_basic_type(basic_type) {
	}

	virtual ~PrimitiveTypeSpecifier() {
	}

	virtual const string ToString() const {
		return TypeToString(m_basic_type);
	}

	const string ToString(const_shared_ptr<void> value) const;

	virtual const AnalysisResult AnalyzeAssignmentTo(
			const_shared_ptr<TypeSpecifier> other, const TypeTable& type_table,
			const_shared_ptr<type_specifier_map> type_specifier_mapping) const;

	virtual bool operator==(const TypeSpecifier &other) const;

	virtual const_shared_ptr<Result> GetType(const TypeTable& type_table,
			AliasResolution resolution = AliasResolution::RESOLVE) const;

	const BasicType GetBasicType() const {
		return m_basic_type;
	}

	virtual const_shared_ptr<TypeSpecifier> WithTypeArgumentList(
			TypeSpecifierListRef type_arguments) const;

	static const_shared_ptr<PrimitiveTypeSpecifier> FromBasicType(
			BasicType type);

	static const_shared_ptr<PrimitiveTypeSpecifier> GetNone();

	static const_shared_ptr<PrimitiveTypeSpecifier> GetBoolean();

	static const_shared_ptr<PrimitiveTypeSpecifier> GetInt();

	static const_shared_ptr<PrimitiveTypeSpecifier> GetDouble();

	static const_shared_ptr<PrimitiveTypeSpecifier> GetString();

	static const_shared_ptr<PrimitiveTypeSpecifier> GetByte();

private:
	PrimitiveTypeSpecifier(const BasicType basic_type) :
			m_basic_type(basic_type) {
	}

	const BasicType m_basic_type;
};

#endif /* SPECIFIERS_PRIMITIVE_TYPE_SPECIFIER_H_ */

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

#ifndef FUNCTION_VARIANT_H_
#define FUNCTION_VARIANT_H_

#include <defaults.h>
#include <linked_list.h>

class FunctionDeclaration;
class StatementBlock;
class Indent;

class FunctionVariant {
public:
	FunctionVariant(const yy::location location,
			const_shared_ptr<FunctionDeclaration> declaration,
			const_shared_ptr<StatementBlock> body);
	virtual ~FunctionVariant();

	const yy::location GetLocation() const {
		return m_location;
	}

	const_shared_ptr<StatementBlock> GetBody() const {
		return m_body;
	}

	const_shared_ptr<FunctionDeclaration> GetDeclaration() const {
		return m_declaration;
	}

	const string ToString(const Indent &indent) const;

private:
	const yy::location m_location;
	const_shared_ptr<FunctionDeclaration> m_declaration;
	const_shared_ptr<StatementBlock> m_body;
};

typedef const LinkedList<const FunctionVariant, NO_DUPLICATES> FunctionVariantList;
typedef std::shared_ptr<const FunctionVariantList> FunctionVariantListRef;

#endif /* FUNCTION_VARIANT_H_ */

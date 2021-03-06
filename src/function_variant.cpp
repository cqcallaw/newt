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

#include <function_variant.h>
#include <sstream>
#include <function_declaration.h>
#include <indent.h>
#include <execution_context.h>

FunctionVariant::FunctionVariant(const yy::location location,
		const_shared_ptr<FunctionDeclaration> declaration,
		const_shared_ptr<StatementBlock> body) :
		m_location(location), m_declaration(declaration), m_body(body), m_context(
				make_shared<ExecutionContext>(Modifier::Type::MUTABLE)) {
}

FunctionVariant::~FunctionVariant() {
}

const string FunctionVariant::ToString(const Indent &indent) const {
	ostringstream buffer;
	buffer << indent << GetDeclaration()->ToString() << ":" << endl;
	buffer << indent + 1 << "Location: ";
	if (GetLocation() != GetDefaultLocation()) {
		buffer << GetLocation();
	} else {
		buffer << "[default location]";
	}

	return buffer.str();
}

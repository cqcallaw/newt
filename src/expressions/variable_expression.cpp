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

#include <variable_expression.h>
#include <variable.h>
#include <array_variable.h>
#include <error.h>
#include <sstream>

VariableExpression::VariableExpression(const Variable* variable) :
		Expression(variable->GetType()), m_variable(variable) {
}

const void* VariableExpression::Evaluate() const {
	const Symbol* symbol = SymbolTable::instance()->GetSymbol(
			m_variable->GetName());
	switch (symbol->GetType()) {
	case NONE:
		return NULL;
	case INT_ARRAY:
	case DOUBLE_ARRAY:
	case STRING_ARRAY: {
		ArraySymbol* as_array_symbol = (ArraySymbol*) symbol;

		ArrayVariable* as_array_variable = (ArrayVariable*) m_variable;
		const Expression* array_index_expression =
				as_array_variable->GetIndexExpression();

		int index = *((int *) array_index_expression->Evaluate());

		if (index > as_array_symbol->GetSize()) {
			ostringstream buffer;
			buffer << index;
			Error::error(Error::ARRAY_INDEX_OUT_OF_BOUNDS,
					*(m_variable->GetName()), buffer.str());
			return NULL;
		} else {
			return (void *) as_array_symbol->GetValue(index);
		}
		break;
	}

	case INT:
	case DOUBLE:
	case STRING:
	default: {
		return symbol->GetValue();
	}
	}
}

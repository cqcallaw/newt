/*
 * variable_expression.cpp
 *
 *  Created on: Apr 18, 2015
 *      Author: caleb
 */

#include <variable_expression.h>
#include <variable.h>
#include <error.h>
#include <sstream>

VariableExpression::VariableExpression(const Variable* variable) :
		Expression(variable->GetType()), m_variable(variable) {
}

const void* VariableExpression::Evaluate() const {
	const Symbol* symbol = Symbol_table::instance()->GetSymbol(
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

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

#include <sstream>
#include <defaults.h>
#include <execution_context.h>

#include "variable.h"
#include "assert.h"
#include "expression.h"

Variable::Variable(const string* name, YYLTYPE location) :
		m_name(name), m_location(location) {
	assert(name != NULL && name != nullptr);
}

const string* Variable::ToString() const {
	ostringstream buffer;
	buffer << "<" << *m_name << ">";
	return new string(buffer.str());
}

std::ostream &operator<<(std::ostream &os, const Variable &variable) {
	os << *(variable.ToString());
	return os;
}

Variable::~Variable() {
}

const TypeSpecifier* Variable::GetType(const ExecutionContext* context) const {
	auto symbol = context->GetSymbolContext()->GetSymbol(m_name);
	return symbol->GetType();
}

const Result* Variable::Evaluate(const ExecutionContext* context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	const SymbolContext* symbol_context = context->GetSymbolContext();
	const Symbol* symbol = symbol_context->GetSymbol(GetName());
	const Symbol* result_symbol = Symbol::DefaultSymbol;

	if (symbol == nullptr || symbol == Symbol::DefaultSymbol) {
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						GetLocation().first_line, GetLocation().first_column,
						*(GetName())));
	} else {
		result_symbol = symbol;
	}

	const Result* result = new Result(result_symbol->GetValue(), errors);
	return result;
}

const LinkedList<const Error*>* Variable::SetSymbol(
		const ExecutionContext* context, const bool* value) const {
	return ToErrorList(
			context->GetSymbolContext()->SetSymbol(*GetName(), value));
}

const LinkedList<const Error*>* Variable::SetSymbol(
		const ExecutionContext* context, const int* value) const {
	return ToErrorList(
			context->GetSymbolContext()->SetSymbol(*GetName(), value));
}

const LinkedList<const Error*>* Variable::SetSymbol(
		const ExecutionContext* context, const double* value) const {
	return ToErrorList(
			context->GetSymbolContext()->SetSymbol(*GetName(), value));
}

const LinkedList<const Error*>* Variable::SetSymbol(
		const ExecutionContext* context, const string* value) const {
	return ToErrorList(
			context->GetSymbolContext()->SetSymbol(*GetName(), value));
}

const LinkedList<const Error*>* Variable::SetSymbol(
		const ExecutionContext* context,
		const CompoundTypeInstance* value) const {
	return ToErrorList(
			context->GetSymbolContext()->SetSymbol(*GetName(), value));
}

const LinkedList<const Error*>* Variable::ToErrorList(SetResult result) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	switch (result) {
	case NO_SET_RESULT:
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::DEFAULT_ERROR_CODE,
						GetLocation().first_line, GetLocation().first_column,
						*(GetName())));
		break;
	case UNDEFINED_SYMBOL:
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						GetLocation().first_line, GetLocation().first_column,
						*(GetName())));
		break;
	case INCOMPATIBLE_TYPE:
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::ASSIGNMENT_TYPE_ERROR,
						GetLocation().first_line, GetLocation().first_column,
						*(GetName())));
		break;
	case MUTATION_DISALLOWED:
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::READONLY,
						GetLocation().first_line, GetLocation().first_column,
						*(GetName())));
		break;
	case SET_SUCCESS:
	default:
		break;
	}

	return errors;
}

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

#include <basic_variable.h>
#include <sstream>
#include <defaults.h>
#include <execution_context.h>

#include "assert.h"
#include "expression.h"

BasicVariable::BasicVariable(const string* name, const YYLTYPE location) :
		Variable(name, location) {
}

BasicVariable::~BasicVariable() {
}

const string* BasicVariable::ToString(const ExecutionContext* context) const {
	ostringstream buffer;
	buffer << "<" << *GetName() << ">";
	return new string(buffer.str());
}

const TypeSpecifier* BasicVariable::GetType(
		const ExecutionContext* context) const {
	auto symbol = context->GetSymbolContext()->GetSymbol(GetName());
	return symbol->GetType();
}

const Result* BasicVariable::Evaluate(const ExecutionContext* context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	const SymbolContext* symbol_context = context->GetSymbolContext();
	const Symbol* symbol = symbol_context->GetSymbol(GetName());
	const Symbol* result_symbol = Symbol::DefaultSymbol;

	if (symbol != nullptr && symbol != Symbol::DefaultSymbol) {
		result_symbol = symbol;
	} else {
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						GetLocation().first_line, GetLocation().first_column,
						*(GetName())));
	}

	const Result* result = new Result(result_symbol->GetValue(), errors);
	return result;
}

const LinkedList<const Error*>* BasicVariable::SetSymbol(
		const ExecutionContext* context, const bool* value) const {
	return ToErrorList(
			context->GetSymbolContext()->SetSymbol(*GetName(), value));
}

const LinkedList<const Error*>* BasicVariable::SetSymbol(
		const ExecutionContext* context, const int* value) const {
	return ToErrorList(
			context->GetSymbolContext()->SetSymbol(*GetName(), value));
}

const LinkedList<const Error*>* BasicVariable::SetSymbol(
		const ExecutionContext* context, const double* value) const {
	return ToErrorList(
			context->GetSymbolContext()->SetSymbol(*GetName(), value));
}

const LinkedList<const Error*>* BasicVariable::SetSymbol(
		const ExecutionContext* context, const string* value) const {
	return ToErrorList(
			context->GetSymbolContext()->SetSymbol(*GetName(), value));
}

const Variable* BasicVariable::GetDefaultVariable() {
	const static string* name = new string("!!!!!DefaultVariable!!!!!");
	const static Variable* instance = new BasicVariable(name, DefaultLocation);

	return instance;
}

const LinkedList<const Error*>* BasicVariable::SetSymbol(
		const ExecutionContext* context,
		const CompoundTypeInstance* value) const {
	return ToErrorList(
			context->GetSymbolContext()->SetSymbol(*GetName(), value));
}

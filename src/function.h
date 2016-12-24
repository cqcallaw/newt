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

#ifndef FUNCTION_H_
#define FUNCTION_H_

#include <expression.h>
#include <function_variant.h>

class FunctionDeclaration;
class StatementBlock;
class Result;
class ExecutionContext;

class Function {
public:
	static const_shared_ptr<Function> Build(const yy::location location,
			const_shared_ptr<FunctionDeclaration> declaration,
			const_shared_ptr<StatementBlock> statement_block,
			const shared_ptr<ExecutionContext> closure);

	static const_shared_ptr<Function> Build(const yy::location location,
			const_shared_ptr<FunctionDeclaration> declaration,
			const_shared_ptr<StatementBlock> statement_block,
			const weak_ptr<ExecutionContext> closure);

	static const_shared_ptr<Function> Build(const yy::location location,
			FunctionVariantListRef variant_list,
			const shared_ptr<ExecutionContext> closure);

	static const_shared_ptr<Function> Build(const yy::location location,
			FunctionVariantListRef variant_list,
			const weak_ptr<ExecutionContext> closure);

	virtual ~Function();

	const_shared_ptr<Result> Evaluate(ArgumentListRef argument_list,
			const yy::location argument_list_location,
			const shared_ptr<ExecutionContext> invocation_context) const;

	const string ToString(const TypeTable& type_table,
			const Indent& indent) const;

	const FunctionVariantListRef GetVariantList() const {
		return m_variant_list;
	}

	const yy::location GetLocation() const {
		return m_location;
	}

	const_shared_ptr<TypeSpecifier> GetTypeSpecifier() const;

	static const TypedResult<FunctionVariant> GetVariant(
			const ArgumentListRef argument_list,
			const yy::location argument_list_location,
			const FunctionVariantListRef variant_list,
			const shared_ptr<ExecutionContext>);

private:
	Function(const yy::location location,
			const FunctionVariantListRef m_variant_list,
			const shared_ptr<ExecutionContext> closure);

	Function(const yy::location location,
			const FunctionVariantListRef m_variant_list,
			const weak_ptr<ExecutionContext> weak_closure);

	const shared_ptr<ExecutionContext> GetClosureReference() const;

	const_shared_ptr<FunctionVariant> GetFunctionVariant(
			ArgumentListRef argument_list) const;

	const yy::location m_location;
	const FunctionVariantListRef m_variant_list;
	const shared_ptr<ExecutionContext> m_closure;
	const weak_ptr<ExecutionContext> m_weak_closure;
};

#endif /* FUNCTION_H_ */

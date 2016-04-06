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

#ifndef COMPLEX_TYPE_H_
#define COMPLEX_TYPE_H_

#include <concrete_type.h>
#include <defaults.h>
#include <error.h>
#include <execution_context.h>

class TypeSpecifier;
class TypeTable;
class Result;
class ExecutionContext;
class Expression;

class ComplexType: public ConcreteType {
public:
	virtual ~ComplexType();
	static const_shared_ptr<TypeSpecifier> ToActualType(
			const_shared_ptr<TypeSpecifier> original, const TypeTable& table);

	const_shared_ptr<Result> PreprocessSymbol(
			const std::shared_ptr<ExecutionContext> execution_context,
			const_shared_ptr<ComplexTypeSpecifier> type_specifier,
			const_shared_ptr<Expression> initializer) const;

	const ErrorListRef Instantiate(
			const std::shared_ptr<ExecutionContext> execution_context,
			const_shared_ptr<ComplexTypeSpecifier> type_specifier,
			const_shared_ptr<std::string> instance_name,
			const_shared_ptr<Expression> initializer) const;

	virtual const_shared_ptr<TypeSpecifier> GetMemberTypeSpecifier(
			const_shared_ptr<std::string> member_name) const = 0;

	virtual const_shared_ptr<void> GetMemberDefaultValue(
			const_shared_ptr<std::string> member_name) const = 0;

	virtual const_shared_ptr<TypeTable> GetDefinition() const = 0;

protected:
	virtual const_shared_ptr<Result> PreprocessSymbolCore(
			const std::shared_ptr<ExecutionContext> execution_context,
			const_shared_ptr<ComplexTypeSpecifier> type_specifier,
			const_shared_ptr<Expression> initializer) const = 0;

	virtual const SetResult InstantiateCore(
			const std::shared_ptr<ExecutionContext> execution_context,
			const_shared_ptr<ComplexTypeSpecifier> type_specifier,
			const std::string& instance_name,
			const_shared_ptr<void> data) const = 0;
};

#endif /* COMPLEX_TYPE_H_ */

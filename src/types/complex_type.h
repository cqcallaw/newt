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

#include <type_definition.h>
#include <defaults.h>
#include <error.h>
#include <execution_context.h>

class TypeTable;
class Result;
class ExecutionContext;
class Expression;
class MaybeType;

class ComplexType: public TypeDefinition {
public:
	virtual ~ComplexType();

	const_shared_ptr<Result> PreprocessSymbol(
			const std::shared_ptr<ExecutionContext> execution_context,
			const_shared_ptr<ComplexTypeSpecifier> type_specifier,
			const_shared_ptr<Expression> initializer,
			const_shared_ptr<type_parameter_map> type_mapping) const;

	const ErrorListRef Instantiate(
			const std::shared_ptr<ExecutionContext> execution_context,
			const_shared_ptr<ComplexTypeSpecifier> type_specifier,
			const_shared_ptr<std::string> instance_name,
			const_shared_ptr<Expression> initializer) const;

	virtual const_shared_ptr<TypeTable> GetDefinition() const = 0;

	virtual const AnalysisResult AnalyzeConversion(
			const ComplexTypeSpecifier& current,
			const TypeSpecifier& unaliased_other) const = 0;

	virtual const_shared_ptr<MaybeType> GetMaybeType() const = 0;

	static std::string TypeSpecifierListToString(
			const TypeSpecifierListRef type_specifier_list);

	static TypedResult<type_parameter_map> GetTypeParameterMap(
			const TypeSpecifierListRef type_parameters,
			const TypeSpecifierListRef type_arguments,
			const TypeTable& type_table);

	static const_shared_ptr<type_parameter_map> DefaultTypeParameterMap;

	static const TypeSpecifierListRef TypeParameterSubstitution(
			const TypeSpecifierListRef original,
			const_shared_ptr<type_parameter_map> type_mapping);

protected:
	virtual const_shared_ptr<Result> PreprocessSymbolCore(
			const std::shared_ptr<ExecutionContext> execution_context,
			const_shared_ptr<ComplexTypeSpecifier> type_specifier,
			const_shared_ptr<Expression> initializer) const = 0;

	virtual const SetResult InstantiateCore(
			const std::shared_ptr<ExecutionContext> execution_context,
			const_shared_ptr<ComplexTypeSpecifier> type_specifier,
			const_shared_ptr<TypeSpecifier> value_type_specifier,
			const std::string& instance_name,
			const_shared_ptr<void> data) const = 0;
};

#endif /* COMPLEX_TYPE_H_ */

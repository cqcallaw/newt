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

#include <alias_definition.h>
#include <expression.h>
#include <linked_list.h>
#include <location.hh>
#include <position.hh>
#include <primitive_type.h>
#include <primitive_type_specifier.h>
#include <sum.h>
#include <sum_type.h>
#include <sum_type_specifier.h>
#include <symbol.h>
#include <memory>
#include <sstream>
#include <typeinfo>
#include <primitive_declaration_statement.h>
#include <record_declaration_statement.h>
#include <type_alias_declaration_statement.h>
#include <complex_instantiation_statement.h>
#include <record_type.h>
#include <execution_context.h>
#include <symbol_context.h>

const std::string SumType::ToString(const TypeTable& type_table,
		const Indent& indent) const {
	ostringstream os;
	Indent child_indent = indent + 1;
	os << child_indent << "<sum>" << endl;
	m_definition->print(os, child_indent);
	return os.str();
}

const std::string SumType::ValueToString(const TypeTable& type_table,
		const Indent& indent, const_shared_ptr<void> value) const {
	ostringstream buffer;
	auto sum_instance = static_pointer_cast<const Sum>(value);
	buffer << sum_instance->ToString(*this, type_table, indent);
	return buffer.str();
}

const_shared_ptr<Result> SumType::Build(
		const shared_ptr<ExecutionContext> context,
		const DeclarationListRef member_declarations) {
	ErrorListRef errors = ErrorList::GetTerminator();

	auto type_table = context->GetTypeTable();
	const shared_ptr<TypeTable> types = make_shared<TypeTable>();

	auto parent = SymbolContextList::From(context, context->GetParent());
	shared_ptr<ExecutionContext> tmp_context = make_shared<ExecutionContext>(
			Modifier::NONE, parent, types, EPHEMERAL);

	DeclarationListRef subject = member_declarations;
	while (!DeclarationList::IsTerminator(subject)) {
		const_shared_ptr<DeclarationStatement> declaration = subject->GetData();

		const_shared_ptr<Expression> initializer_expression =
				declaration->GetInitializerExpression();
		if (initializer_expression && !initializer_expression->IsConstant()) {
			//if we have a non-constant initializer expression, generate an error
			yy::location position = initializer_expression->GetPosition();
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::MEMBER_DEFAULTS_MUST_BE_CONSTANT,
							position.begin.line, position.begin.column),
					errors);
		}

		if (ErrorList::IsTerminator(errors)) {
			auto declaration = subject->GetData();
			auto variant_name = declaration->GetName();

			auto as_record = dynamic_pointer_cast<
					const RecordDeclarationStatement>(declaration);
			if (as_record) {
				auto record_type_name = as_record->GetName();
				auto validation_errors = as_record->preprocess(tmp_context);
				errors = errors->Concatenate(errors, validation_errors);
				//no need to "execute" the declaration statement, since all the work happens during preprocessing
			}

			auto as_alias = dynamic_pointer_cast<
					const TypeAliasDeclarationStatement>(declaration);
			if (as_alias) {
				auto alias_type_name = *as_alias->GetName();
				auto original_type_specifier = as_alias->GetTypeSpecifier();

				if (!types->ContainsType(alias_type_name)) {
					//would be nice to abstract this casting logic into a helper function
					auto original_as_primitive = dynamic_pointer_cast<
							const PrimitiveTypeSpecifier>(
							original_type_specifier);
					if (original_as_primitive) {
						auto alias = make_shared<AliasDefinition>(type_table,
								original_as_primitive);
						types->AddType(alias_type_name, alias);
					}

					auto original_as_complex = dynamic_pointer_cast<
							const ComplexTypeSpecifier>(
							original_type_specifier);
					if (original_as_complex) {
						auto original_name = original_as_complex->GetTypeName();
						auto original = type_table->GetType<RecordType>(
								original_name);
						if (original) {
							auto alias = make_shared<AliasDefinition>(
									type_table, original_as_complex);
							types->AddType(alias_type_name, alias);
						} else {
							errors =
									ErrorList::From(
											make_shared<Error>(Error::SEMANTIC,
													Error::UNDECLARED_TYPE,
													as_alias->GetTypePosition().begin.line,
													as_alias->GetTypePosition().begin.column,
													as_alias->GetTypeSpecifier()->ToString()),
											errors);
						}
					}
				} else {
					errors = ErrorList::From(
							make_shared<Error>(Error::SEMANTIC,
									Error::PREVIOUS_DECLARATION,
									as_alias->GetPosition().begin.line,
									as_alias->GetPosition().begin.column,
									alias_type_name), errors);
				}
			}
		}

		subject = subject->GetNext();
	}

	if (ErrorList::IsTerminator(errors)) {
		auto type = const_shared_ptr<SumType>(
				new SumType(types, member_declarations->GetData()));
		return make_shared<Result>(type, errors);
	} else {
		return make_shared<Result>(nullptr, errors);
	}
}

const WideningResult SumType::AnalyzeConversion(
		const ComplexTypeSpecifier& current, const TypeSpecifier& other) const {
	//Note: "other" type is a type that is potentially convertible to the sum type.
	auto count = m_definition->CountEntriesOfType(current, other);
	if (count > 1) {
		return AMBIGUOUS;
	} else if (count == 1) {
		return UNAMBIGUOUS;
	}

	return INCOMPATIBLE;
}

const_shared_ptr<Symbol> SumType::GetSymbol(const TypeTable& type_table,
		const_shared_ptr<TypeSpecifier> type_specifier,
		const_shared_ptr<void> value) const {
	auto original_as_complex = dynamic_pointer_cast<const ComplexTypeSpecifier>(
			type_specifier);

	if (original_as_complex) {
		auto type = original_as_complex->GetType(type_table, RESOLVE);

		auto as_sum = dynamic_pointer_cast<const SumType>(type);
		if (as_sum) {
			auto as_sum_type_specifier = make_shared<const SumTypeSpecifier>(
					original_as_complex->GetTypeName(),
					original_as_complex->GetContainer());
			auto as_sum = static_pointer_cast<const Sum>(value);
			return make_shared<Symbol>(as_sum_type_specifier, as_sum);
		}
	}

	return Symbol::GetDefaultSymbol();
}

const_shared_ptr<std::string> SumType::MapSpecifierToVariant(
		const ComplexTypeSpecifier& current, const TypeSpecifier& other) const {
	return make_shared<std::string>(
			m_definition->MapSpecifierToName(current, other));
}

const_shared_ptr<void> SumType::GetDefaultValue(
		const TypeTable& type_table) const {
	return Sum::GetDefaultInstance(*this);
}

const_shared_ptr<Result> SumType::PreprocessSymbolCore(
		const std::shared_ptr<ExecutionContext> execution_context,
		const_shared_ptr<ComplexTypeSpecifier> type_specifier,
		const_shared_ptr<Expression> initializer) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	plain_shared_ptr<Sum> instance = nullptr;
	plain_shared_ptr<Symbol> symbol = Symbol::GetDefaultSymbol();

	const_shared_ptr<TypeSpecifier> initializer_expression_type_specifier =
			initializer->GetTypeSpecifier(execution_context);
	auto widening_analysis = AnalyzeConversion(*type_specifier,
			*initializer_expression_type_specifier);

	if (initializer_expression_type_specifier->IsAssignableTo(type_specifier,
			execution_context->GetTypeTable())) {
		auto expression_type = initializer_expression_type_specifier->GetType(
				execution_context->GetTypeTable(), RESOLVE);

		auto as_sum = dynamic_pointer_cast<const SumType>(expression_type);
		if (as_sum) {
			//generate default instance; actual assignment must be done in execute stage
			//this is because assignment of constant expressions to sum types is only valid
			//if the constant expression is narrower than the sum type
			instance = Sum::GetDefaultInstance(*this);
		} else {
			if (widening_analysis == UNAMBIGUOUS) {
				if (initializer->IsConstant()) {
					//assignment of constant expressions to sum types is only valid if constant expression is narrower than the sum type
					//we can therefore assume that a new Sum must be created if we've hit this branch
					//widening conversions of non-constant initializers must be handled in the execute stage
					const_shared_ptr<Result> result = initializer->Evaluate(
							execution_context);
					errors = result->GetErrors();
					if (ErrorList::IsTerminator(errors)) {
						auto variant_name = MapSpecifierToVariant(
								*type_specifier,
								*initializer_expression_type_specifier);

						instance = make_shared<Sum>(variant_name,
								result->GetRawData());
					}
				}
			}
		}
	} else {

		if (widening_analysis == AMBIGUOUS) {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::AMBIGUOUS_WIDENING_CONVERSION,
							initializer->GetPosition().begin.line,
							initializer->GetPosition().begin.column,
							type_specifier->ToString(),
							initializer_expression_type_specifier->ToString()),
					errors);
		} else {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::ASSIGNMENT_TYPE_ERROR,
							initializer->GetPosition().begin.line,
							initializer->GetPosition().begin.column,
							type_specifier->ToString(),
							initializer_expression_type_specifier->ToString()),
					errors);
		}
	}

	if (ErrorList::IsTerminator(errors)) {
		symbol = make_shared<Symbol>(type_specifier, instance);
	}

	return make_shared<Result>(symbol, errors);
}

const_shared_ptr<void> SumType::GetMemberDefaultValue(
		const_shared_ptr<std::string> member_name) const {
	auto definition = m_definition->GetType<ConcreteType>(member_name);
	return definition->GetDefaultValue(*m_definition);
}

const_shared_ptr<TypeSpecifier> SumType::GetTypeSpecifier(
		const_shared_ptr<std::string> name,
		const_shared_ptr<ComplexTypeSpecifier> container) const {
	return make_shared<SumTypeSpecifier>(name, container);
}

const std::string SumType::GetValueSeperator(const Indent& indent,
		const_shared_ptr<void> value) const {
	auto as_sum = static_pointer_cast<const Sum>(value);
	auto variant_name = as_sum->GetTag();

	auto variant_type = m_definition->GetType<TypeDefinition>(variant_name);
	return variant_type->GetValueSeperator(indent, value);
}

const_shared_ptr<DeclarationStatement> SumType::GetDeclarationStatement(
		const yy::location position, const_shared_ptr<TypeSpecifier> type,
		const yy::location type_position, const_shared_ptr<std::string> name,
		const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) const {
	return make_shared<ComplexInstantiationStatement>(position,
			static_pointer_cast<const ComplexTypeSpecifier>(type),
			type_position, name, name_position, initializer_expression);
}

const SetResult SumType::InstantiateCore(
		const std::shared_ptr<ExecutionContext> execution_context,
		const_shared_ptr<ComplexTypeSpecifier> type_specifier,
		const std::string& instance_name, const_shared_ptr<void> data) const {
	auto instance = static_pointer_cast<const Sum>(data);
	auto specifier = dynamic_pointer_cast<const ComplexTypeSpecifier>(
			type_specifier);
	if (specifier) {
		auto set_result = execution_context->SetSymbol(instance_name, specifier,
				instance, execution_context->GetTypeTable());
		return set_result;
	} else {
		return SetResult::INCOMPATIBLE_TYPE;
	}
}

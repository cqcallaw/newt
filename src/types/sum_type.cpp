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
#include <unit_declaration_statement.h>
#include <record_type.h>
#include <execution_context.h>
#include <symbol_context.h>

#include <variable_expression.h>
#include <return_statement.h>
#include <with_expression.h>
#include <function_type.h>
#include <basic_variable.h>
#include <complex_instantiation_statement.h>
#include <statement_block.h>
#include <function_declaration.h>
#include <record.h>
#include <unit_type.h>

#include <maybe_type_specifier.h>

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
	buffer << sum_instance->ToString(*this, *m_definition, indent);
	return buffer.str();
}

const_shared_ptr<Result> SumType::Build(
		const shared_ptr<ExecutionContext> context,
		const DeclarationListRef member_declarations,
		const_shared_ptr<SumTypeSpecifier> sum_type_specifier) {
	ErrorListRef errors = ErrorList::GetTerminator();

	auto type_table = context->GetTypeTable();
	const shared_ptr<TypeTable> types = make_shared<TypeTable>(type_table);

	auto parent = SymbolContextList::From(context, context->GetParent());
	shared_ptr<ExecutionContext> tmp_context = make_shared<ExecutionContext>(
			Modifier::NONE, parent, types, EPHEMERAL);

	auto constructors = make_shared<SymbolTable>();

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

			auto as_unit = dynamic_pointer_cast<const UnitDeclarationStatement>(
					declaration);
			if (as_unit) {
				auto validation_errors = as_unit->preprocess(tmp_context);
				errors = errors->Concatenate(errors, validation_errors);
				//no need to "execute" the declaration statement, since all the work happens during preprocessing
			}

			auto as_record = dynamic_pointer_cast<
					const RecordDeclarationStatement>(declaration);
			if (as_record) {
				auto validation_errors = as_record->preprocess(tmp_context);
				errors = errors->Concatenate(errors, validation_errors);
				//no need to "execute" the declaration statement, since all the work happens during preprocessing
			}

			auto as_alias = dynamic_pointer_cast<
					const TypeAliasDeclarationStatement>(declaration);
			if (as_alias) {
				auto alias_type_name = *as_alias->GetName();
				auto alias_type_specifier = as_alias->GetTypeSpecifier();

				if (alias_type_name.compare("nil") == 0
						|| !types->ContainsType(alias_type_name)) {
					//would be nice to abstract this casting logic into a helper function
					auto alias_as_primitive = dynamic_pointer_cast<
							const PrimitiveTypeSpecifier>(alias_type_specifier);
					if (alias_as_primitive) {
						auto alias = make_shared<AliasDefinition>(type_table,
								alias_as_primitive, DIRECT);
						types->AddType(alias_type_name, alias);

						const_shared_ptr<PrimitiveDeclarationStatement> parameter_declaration =
								make_shared<PrimitiveDeclarationStatement>(
										as_alias->GetPosition(),
										alias_as_primitive,
										as_alias->GetTypePosition(),
										as_alias->GetName(),
										as_alias->GetNamePosition());
						DeclarationListRef parameter = DeclarationList::From(
								parameter_declaration,
								DeclarationList::GetTerminator());

						auto function_signature = make_shared<
								FunctionDeclaration>(parameter,
								sum_type_specifier, GetDefaultLocation());

						const_shared_ptr<Expression> return_expression =
								make_shared<VariableExpression>(
										GetDefaultLocation(),
										make_shared<BasicVariable>(
												declaration->GetName(),
												GetDefaultLocation()));

						const_shared_ptr<ReturnStatement> return_statement =
								make_shared<ReturnStatement>(return_expression);

						const StatementListRef statement_list =
								StatementList::From(return_statement,
										StatementList::GetTerminator());
						const_shared_ptr<StatementBlock> statement_block =
								make_shared<StatementBlock>(statement_list,
										as_alias->GetNamePosition());

						auto weak = weak_ptr<ExecutionContext>(context);
						const_shared_ptr<Function> function = make_shared<
								Function>(function_signature, statement_block,
								weak);

						auto symbol = make_shared<Symbol>(function);
						auto insert_result = constructors->InsertSymbol(
								*variant_name, symbol);
						//this should never happen because we already have an existence check on the variant type table,
						//but it never hurts to be safe
						if (insert_result == SYMBOL_EXISTS) {
							errors =
									ErrorList::From(
											make_shared<Error>(Error::SEMANTIC,
													Error::PREVIOUS_DECLARATION,
													as_alias->GetPosition().begin.line,
													as_alias->GetPosition().begin.column,
													*variant_name), errors);
						}
					}

					auto original_as_complex = dynamic_pointer_cast<
							const ComplexTypeSpecifier>(alias_type_specifier);
					if (original_as_complex) {
						auto original_name = original_as_complex->GetTypeName();
						auto original = type_table->GetType<ComplexType>(
								original_name, DEEP, RETURN);
						if (original) {
							auto alias = make_shared<AliasDefinition>(
									type_table, original_as_complex, DIRECT);
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
				new SumType(types, member_declarations->GetData(),
						constructors));
		return make_shared<Result>(type, errors);
	} else {
		return make_shared<Result>(nullptr, errors);
	}
}

const AnalysisResult SumType::AnalyzeConversion(
		const ComplexTypeSpecifier& current, const TypeSpecifier& other) const {
	//Note: "other" type is a type that is potentially convertible to the sum type.
	auto count = m_definition->CountEntriesOfType(current, other);
	if (count > 1) {
		return AnalysisResult::AMBIGUOUS;
	} else if (count == 1) {
		return AnalysisResult::UNAMBIGUOUS;
	}

	return AnalysisResult::INCOMPATIBLE;
}

const_shared_ptr<Symbol> SumType::GetSymbol(const TypeTable& type_table,
		const_shared_ptr<TypeSpecifier> type_specifier,
		const_shared_ptr<void> value) const {
	auto original_as_complex = dynamic_pointer_cast<const ComplexTypeSpecifier>(
			type_specifier);
	if (original_as_complex) {
		auto type = type_specifier->GetType(type_table, RESOLVE);

		auto as_sum = dynamic_pointer_cast<const SumType>(type);
		if (as_sum) {
			auto as_sum_type_specifier = make_shared<const SumTypeSpecifier>(
					original_as_complex->GetTypeName(),
					original_as_complex->GetContainer());
			auto as_sum = static_pointer_cast<const Sum>(value);
			return make_shared<Symbol>(as_sum_type_specifier, as_sum);
		}
	}

	auto original_as_maybe = dynamic_pointer_cast<const MaybeTypeSpecifier>(
			type_specifier);
	if (original_as_maybe) {
		auto as_sum = static_pointer_cast<const Sum>(value);
		return make_shared<Symbol>(original_as_maybe, as_sum);
	}

	assert(false);
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

	auto conversion_analysis =
			initializer_expression_type_specifier->IsAssignableTo(
					type_specifier, execution_context->GetTypeTable());
	if (conversion_analysis == EQUIVALENT) {
		auto expression_type = initializer_expression_type_specifier->GetType(
				execution_context->GetTypeTable(), RESOLVE);
		auto as_sum = dynamic_pointer_cast<const SumType>(expression_type);
		if (as_sum) {
			//generate default instance; actual assignment must be done in execute stage
			//this is because assignment of constant expressions to sum types is only valid
			//if the constant expression is narrower than the sum type
			instance = Sum::GetDefaultInstance(*this);
		} else {
			assert(false);
		}
	} else if (conversion_analysis == UNAMBIGUOUS) {
		if (initializer->IsConstant()) {
			//assignment of constant expressions to sum types is only valid if constant expression is narrower than the sum type
			//we can therefore assume that a new Sum must be created if we've hit this branch
			//widening conversions of non-constant initializers must be handled in the execute stage
			const_shared_ptr<Result> result = initializer->Evaluate(
					execution_context);
			errors = result->GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				auto variant_name = MapSpecifierToVariant(*type_specifier,
						*initializer_expression_type_specifier);
				instance = make_shared<Sum>(variant_name, result->GetRawData());
			}
		}
	} else if (conversion_analysis == AMBIGUOUS) {
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

	if (ErrorList::IsTerminator(errors)) {
		symbol = make_shared<Symbol>(type_specifier, instance);
	}

	return make_shared<Result>(symbol, errors);
}

const_shared_ptr<void> SumType::GetMemberDefaultValue(
		const_shared_ptr<std::string> member_name) const {
	auto definition = m_definition->GetType<TypeDefinition>(member_name,
			SHALLOW, RESOLVE);
	return definition->GetDefaultValue(*m_definition);
}

const_shared_ptr<TypeSpecifier> SumType::GetTypeSpecifier(
		const_shared_ptr<std::string> name,
		const_shared_ptr<ComplexTypeSpecifier> container) const {
	return make_shared<SumTypeSpecifier>(name, container);
}

const std::string SumType::GetValueSeparator(const Indent& indent,
		const void* value) const {
	auto as_sum = static_cast<const Sum*>(value);
	auto variant_name = *as_sum->GetTag();

	auto variant_type = m_definition->GetType<TypeDefinition>(variant_name,
			SHALLOW, RESOLVE);

	if (variant_type) {
		return variant_type->GetValueSeparator(indent, as_sum->GetValue().get());
	} else {
		assert(false);
		return "";
	}
}

const std::string SumType::GetTagSeparator(const Indent& indent,
		const void* value) const {
	auto as_sum = static_cast<const Sum*>(value);
	auto variant_name = *as_sum->GetTag();

	auto variant_type = m_definition->GetType<TypeDefinition>(variant_name,
			SHALLOW, RESOLVE);

	if (variant_type) {
		return variant_type->GetTagSeparator(indent, as_sum->GetValue().get());
	} else {
		assert(false);
		return "";
	}
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
		const_shared_ptr<TypeSpecifier> value_type_specifier,
		const std::string& instance_name, const_shared_ptr<void> data) const {
	auto value_type = value_type_specifier->GetType(
			execution_context->GetTypeTable(), RESOLVE);

	plain_shared_ptr<Sum> instance = nullptr;
	auto as_sum_type = dynamic_pointer_cast<const SumType>(value_type);
	if (as_sum_type) {
		instance = static_pointer_cast<const Sum>(data);
	} else {
		auto conversion_result = AnalyzeConversion(*type_specifier,
				*value_type_specifier);
		if (conversion_result == UNAMBIGUOUS) {
			auto tag = MapSpecifierToVariant(*type_specifier,
					*value_type_specifier);
			instance = make_shared<const Sum>(tag, data);
		} else {
			return INCOMPATIBLE_TYPE;
		}
	}

	auto set_result = execution_context->SetSymbol(instance_name,
			type_specifier, instance, execution_context->GetTypeTable());
	return set_result;
}

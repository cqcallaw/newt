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
#include <sum_declaration_statement.h>
#include <type_alias_declaration_statement.h>
#include <unit_declaration_statement.h>
#include <maybe_declaration_statement.h>
#include <record_type.h>
#include <maybe_type.h>
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
#include <function_expression.h>

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
		const shared_ptr<ExecutionContext> output,
		const shared_ptr<ExecutionContext> closure,
		const DeclarationListRef member_declarations,
		const_shared_ptr<SumTypeSpecifier> sum_type_specifier) {
	ErrorListRef errors = ErrorList::GetTerminator();

	auto output_type_table = output->GetTypeTable();
	auto closure_type_table = closure->GetTypeTable();
	const shared_ptr<TypeTable> definition = make_shared<TypeTable>(
			output_type_table);

	auto parent = ExecutionContextList::From(output, output->GetParent());
	auto tmp_context = make_shared<ExecutionContext>(Modifier::NONE, parent,
			definition, TEMPORARY, output->GetDepth() + 1);

	auto constructors = make_shared<ExecutionContext>(Modifier::Type::MUTABLE);

	DeclarationListRef subject = member_declarations;
	while (!DeclarationList::IsTerminator(subject)) {
		const_shared_ptr<DeclarationStatement> declaration = subject->GetData();

		const_shared_ptr<Expression> initializer_expression =
				declaration->GetInitializerExpression();
		if (initializer_expression && !initializer_expression->IsConstant()) {
			//if we have a non-constant initializer expression, generate an error
			yy::location position = initializer_expression->GetLocation();
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::MEMBER_DEFAULTS_MUST_BE_CONSTANT,
							position.begin), errors);
		}

		if (ErrorList::IsTerminator(errors)) {
			auto declaration = subject->GetData();
			auto variant_name = declaration->GetName();

			auto as_unit = dynamic_pointer_cast<const UnitDeclarationStatement>(
					declaration);
			if (as_unit) {
				auto validation_result = as_unit->Preprocess(tmp_context,
						output);
				auto validation_errors = validation_result.GetErrors();
				errors = errors->Concatenate(errors, validation_errors);
			}

			auto as_record = dynamic_pointer_cast<
					const RecordDeclarationStatement>(declaration);
			if (as_record) {
				auto validation_result = as_record->Preprocess(tmp_context,
						closure);
				auto validation_errors = validation_result.GetErrors();
				errors = errors->Concatenate(errors, validation_errors);
			}

			auto as_sum = dynamic_pointer_cast<const SumDeclarationStatement>(
					declaration);
			if (as_sum) {
				auto validation_result = as_sum->Preprocess(tmp_context,
						output);
				auto validation_errors = validation_result.GetErrors();
				errors = errors->Concatenate(errors, validation_errors);
			}

			auto as_alias = dynamic_pointer_cast<
					const TypeAliasDeclarationStatement>(declaration);
			if (as_alias) {
				auto alias_type_name = *as_alias->GetName();

				if (alias_type_name.compare(*TypeTable::GetNilName()) == 0
						|| !definition->ContainsType(alias_type_name)) {
					auto alias_type_specifier = as_alias->GetTypeSpecifier();

					//would be nice to abstract this casting logic into a helper function
					auto alias_as_primitive = dynamic_pointer_cast<
							const PrimitiveTypeSpecifier>(alias_type_specifier);
					if (alias_as_primitive) {
						auto alias = make_shared<AliasDefinition>(
								closure_type_table, alias_as_primitive, DIRECT);
						definition->AddType(alias_type_name, alias);

						auto type_constructor_result =
								TypeAliasDeclarationStatement::GetTypeConstructor(
										as_alias->GetLocation(),
										as_alias->GetName(),
										as_alias->GetNameLocation(),
										as_alias->GetTypeSpecifierLocation(),
										alias_as_primitive, sum_type_specifier,
										alias, output, closure);
						errors = ErrorList::Concatenate(errors,
								type_constructor_result.GetErrors());

						auto symbol = type_constructor_result.GetData();
						auto insert_result = constructors->InsertSymbol(
								*variant_name, symbol);
						// this should never happen because we already have an existence check on the variant type table,
						// but it never hurts to be safe
						if (insert_result == SYMBOL_EXISTS) {
							errors = ErrorList::From(
									make_shared<Error>(Error::SEMANTIC,
											Error::PREVIOUS_DECLARATION,
											as_alias->GetLocation().begin,
											*variant_name), errors);
						}
					}

					auto original_as_complex = dynamic_pointer_cast<
							const ComplexTypeSpecifier>(alias_type_specifier);
					if (original_as_complex) {
						auto original_name_result =
								original_as_complex->GetType(closure_type_table,
										RETURN);
						auto original_name_result_errors =
								original_name_result->GetErrors();
						if (ErrorList::IsTerminator(
								original_name_result_errors)) {
							auto alias = make_shared<AliasDefinition>(
									closure_type_table, original_as_complex,
									DIRECT);
							definition->AddType(alias_type_name, alias);
						} else {
							errors = ErrorList::Concatenate(errors,
									original_name_result_errors);
						}
					}
				} else {
					errors = ErrorList::From(
							make_shared<Error>(Error::SEMANTIC,
									Error::PREVIOUS_DECLARATION,
									as_alias->GetLocation().begin,
									alias_type_name), errors);
				}
			}
		}

		subject = subject->GetNext();
	}

	if (ErrorList::IsTerminator(errors)) {
		auto maybe_type_result = MaybeType::Build(closure, sum_type_specifier);
		errors = maybe_type_result->GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto maybe_type = maybe_type_result->GetData<MaybeType>();
			auto type = const_shared_ptr<SumType>(
					new SumType(definition,
							member_declarations->GetData()->GetName(),
							constructors, maybe_type));
			return make_shared<Result>(type, errors);
		}
	}

	return make_shared<Result>(nullptr, errors);
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
		auto type_result = type_specifier->GetType(type_table, RESOLVE);

		if (ErrorList::IsTerminator(type_result->GetErrors())) {
			auto type = type_result->GetData<TypeDefinition>();
			auto as_sum = dynamic_pointer_cast<const SumType>(type);
			if (as_sum) {
				auto as_sum_type_specifier =
						make_shared<const SumTypeSpecifier>(
								original_as_complex->GetTypeName(),
								original_as_complex->GetTypeParameterList(),
								original_as_complex->GetContainer());
				auto as_sum = static_pointer_cast<const Sum>(value);
				return make_shared<Symbol>(as_sum_type_specifier, as_sum);
			}
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
	auto result = Sum::GetDefaultInstance(*this);
	return result;
}

const_shared_ptr<Result> SumType::PreprocessSymbolCore(
		const std::shared_ptr<ExecutionContext> execution_context,
		const_shared_ptr<ComplexTypeSpecifier> type_specifier,
		const_shared_ptr<Expression> initializer) const {
	auto type_table = execution_context->GetTypeTable();
	plain_shared_ptr<const Sum> instance = static_pointer_cast<const Sum>(
			GetDefaultValue(type_table));
	plain_shared_ptr<Symbol> symbol = Symbol::GetDefaultSymbol();

	auto initializer_expression_type_specifier_result =
			initializer->GetTypeSpecifier(execution_context);

	auto errors = initializer_expression_type_specifier_result.GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto initializer_expression_type_specifier =
				initializer_expression_type_specifier_result.GetData();
		auto conversion_analysis =
				initializer_expression_type_specifier->AnalyzeAssignmentTo(
						type_specifier, execution_context->GetTypeTable());
		if (conversion_analysis == EQUIVALENT) {
			auto expression_type_result =
					initializer_expression_type_specifier->GetType(
							execution_context->GetTypeTable(), RESOLVE);

			errors = expression_type_result->GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				auto expression_type = expression_type_result->GetData<
						TypeDefinition>();
				auto as_sum = dynamic_pointer_cast<const SumType>(
						expression_type);
				if (as_sum) {
					//generate default instance; actual assignment must be done in execute stage
					//this is because assignment of constant expressions to sum types is only valid
					//if the constant expression is narrower than the sum type
					instance = Sum::GetDefaultInstance(*this);
				} else {
					assert(false);
				}
			}
		} else if (conversion_analysis == UNAMBIGUOUS) {
			if (initializer->IsConstant()) {
				//assignment of constant expressions to sum types is only valid if constant expression is narrower than the sum type
				//we can therefore assume that a new Sum must be created if we've hit this branch
				//widening conversions of non-constant initializers must be handled in the execute stage
				const_shared_ptr<Result> result = initializer->Evaluate(
						execution_context, execution_context);
				errors = result->GetErrors();
				if (ErrorList::IsTerminator(errors)) {
					auto variant_name = MapSpecifierToVariant(*type_specifier,
							*initializer_expression_type_specifier);
					instance = make_shared<Sum>(variant_name,
							result->GetRawData());
				}
			}
		} else if (conversion_analysis == AMBIGUOUS) {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::AMBIGUOUS_WIDENING_CONVERSION,
							initializer->GetLocation().begin,
							type_specifier->ToString(),
							initializer_expression_type_specifier->ToString()),
					errors);
		} else {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::ASSIGNMENT_TYPE_ERROR,
							initializer->GetLocation().begin,
							type_specifier->ToString(),
							initializer_expression_type_specifier->ToString()),
					errors);
		}

		if (ErrorList::IsTerminator(errors)) {
			symbol = make_shared<Symbol>(type_specifier, instance);
		}
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
		const_shared_ptr<ComplexTypeSpecifier> container,
		yy::location location) const {
	return make_shared<SumTypeSpecifier>(name,
			container ?
					container->GetTypeParameterList() :
					TypeSpecifierList::GetTerminator(), container, location);
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
	auto value_type_result = value_type_specifier->GetType(
			execution_context->GetTypeTable(), RESOLVE);

	plain_shared_ptr<Sum> instance = nullptr;
	auto errors = value_type_result->GetErrors();

	if (ErrorList::IsTerminator(errors)) {
		auto value_type = value_type_result->GetData<TypeDefinition>();
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
	} else {
		return INCOMPATIBLE_TYPE;
	}
}

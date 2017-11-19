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

#include <record_type.h>

#include <sstream>
#include <assert.h>
#include <complex_instantiation_statement.h>
#include <specifiers/type_specifier.h>
#include <maybe_type_specifier.h>
#include <memory>
#include <symbol_table.h>
#include <execution_context.h>
#include <expression.h>
#include <symbol_context.h>
#include <record.h>
#include <placeholder_type.h>
#include <maybe_type.h>
#include <unit_type.h>
#include <function_expression.h>
#include <function_type.h>
#include <function_declaration.h>

RecordType::RecordType(const_shared_ptr<TypeTable> definition,
		const Modifier::Type modifiers, const_shared_ptr<MaybeType> maybe_type) :
		m_definition(definition), m_modifiers(modifiers), m_maybe_type(
				maybe_type) {
}

const_shared_ptr<TypeDefinition> RecordType::GetMember(
		const std::string& name) const {
	return m_definition->GetType<TypeDefinition>(name, SHALLOW, RETURN);
}

RecordType::~RecordType() {
}

const string RecordType::ToString(const TypeTable& type_table,
		const Indent& indent) const {
	ostringstream os;
	Indent child_indent = indent + 1;
	os << child_indent << "<record>" << endl;
	m_definition->print(os, child_indent);
	return os.str();
}

const ErrorListRef RecordType::Build(const_shared_ptr<string> name,
		const shared_ptr<ExecutionContext> output,
		const shared_ptr<ExecutionContext> closure,
		const Modifier::Type modifiers,
		const DeclarationListRef member_declarations,
		const_shared_ptr<RecordTypeSpecifier> type_specifier) {
	ErrorListRef errors = ErrorList::GetTerminator();

	const_shared_ptr<Record> default_value = make_shared<Record>(
			make_shared<SymbolContext>(Modifier::Type::NONE));
	auto placeholder_symbol = make_shared<Symbol>(type_specifier,
			default_value);
	auto maybe_result = MaybeType::Build(closure, type_specifier);
	errors = maybe_result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto output_type_table = output->GetTypeTable();
		auto closure_type_table = closure->GetTypeTable();
		auto type_table = make_shared<TypeTable>(output_type_table);

		// forward declaration
		// we generate a placeholder type to enable detection of recursive member declarations
		// since one and only one type is under construction at any time,
		// a lookup that returns a placeholder is a lookup for the under-construction type
		// this should work even for complex cases, such as:
		// a {
		//   b {
		//     d:int,
		//     c:{
		//       e:b?
		//     }
		//   }
		// }
		//
		// if the single type under construction property no longer holds, would need to do some sort of cycle detection logic.

		auto maybe_type = maybe_result->GetData<MaybeType>();
		auto forward_declaration = make_shared<PlaceholderType>(name,
				placeholder_symbol, maybe_type);
		output_type_table->AddType(*name, forward_declaration);

		DeclarationListRef split_declarations =
				DeclarationList::GetTerminator();
		DeclarationListRef subject = member_declarations;
		while (!DeclarationList::IsTerminator(subject)) {
			const_shared_ptr<DeclarationStatement> declaration =
					subject->GetData();

			auto member_name = declaration->GetName();

			auto declaration_type_specifier = declaration->GetTypeSpecifier();
			auto declaration_errors =
					declaration_type_specifier->ValidateDeclaration(
							output_type_table, declaration->GetNameLocation());

			auto existing_member_type = type_table->GetType<TypeDefinition>(
					member_name, SHALLOW, RETURN);

			if (!existing_member_type) {
				const shared_ptr<symbol_map> values = make_shared<symbol_map>();
				auto member_type_table = make_shared<TypeTable>(
						output_type_table);
				// generate a temporary structure in which to perform evaluations
				// of the member declaration statement
				// N.B. that this context is "TEMPORARY" so that strong references aren't made to it.
				auto member_tmp_context = ExecutionContext::GetEmptyChild(
						output, output->GetModifiers(), TEMPORARY,
						member_type_table, values);

				if (ErrorList::IsTerminator(declaration_errors)) {
					const_shared_ptr<Expression> initializer_expression =
							declaration->GetInitializerExpression();
					if (initializer_expression
							&& !initializer_expression->IsConstant()) {
						// if we have a non-constant initializer expression, generate an error
						yy::location position =
								initializer_expression->GetLocation();
						declaration_errors = ErrorList::From(
								make_shared<Error>(Error::SEMANTIC,
										Error::MEMBER_DEFAULTS_MUST_BE_CONSTANT,
										position.begin.line,
										position.begin.column),
								declaration_errors);
					}

					if (ErrorList::IsTerminator(declaration_errors)) {
						// otherwise (no initializer expression OR a valid initializer expression);
						// we're cleared to proceed
						auto declaration_subject = declaration;

						// handle split function declarations
						auto function_initializer_expression =
								dynamic_pointer_cast<const FunctionExpression>(
										initializer_expression);
						if (function_initializer_expression) {
							auto location =
									function_initializer_expression->GetLocation();
							auto variants =
									function_initializer_expression->GetVariantList();
							auto bare_variants =
									FunctionVariantList::GetTerminator();

							auto variant = variants;
							while (!FunctionVariantList::IsTerminator(variant)) {
								auto variant_definition = variant->GetData();

								auto bare_statement_block =
										FunctionType::GetDefaultStatementBlock(
												variant_definition->GetDeclaration()->GetReturnTypeSpecifier(),
												output_type_table);
								auto bare_variant =
										make_shared<FunctionVariant>(
												variant_definition->GetLocation(),
												variant_definition->GetDeclaration(),
												bare_statement_block);
								bare_variants = FunctionVariantList::From(
										bare_variant, bare_variants);
								variant = variant->GetNext();
							}

							auto bare_expression = make_shared<
									FunctionExpression>(location,
									bare_variants);
							declaration_subject = const_shared_ptr<
									DeclarationStatement>(
									declaration->WithInitializerExpression(
											bare_expression));

							split_declarations = DeclarationList::From(
									declaration, split_declarations);
						}

						auto preprocess_result =
								declaration_subject->Preprocess(
										member_tmp_context, closure);
						auto preprocess_errors = preprocess_result.GetErrors();
						declaration_errors = ErrorList::Concatenate(
								declaration_errors, preprocess_errors);

						if (ErrorList::IsTerminator(declaration_errors)) {
							// we've pre-processed this statement without issue;
							// finalize declaration by running Execute pass
							declaration_errors =
									ErrorList::Concatenate(declaration_errors,
											declaration_subject->Execute(
													member_tmp_context, closure).GetErrors());
						}
					}
				}

				if (ErrorList::IsTerminator(declaration_errors)) {
					symbol_map::iterator iter;
					// extract member definitions into type aliases
					for (iter = values->begin(); iter != values->end();
							++iter) {
						const string member_name = iter->first;
						auto symbol = iter->second;

						const_shared_ptr<TypeSpecifier> type_specifier =
								symbol->GetTypeSpecifier();

						auto symbol_type_result = type_specifier->GetType(
								output_type_table);
						auto symbol_type_errors =
								symbol_type_result->GetErrors();
						if (ErrorList::IsTerminator(symbol_type_errors)) {
							auto symbol_type = symbol_type_result->GetData<
									TypeDefinition>();
							auto as_recursive = dynamic_pointer_cast<
									const PlaceholderType>(symbol_type);
							plain_shared_ptr<AliasDefinition> alias = nullptr;
							if (as_recursive) {
								auto as_maybe_specifier = dynamic_pointer_cast<
										const MaybeTypeSpecifier>(
										type_specifier);
								if (as_maybe_specifier) {
									alias = make_shared<AliasDefinition>(
											closure_type_table, type_specifier,
											RECURSIVE, nullptr);
								} else {
									assert(false);
								}
							} else {
								auto value = symbol->GetValue();
								alias = make_shared<AliasDefinition>(
										closure_type_table, type_specifier,
										DIRECT, value);
							}

							type_table->AddType(member_name, alias);
						} else {
							declaration_errors = ErrorList::Concatenate(
									declaration_errors, symbol_type_errors);
						}
					}

					// add a type definition if we have one (that is, no aliasing occurred)
					auto member_definition = member_type_table->GetType<
							TypeDefinition>(member_name, SHALLOW, RETURN);
					if (member_definition) {
						type_table->AddType(*member_name, member_definition);
					}
				}
			} else {
				declaration_errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::PREVIOUS_DECLARATION,
								declaration->GetNameLocation().begin.line,
								declaration->GetNameLocation().begin.column,
								*member_name), declaration_errors);
			}

			errors = ErrorList::Concatenate(declaration_errors, errors);
			subject = subject->GetNext();
		}

		if (ErrorList::IsTerminator(errors)) {
			// update output with what we have so far
			auto type = const_shared_ptr<RecordType>(
					new RecordType(type_table, modifiers, maybe_type));
			output_type_table->ReplaceTypeDefinition<PlaceholderType>(*name,
					type);

			// handle split declarations
			auto split_declaration_subject = split_declarations;
			while (!DeclarationList::IsTerminator(split_declaration_subject)) {
				auto split_declaration_subject_data =
						split_declaration_subject->GetData();

				auto member_name = split_declaration_subject_data->GetName();

				auto declaration_type_specifier =
						split_declaration_subject_data->GetTypeSpecifier();
				auto declaration_errors =
						declaration_type_specifier->ValidateDeclaration(
								output_type_table,
								split_declaration_subject_data->GetNameLocation());

				const shared_ptr<symbol_map> values = make_shared<symbol_map>();
				auto member_type_table = make_shared<TypeTable>(type_table);
				// generate a temp context
				// N.B. that this context is "TEMPORARY" so that strong references aren't made to it.
				auto member_tmp_context = ExecutionContext::GetEmptyChild(
						output, output->GetModifiers(), TEMPORARY,
						member_type_table, values);

				auto preprocess_result =
						split_declaration_subject_data->Preprocess(
								member_tmp_context, closure);
				auto preprocess_errors = preprocess_result.GetErrors();
				declaration_errors = ErrorList::Concatenate(declaration_errors,
						preprocess_errors);

				if (ErrorList::IsTerminator(declaration_errors)) {
					// we've pre-processed this statement without issue;
					// finalize declaration by running Execute pass
					errors = ErrorList::Concatenate(errors,
							split_declaration_subject_data->Execute(
									member_tmp_context, closure).GetErrors());
				} else {
					errors = ErrorList::Concatenate(errors, declaration_errors);
				}

				if (ErrorList::IsTerminator(declaration_errors)) {
					symbol_map::iterator iter;
					// for split declarations, we assume aliasing occurred
					for (iter = values->begin(); iter != values->end();
							++iter) {
						const string member_name = iter->first;
						auto symbol = iter->second;

						const_shared_ptr<TypeSpecifier> type_specifier =
								symbol->GetTypeSpecifier();

						auto value = symbol->GetValue();
						auto alias = make_shared<AliasDefinition>(
								closure_type_table, type_specifier, DIRECT,
								value);

						type_table->ReplaceTypeDefinition<TypeDefinition>(
								member_name, alias);
					}
				}

				split_declaration_subject =
						split_declaration_subject->GetNext();
			}

			if (!ErrorList::IsTerminator(errors)) {
				// an error occurred during split definition processing; remove type
				output_type_table->RemoveTypeDefinition<RecordType>(name);
			}

		} else {
			output_type_table->RemoveTypeDefinition<PlaceholderType>(name);
		}
	}

	return errors;
}

const_shared_ptr<void> RecordType::GetMemberDefaultValue(
		const_shared_ptr<std::string> member_name) const {
	return GetMember(*member_name)->GetDefaultValue(*m_definition);
}

const std::string RecordType::ValueToString(const TypeTable& type_table,
		const Indent& indent, const_shared_ptr<void> value) const {
	ostringstream buffer;
	auto record_type_instance = static_pointer_cast<const Record>(value);
	buffer << record_type_instance->ToString(*m_definition, indent + 1);
	return buffer.str();
}

const_shared_ptr<void> RecordType::GetDefaultValue(
		const TypeTable& type_table) const {
	return Record::GetDefaultInstance(*this);
}

const_shared_ptr<Symbol> RecordType::GetSymbol(const TypeTable& type_table,
		const_shared_ptr<TypeSpecifier> type_specifier,
		const_shared_ptr<void> value) const {
	auto as_complex_specifier =
			dynamic_pointer_cast<const ComplexTypeSpecifier>(type_specifier);

	if (as_complex_specifier) {
		auto as_record = static_pointer_cast<const Record>(value);
		return make_shared<Symbol>(as_complex_specifier, as_record);
	}

	return Symbol::GetDefaultSymbol();
}

const_shared_ptr<Result> RecordType::PreprocessSymbolCore(
		const std::shared_ptr<ExecutionContext> execution_context,
		const_shared_ptr<ComplexTypeSpecifier> type_specifier,
		const_shared_ptr<Expression> initializer) const {

	plain_shared_ptr<Record> instance = nullptr;
	plain_shared_ptr<Symbol> symbol = Symbol::GetDefaultSymbol();

	auto initializer_expression_type_result = initializer->GetTypeSpecifier(
			execution_context);

	auto errors = initializer_expression_type_result.GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto initializer_expression_type =
				initializer_expression_type_result.GetData();
		auto initializer_analysis =
				initializer_expression_type->AnalyzeAssignmentTo(type_specifier,
						execution_context->GetTypeTable());
		if (initializer_analysis == EQUIVALENT) {
			if (initializer->IsConstant()) {
				const_shared_ptr<Result> result = initializer->Evaluate(
						execution_context, execution_context);
				errors = result->GetErrors();
				if (ErrorList::IsTerminator(errors)) {
					instance = result->GetData<Record>();
				}
			} else {
				instance = Record::GetDefaultInstance(*this);
			}
		} else {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::ASSIGNMENT_TYPE_ERROR,
							initializer->GetLocation().begin.line,
							initializer->GetLocation().begin.column,
							type_specifier->ToString(),
							initializer_expression_type->ToString()), errors);
		}

		if (ErrorList::IsTerminator(errors)) {
			auto as_complex_specifier = dynamic_pointer_cast<
					const ComplexTypeSpecifier>(type_specifier);
			if (as_complex_specifier) {
				symbol = make_shared<Symbol>(as_complex_specifier, instance);
			} else {
				//TODO: error handling
				assert(false);
			}
		}
	}

	return make_shared<Result>(symbol, errors);
}

const_shared_ptr<TypeSpecifier> RecordType::GetTypeSpecifier(
		const_shared_ptr<std::string> name,
		const_shared_ptr<ComplexTypeSpecifier> container,
		yy::location location) const {
	return make_shared<RecordTypeSpecifier>(name, container, location);
}

const SetResult RecordType::InstantiateCore(
		const std::shared_ptr<ExecutionContext> execution_context,
		const_shared_ptr<ComplexTypeSpecifier> type_specifier,
		const_shared_ptr<TypeSpecifier> value_type_specifier,
		const std::string& instance_name, const_shared_ptr<void> data) const {
	auto instance = static_pointer_cast<const Record>(data);
	auto specifier = dynamic_pointer_cast<const ComplexTypeSpecifier>(
			type_specifier);

	if (specifier
			&& value_type_specifier->AnalyzeAssignmentTo(type_specifier,
					execution_context->GetTypeTable())) {
		auto set_result = execution_context->SetSymbol(instance_name, specifier,
				instance, execution_context->GetTypeTable());
		return set_result;
	} else {
		return SetResult::INCOMPATIBLE_TYPE;
	}
}

const std::string RecordType::GetValueSeparator(const Indent& indent,
		const void* value) const {
	return "\n";
}

const std::string RecordType::GetTagSeparator(const Indent& indent,
		const void* value) const {
	return indent.ToString();
}

const_shared_ptr<DeclarationStatement> RecordType::GetDeclarationStatement(
		const yy::location position, const_shared_ptr<TypeSpecifier> type,
		const yy::location type_position, const_shared_ptr<std::string> name,
		const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) const {
	auto as_complex_specifier =
			dynamic_pointer_cast<const ComplexTypeSpecifier>(type);
	if (as_complex_specifier) {
		return make_shared<ComplexInstantiationStatement>(position,
				as_complex_specifier, type_position, name, name_position,
				initializer_expression);
	} else {
		assert(false);
		return nullptr;
	}
}

const AnalysisResult RecordType::AnalyzeConversion(
		const ComplexTypeSpecifier& current,
		const TypeSpecifier& unaliased_other) const {
	return INCOMPATIBLE;
}

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
#include <specifiers/type_specifier.h>
#include <memory>
#include <symbol_table.h>
#include <execution_context.h>
#include <expression.h>
#include <symbol_context.h>
#include <record.h>
#include <complex_instantiation_statement.h>
#include <placeholder_type.h>

RecordType::RecordType(const_shared_ptr<TypeTable> definition,
		const Modifier::Type modifiers) :
		m_definition(definition), m_modifiers(modifiers) {
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

const_shared_ptr<Result> RecordType::Build(
		const shared_ptr<ExecutionContext> context,
		const Modifier::Type modifiers,
		const DeclarationListRef member_declarations) {
	ErrorListRef errors = ErrorList::GetTerminator();

	//generate a temporary structure in which to perform evaluations
	//of the member declaration statements
	const shared_ptr<symbol_map> values = make_shared<symbol_map>();

	auto context_type_table = context->GetTypeTable();
	auto type_table = make_shared<TypeTable>(context_type_table);
	shared_ptr<ExecutionContext> struct_context =
			ExecutionContext::GetEmptyChild(context, context->GetModifiers(),
					EPHEMERAL, type_table, values);

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
		} else {
			//otherwise (no initializer expression OR a valid initializer expression), preprocess
			auto preprocess_errors = declaration->preprocess(struct_context);
			errors = ErrorList::Concatenate(errors, preprocess_errors);
		}

		if (ErrorList::IsTerminator(errors)) {
			//we've pre-processed this statement without issue
			errors = ErrorList::Concatenate(errors,
					declaration->execute(struct_context));
		}

		subject = subject->GetNext();
	}

	symbol_map::iterator iter;
	if (ErrorList::IsTerminator(errors)) {
		//extract member definitions into type aliases
		int suffix_length = DeclarationStatement::CONSTRUCTOR_SUFFIX.length();
		for (iter = values->begin(); iter != values->end(); ++iter) {
			const string member_name = iter->first;
			auto symbol = iter->second;
			const_shared_ptr<TypeSpecifier> type_specifier =
					symbol->GetTypeSpecifier();

			//special case: check for type constructors and don't alias them
			auto as_complex = dynamic_pointer_cast<const ComplexTypeSpecifier>(
					type_specifier);
			if (as_complex) {
				auto type_name = *as_complex->GetTypeName();
				int length = type_name.length();
				if (length >= suffix_length
						&& type_name.compare(length - suffix_length,
								suffix_length,
								DeclarationStatement::CONSTRUCTOR_SUFFIX)
								== 0) {
					//our member identifier is a type constructor; don't add it
					continue;
				}
			}

			auto value = symbol->GetValue();

			auto symbol_type = type_specifier->GetType(context_type_table);
			auto as_sum_recursive = dynamic_pointer_cast<const PlaceholderType>(
					symbol_type);

			plain_shared_ptr<AliasDefinition> alias = nullptr;
			if (as_sum_recursive) {
				alias = make_shared<AliasDefinition>(context_type_table,
						type_specifier, nullptr);
			} else {
				alias = make_shared<AliasDefinition>(context_type_table,
						type_specifier, value);
			}

			type_table->AddType(member_name, alias);
		}
	}

	auto type = make_shared<RecordType>(type_table, modifiers);
	return make_shared<Result>(type, errors);
}

const_shared_ptr<void> RecordType::GetMemberDefaultValue(
		const_shared_ptr<std::string> member_name) const {
	return GetMember(*member_name)->GetDefaultValue(*m_definition);
}

const std::string RecordType::ValueToString(const TypeTable& type_table,
		const Indent& indent, const_shared_ptr<void> value) const {
	ostringstream buffer;
	auto as_record = static_pointer_cast<const Record>(value);
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
	auto as_record_specifier = dynamic_pointer_cast<const RecordTypeSpecifier>(
			type_specifier);

	if (as_record_specifier) {
		auto as_record = static_pointer_cast<const Record>(value);
		return make_shared<Symbol>(as_record_specifier, as_record);
	}

	return Symbol::GetDefaultSymbol();
}

const_shared_ptr<Result> RecordType::PreprocessSymbolCore(
		const std::shared_ptr<ExecutionContext> execution_context,
		const_shared_ptr<ComplexTypeSpecifier> type_specifier,
		const_shared_ptr<Expression> initializer) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	plain_shared_ptr<Record> instance = nullptr;
	plain_shared_ptr<Symbol> symbol = Symbol::GetDefaultSymbol();

	const_shared_ptr<TypeSpecifier> initializer_expression_type =
			initializer->GetTypeSpecifier(execution_context);
	if (initializer_expression_type->IsAssignableTo(type_specifier,
			execution_context->GetTypeTable())) {
		if (initializer->IsConstant()) {
			const_shared_ptr<Result> result = initializer->Evaluate(
					execution_context);
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
						initializer->GetPosition().begin.line,
						initializer->GetPosition().begin.column,
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

	return make_shared<Result>(symbol, errors);
}

const_shared_ptr<TypeSpecifier> RecordType::GetTypeSpecifier(
		const_shared_ptr<std::string> name,
		const_shared_ptr<ComplexTypeSpecifier> container) const {
	return make_shared<RecordTypeSpecifier>(name, container);
}

const SetResult RecordType::InstantiateCore(
		const std::shared_ptr<ExecutionContext> execution_context,
		const_shared_ptr<ComplexTypeSpecifier> type_specifier,
		const_shared_ptr<TypeSpecifier> value_type_specifier,
		const std::string& instance_name, const_shared_ptr<void> data) const {
	auto instance = static_pointer_cast<const Record>(data);
	auto specifier = dynamic_pointer_cast<const RecordTypeSpecifier>(
			type_specifier);

	if (specifier
			&& value_type_specifier->IsAssignableTo(type_specifier,
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

const WideningResult RecordType::AnalyzeConversion(
		const ComplexTypeSpecifier& current,
		const TypeSpecifier& unaliased_other) const {
	return INCOMPATIBLE;
}

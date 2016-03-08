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
#include <member_definition.h>
#include <sstream>
#include <assert.h>
#include <specifiers/type_specifier.h>
#include <memory>
#include <symbol_table.h>
#include <execution_context.h>
#include <expression.h>

RecordType::RecordType(const_shared_ptr<definition_map> definition,
		const Modifier::Type modifiers) :
		m_definition(definition), m_modifiers(modifiers) {
}

const_shared_ptr<MemberDefinition> RecordType::GetMember(
		const std::string& name) const {
	auto result = m_definition->find(name);
	if (result != m_definition->end()) {
		return result->second;
	} else {
		return MemberDefinition::GetDefaultMemberDefinition();
	}
}

RecordType::~RecordType() {
}

const string RecordType::ToString(const TypeTable& type_table,
		const Indent& indent) const {
	ostringstream os;
	Indent child_indent = indent + 1;
	if (m_definition->size() > 0) {
		for (auto& type_iter : *m_definition) {
			const string member_name = type_iter.first;
			const_shared_ptr<MemberDefinition> member_definition =
					type_iter.second;
			const_shared_ptr<TypeSpecifier> member_type =
					member_definition->GetType();

			os << child_indent << member_type->ToString() << " " << member_name
					<< " ("
					<< member_definition->ToString(type_table, child_indent)
					<< ")";

			os << endl;
		}
	} else {
		os << child_indent << "<empty>" << endl;
	}
	return os.str();
}

const_shared_ptr<Result> RecordType::Build(
		const_shared_ptr<ExecutionContext> context,
		const Modifier::Type modifiers,
		const DeclarationListRef member_declarations) {
	ErrorListRef errors = ErrorList::GetTerminator();

	//generate a temporary structure in which to perform evaluations
	//of the member declaration statements
	const shared_ptr<symbol_map> values = make_shared<symbol_map>();
	volatile_shared_ptr<SymbolTable> member_buffer = make_shared<SymbolTable>(
			Modifier::NONE, values);
	shared_ptr<ExecutionContext> struct_context = context->WithContents(
			member_buffer);

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

	volatile_shared_ptr<definition_map> mapping = make_shared<definition_map>();
	symbol_map::iterator iter;
	if (ErrorList::IsTerminator(errors)) {
		//we've evaluated everything without issue
		//extract member declaration information into immutable MemberDefinition
		for (iter = values->begin(); iter != values->end(); ++iter) {
			const string member_name = iter->first;
			auto symbol = iter->second;
			const_shared_ptr<TypeSpecifier> type = symbol->GetType();
			auto value = symbol->GetValue();
			const_shared_ptr<MemberDefinition> definition = make_shared<
					MemberDefinition>(type, value);
			mapping->insert(
					pair<const string, const_shared_ptr<MemberDefinition>>(
							member_name, definition));
		}
	}

	auto type = make_shared<RecordType>(mapping, modifiers);
	return make_shared<Result>(type, errors);
}

const_shared_ptr<TypeSpecifier> RecordType::GetMemberType(
		const std::string& member_name) const {
	return GetMember(member_name)->GetType();
}

const std::string RecordType::ValueToString(const TypeTable& type_table,
		const Indent& indent, const_shared_ptr<void> value) const {
	ostringstream buffer;
	auto as_record = static_pointer_cast<const Record>(value);
	buffer
			<< Symbol::ToString(as_record->GetTypeSpecifier(), value,
					type_table, indent);
	return buffer.str();
}

const_shared_ptr<void> RecordType::GetDefaultValue(
		const_shared_ptr<std::string> type_name) const {
	return Record::GetDefaultInstance(type_name, *this);
}

const_shared_ptr<Symbol> RecordType::GetSymbol(const_shared_ptr<void> value,
		const_shared_ptr<ComplexTypeSpecifier> container) const {
	auto cast = static_pointer_cast<const Record>(value);
	if (container) {
		return make_shared<Symbol>(container, cast);
	} else {
		return make_shared<Symbol>(cast);
	}
}

bool RecordType::IsSpecifiedBy(const std::string& name,
		const TypeSpecifier& type_specifier) const {
	try {
		const RecordTypeSpecifier& as_record =
				dynamic_cast<const RecordTypeSpecifier&>(type_specifier);
		return name == *as_record.GetTypeName();
	} catch (std::bad_cast& e) {
		return false;
	}
}

const_shared_ptr<Result> RecordType::GenerateSymbolCore(
		const std::shared_ptr<ExecutionContext> execution_context,
		const_shared_ptr<ComplexTypeSpecifier> type_specifier,
		const_shared_ptr<Expression> initializer) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	plain_shared_ptr<Record> instance = nullptr;
	plain_shared_ptr<Symbol> symbol = Symbol::GetDefaultSymbol();

	auto type_name = type_specifier->GetTypeName();

	const_shared_ptr<TypeSpecifier> initializer_expression_type =
			initializer->GetType(execution_context);
	const_shared_ptr<RecordTypeSpecifier> as_record_specifier =
			std::dynamic_pointer_cast<const RecordTypeSpecifier>(
					initializer_expression_type);
	if (as_record_specifier
			&& initializer_expression_type->IsAssignableTo(type_specifier)) {
		if (initializer->IsConstant()) {
			const_shared_ptr<Result> result = initializer->Evaluate(
					execution_context);
			errors = result->GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				instance = result->GetData<Record>();
			}
		} else {
			//generate default instance
			instance = Record::GetDefaultInstance(type_name, *this);
		}
	} else {
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC,
						Error::ASSIGNMENT_TYPE_ERROR,
						initializer->GetPosition().begin.line,
						initializer->GetPosition().begin.column, *type_name,
						initializer_expression_type->ToString()), errors);
	}

	if (ErrorList::IsTerminator(errors)) {
		symbol = make_shared<Symbol>(instance);
	}

	return make_shared<Result>(symbol, errors);
}

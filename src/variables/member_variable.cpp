/*
 * member_variable.cpp
 *
 *  Created on: Aug 29, 2015
 *      Author: caleb
 */

#include <record_type.h>
#include <member_variable.h>
#include <execution_context.h>
#include <symbol_table.h>
#include <sstream>
#include <result.h>
#include <member_definition.h>
#include <nested_type_specifier.h>
#include <sum_type.h>

MemberVariable::MemberVariable(const_shared_ptr<Variable> container,
		const_shared_ptr<Variable> member_variable) :
		Variable(container->GetName(), container->GetLocation()), m_container(
				container), m_member_variable(member_variable) {
}

MemberVariable::~MemberVariable() {
}

const_shared_ptr<TypeSpecifier> MemberVariable::GetType(
		const shared_ptr<ExecutionContext> context) const {
	const_shared_ptr<TypeSpecifier> container_type_specifier =
			m_container->GetType(context);

	shared_ptr<const void> value = nullptr;
	auto type_table = *context->GetTypeTable();

	const_shared_ptr<NestedTypeSpecifier> as_nested = std::dynamic_pointer_cast<
			const NestedTypeSpecifier>(container_type_specifier);
	if (as_nested) {
		value = as_nested->DefaultValue(type_table);
	}

	const_shared_ptr<RecordTypeSpecifier> as_record_type =
			std::dynamic_pointer_cast<const RecordTypeSpecifier>(
					container_type_specifier);
	if (as_record_type) {
		value = as_record_type->DefaultValue(type_table);
	}

	if (value) {
		auto instance = std::static_pointer_cast<const Record>(value);
		auto new_context = context->WithContents(instance->GetDefinition());
		const_shared_ptr<TypeSpecifier> result = m_member_variable->GetType(
				new_context);
		return result;
	} else {
		return PrimitiveTypeSpecifier::GetNone();
	}
}

const_shared_ptr<string> MemberVariable::ToString(
		const shared_ptr<ExecutionContext> context) const {
	ostringstream buffer;
	buffer << "<" << *GetName() << "." << *m_member_variable->ToString(context)
			<< ">";
	return make_shared<string>(buffer.str());
}

const_shared_ptr<Result> MemberVariable::Evaluate(
		const shared_ptr<ExecutionContext> context) const {
	ErrorListRef errors(ErrorList::GetTerminator());

	const_shared_ptr<TypeSpecifier> container_type = m_container->GetType(
			context);
	if (container_type != PrimitiveTypeSpecifier::GetNone()) {
		const_shared_ptr<Result> container_result = m_container->Evaluate(
				context);

		errors = container_result->GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto instance = container_result->GetData<Record>();
			volatile_shared_ptr<SymbolContext> new_symbol_context =
					instance->GetDefinition();
			auto new_context = context->WithContents(new_symbol_context);
			const_shared_ptr<Result> member_result =
					m_member_variable->Evaluate(new_context);
			return member_result;
		}
	} else {
		errors = ErrorList::From(
				make_shared<Error>(Error::RUNTIME, Error::UNDECLARED_VARIABLE,
						m_container->GetLocation().begin.line,
						m_container->GetLocation().begin.column, *(GetName())),
				errors);
	}

	const_shared_ptr<Result> result = make_shared<Result>(
			const_shared_ptr<void>(nullptr), errors);
	return result;
}

const ErrorListRef MemberVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<bool> value) const {
	ErrorListRef errors(ErrorList::GetTerminator());
	const_shared_ptr<Result> container_result = m_container->Evaluate(context);

	errors = container_result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		SetResult set_result = NO_SET_RESULT;

		const_shared_ptr<TypeSpecifier> container_type = m_container->GetType(
				context);
		const_shared_ptr<RecordTypeSpecifier> as_record =
				std::dynamic_pointer_cast<const RecordTypeSpecifier>(
						container_type);

		if (as_record) {
			auto instance = container_result->GetData<Record>();
			const std::string member_name = *(m_member_variable->GetName());
			set_result = instance->GetDefinition()->SetSymbol(member_name,
					value);
		} else {
			set_result = INCOMPATIBLE_TYPE;
		}

		return ToErrorListRef(set_result, m_member_variable->GetType(context),
				PrimitiveTypeSpecifier::GetBoolean());
	} else {
		return errors;
	}
}

const ErrorListRef MemberVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<int> value) const {
	ErrorListRef errors(ErrorList::GetTerminator());

	const_shared_ptr<Result> container_result = m_container->Evaluate(context);

	SetResult set_result = NO_SET_RESULT;

	errors = container_result->GetErrors();
	if (ErrorList::Reverse(errors)) {
		const_shared_ptr<TypeSpecifier> container_type = m_container->GetType(
				context);
		const_shared_ptr<RecordTypeSpecifier> as_record =
				std::dynamic_pointer_cast<const RecordTypeSpecifier>(
						container_type);

		if (as_record) {
			auto instance = container_result->GetData<Record>();
			const std::string member_name = *(m_member_variable->GetName());
			set_result = instance->GetDefinition()->SetSymbol(member_name,
					value);
		} else {
			set_result = INCOMPATIBLE_TYPE;
		}
	} else {
		return errors;
	}

	return ToErrorListRef(set_result, m_member_variable->GetType(context),
			PrimitiveTypeSpecifier::GetInt());
}

const ErrorListRef MemberVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<double> value) const {
	ErrorListRef errors(ErrorList::GetTerminator());

	const_shared_ptr<Result> container_result = m_container->Evaluate(context);

	SetResult set_result = NO_SET_RESULT;

	errors = container_result->GetErrors();
	if (ErrorList::Reverse(errors)) {
		const_shared_ptr<TypeSpecifier> container_type = m_container->GetType(
				context);
		const_shared_ptr<RecordTypeSpecifier> as_record = dynamic_pointer_cast<
				const RecordTypeSpecifier>(container_type);

		if (as_record) {
			auto instance = container_result->GetData<Record>();
			const std::string member_name = *(m_member_variable->GetName());
			set_result = instance->GetDefinition()->SetSymbol(member_name,
					value);
		} else {
			set_result = INCOMPATIBLE_TYPE;
		}
	} else {
		return errors;
	}

	return ToErrorListRef(set_result, m_member_variable->GetType(context),
			PrimitiveTypeSpecifier::GetDouble());
}

const ErrorListRef MemberVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<std::string> value) const {
	ErrorListRef errors(ErrorList::GetTerminator());

	const_shared_ptr<Result> container_result = m_container->Evaluate(context);

	SetResult set_result = NO_SET_RESULT;

	errors = container_result->GetErrors();
	if (ErrorList::Reverse(errors)) {
		const_shared_ptr<TypeSpecifier> container_type = m_container->GetType(
				context);
		const_shared_ptr<RecordTypeSpecifier> as_record =
				std::dynamic_pointer_cast<const RecordTypeSpecifier>(
						container_type);

		if (as_record) {
			auto instance = container_result->GetData<Record>();
			const std::string member_name = *(m_member_variable->GetName());
			set_result = instance->GetDefinition()->SetSymbol(member_name,
					value);
		} else {
			set_result = INCOMPATIBLE_TYPE;
		}
	} else {
		return errors;
	}

	return ToErrorListRef(set_result, m_member_variable->GetType(context),
			PrimitiveTypeSpecifier::GetString());
}

const ErrorListRef MemberVariable::AssignValue(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<Expression> expression,
		const AssignmentType op) const {
	ErrorListRef errors(ErrorList::GetTerminator());

	const_shared_ptr<Result> container_evaluation = GetContainer()->Evaluate(
			context);
	errors = container_evaluation->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		//we're assigning a struct member reference
		auto struct_value = container_evaluation->GetData<Record>();
		shared_ptr<SymbolContext> definition = struct_value->GetDefinition();

		const auto new_parent_context = SymbolContextList::From(context,
				context->GetParent());
		auto new_context = context->WithContents(definition)->WithParent(
				new_parent_context);

		const_shared_ptr<Variable> new_variable = GetMemberVariable();
		errors = new_variable->AssignValue(new_context, expression, op);
	}

	return errors;
}

const ErrorListRef MemberVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<Record> value,
		const_shared_ptr<ComplexTypeSpecifier> container) const {
	ErrorListRef errors(ErrorList::GetTerminator());

	const_shared_ptr<Result> container_result = m_container->Evaluate(context);

	SetResult set_result = NO_SET_RESULT;

	errors = container_result->GetErrors();
	if (ErrorList::Reverse(errors)) {
		const_shared_ptr<TypeSpecifier> container_type = m_container->GetType(
				context);
		const_shared_ptr<RecordTypeSpecifier> as_record =
				std::dynamic_pointer_cast<const RecordTypeSpecifier>(
						container_type);

		if (as_record) {
			auto instance = container_result->GetData<Record>();
			const std::string member_name = *(m_member_variable->GetName());
			set_result = instance->GetDefinition()->SetSymbol(member_name,
					value, container);
		} else {
			set_result = INCOMPATIBLE_TYPE;
		}
	} else {
		return errors;
	}

	return ToErrorListRef(set_result, m_member_variable->GetType(context),
			value->GetTypeSpecifier());
}

const ErrorListRef MemberVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<Array> value) const {
	ErrorListRef errors(ErrorList::GetTerminator());

	const_shared_ptr<Result> container_result = m_container->Evaluate(context);

	SetResult set_result = NO_SET_RESULT;

	errors = container_result->GetErrors();
	if (ErrorList::Reverse(errors)) {
		const_shared_ptr<TypeSpecifier> container_type = m_container->GetType(
				context);
		const_shared_ptr<RecordTypeSpecifier> as_record =
				std::dynamic_pointer_cast<const RecordTypeSpecifier>(
						container_type);

		if (as_record) {
			auto instance = container_result->GetData<Record>();
			const std::string member_name = *(m_member_variable->GetName());
			set_result = instance->GetDefinition()->SetSymbol(member_name,
					value);
		} else {
			set_result = INCOMPATIBLE_TYPE;
		}
	} else {
		return errors;
	}

	return ToErrorListRef(set_result, m_member_variable->GetType(context),
			value->GetTypeSpecifier());
}

const ErrorListRef MemberVariable::Validate(
		const shared_ptr<ExecutionContext> context) const {
	ErrorListRef errors(ErrorList::GetTerminator());

	auto symbol = context->GetSymbol(m_container->GetName(), DEEP);

	if (symbol && symbol != Symbol::GetDefaultSymbol()) {
		const_shared_ptr<TypeSpecifier> container_type = m_container->GetType(
				context);

		const_shared_ptr<NestedTypeSpecifier> as_nested =
				std::dynamic_pointer_cast<const NestedTypeSpecifier>(
						container_type);
		if (as_nested) {
			auto parent_name = as_nested->GetParent()->GetTypeName();
			auto container_definition =
					context->GetTypeTable()->GetType<SumType>(parent_name);

			if (container_definition) {
				auto member_name = as_nested->GetMemberName();
				auto member = container_definition->GetTypeTable()->GetType<
						RecordType>(member_name);
				if (!member) {
					errors =
							ErrorList::From(
									make_shared<Error>(Error::SEMANTIC,
											Error::UNDECLARED_MEMBER,
											m_member_variable->GetLocation().begin.line,
											m_member_variable->GetLocation().begin.column,
											*member_name,
											as_nested->ToString()), errors);
				}
			} else {
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::UNDECLARED_TYPE,
								m_container->GetLocation().begin.line,
								m_container->GetLocation().begin.column,
								*parent_name), errors);
			}
		} else {
			const_shared_ptr<RecordTypeSpecifier> as_record =
					std::dynamic_pointer_cast<const RecordTypeSpecifier>(
							container_type);
			if (as_record) {
				const_shared_ptr<TypeSpecifier> variable_type = GetType(
						context);

				if (variable_type == PrimitiveTypeSpecifier::GetNone()) {
					errors =
							ErrorList::From(
									make_shared<Error>(Error::SEMANTIC,
											Error::UNDECLARED_MEMBER,
											m_member_variable->GetLocation().begin.line,
											m_member_variable->GetLocation().begin.column,
											*m_member_variable->GetName(),
											*as_record->GetTypeName()), errors);
				}
			} else {
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::NOT_A_COMPOUND_TYPE,
								m_container->GetLocation().begin.line,
								m_container->GetLocation().begin.column,
								*m_container->GetName()), errors);
			}
		}
	} else {
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						m_container->GetLocation().begin.line,
						m_container->GetLocation().begin.column, *(GetName())),
				errors);
	}

	return errors;
}

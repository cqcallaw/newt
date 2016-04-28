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
#include <record.h>
#include <nested_type_specifier.h>
#include <sum_type.h>
#include <unit_type.h>

MemberVariable::MemberVariable(const_shared_ptr<Variable> container,
		const_shared_ptr<Variable> member_variable) :
		Variable(container->GetName(), container->GetLocation()), m_container(
				container), m_member_variable(member_variable) {
}

MemberVariable::~MemberVariable() {
}

const_shared_ptr<TypeSpecifier> MemberVariable::GetTypeSpecifier(
		const shared_ptr<ExecutionContext> context,
		AliasResolution resolution) const {
	const_shared_ptr<TypeSpecifier> container_type_specifier =
			m_container->GetTypeSpecifier(context);

	shared_ptr<const void> value = nullptr;
	auto type_table = *context->GetTypeTable();

	auto type = container_type_specifier->GetType(type_table, RESOLVE);
	if (type) {
		auto member_name = m_member_variable->GetName();

		auto as_record = dynamic_pointer_cast<const RecordType>(type);
		if (as_record) {
			auto member_type = as_record->GetDefinition()->GetType<
					TypeDefinition>(*member_name, SHALLOW, RESOLVE);

			if (member_type) {
				auto output = make_shared<NestedTypeSpecifier>(
						container_type_specifier, member_name);

				if (resolution == AliasResolution::RETURN) {
					return output;
				} else {
					return NestedTypeSpecifier::Resolve(output, type_table);
				}
			}
		}
	}

	//try type constructor lookup
	auto as_sum = context->GetTypeTable()->GetType<SumType>(
			m_container->GetName(), DEEP, RESOLVE);
	if (as_sum) {
		auto constructors = as_sum->GetConstructors();
		auto constructor_symbol = constructors->GetSymbol(
				*m_member_variable->GetName());
		if (constructor_symbol != Symbol::GetDefaultSymbol()) {
			auto type_specifier = constructor_symbol->GetTypeSpecifier();
			return type_specifier;
		}
	}

	return PrimitiveTypeSpecifier::GetNone();
}

const_shared_ptr<string> MemberVariable::ToString(
		const shared_ptr<ExecutionContext> context) const {
	ostringstream buffer;
	buffer << "<" << *GetName() << "."
			<< *(m_member_variable->ToString(context)) << ">";
	return make_shared<string>(buffer.str());
}

const_shared_ptr<Result> MemberVariable::Evaluate(
		const shared_ptr<ExecutionContext> context) const {
	ErrorListRef errors(ErrorList::GetTerminator());

	const_shared_ptr<TypeSpecifier> container_type_specifier =
			m_container->GetTypeSpecifier(context);
	if (container_type_specifier != PrimitiveTypeSpecifier::GetNone()) {
		const_shared_ptr<Result> container_result = m_container->Evaluate(
				context);

		errors = container_result->GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto as_record = container_result->GetData<Record>();
			if (as_record) {
				volatile_shared_ptr<SymbolContext> new_symbol_context =
						as_record->GetDefinition();
				auto new_context = context->WithContents(new_symbol_context);
				const_shared_ptr<Result> member_result =
						m_member_variable->Evaluate(new_context);
				return member_result;
			}
		}
	} else {
		//try type constructor lookup
		auto as_sum_type = context->GetTypeTable()->GetType<SumType>(
				m_container->GetName(), DEEP, RESOLVE);
		if (as_sum_type) {
			auto constructors = as_sum_type->GetConstructors();
			auto constructor_symbol = constructors->GetSymbol(
					*m_member_variable->GetName());
			if (constructor_symbol) {
				return make_shared<Result>(constructor_symbol->GetValue(),
						errors);
			}
		} else {
			//no dice: we have a real legit error
			errors = ErrorList::From(
					make_shared<Error>(Error::RUNTIME,
							Error::UNDECLARED_VARIABLE,
							m_container->GetLocation().begin.line,
							m_container->GetLocation().begin.column,
							*(m_container->GetName())), errors);
		}
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

		const_shared_ptr<TypeSpecifier> container_type =
				m_container->GetTypeSpecifier(context);
		const_shared_ptr<RecordTypeSpecifier> as_record =
				std::dynamic_pointer_cast<const RecordTypeSpecifier>(
						container_type);

		if (as_record) {
			auto instance = container_result->GetData<Record>();
			const std::string member_name = *(m_member_variable->GetName());
			set_result = instance->GetDefinition()->SetSymbol(member_name,
					value, context->GetTypeTable());
		} else {
			set_result = INCOMPATIBLE_TYPE;
		}

		return ToErrorListRef(set_result,
				m_member_variable->GetTypeSpecifier(context),
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
		const_shared_ptr<TypeSpecifier> container_type =
				m_container->GetTypeSpecifier(context);
		const_shared_ptr<RecordTypeSpecifier> as_record =
				std::dynamic_pointer_cast<const RecordTypeSpecifier>(
						container_type);

		if (as_record) {
			auto instance = container_result->GetData<Record>();
			const std::string member_name = *(m_member_variable->GetName());
			set_result = instance->GetDefinition()->SetSymbol(member_name,
					value, context->GetTypeTable());
		} else {
			set_result = INCOMPATIBLE_TYPE;
		}
	} else {
		return errors;
	}

	return ToErrorListRef(set_result,
			m_member_variable->GetTypeSpecifier(context),
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
		const_shared_ptr<TypeSpecifier> container_type =
				m_container->GetTypeSpecifier(context);
		const_shared_ptr<RecordTypeSpecifier> as_record = dynamic_pointer_cast<
				const RecordTypeSpecifier>(container_type);

		if (as_record) {
			auto instance = container_result->GetData<Record>();
			const std::string member_name = *(m_member_variable->GetName());
			set_result = instance->GetDefinition()->SetSymbol(member_name,
					value, context->GetTypeTable());
		} else {
			set_result = INCOMPATIBLE_TYPE;
		}
	} else {
		return errors;
	}

	return ToErrorListRef(set_result,
			m_member_variable->GetTypeSpecifier(context),
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
		const_shared_ptr<TypeSpecifier> container_type =
				m_container->GetTypeSpecifier(context);
		const_shared_ptr<RecordTypeSpecifier> as_record =
				std::dynamic_pointer_cast<const RecordTypeSpecifier>(
						container_type);

		if (as_record) {
			auto instance = container_result->GetData<Record>();
			const std::string member_name = *(m_member_variable->GetName());
			set_result = instance->GetDefinition()->SetSymbol(member_name,
					value, context->GetTypeTable());
		} else {
			set_result = INCOMPATIBLE_TYPE;
		}
	} else {
		return errors;
	}

	return ToErrorListRef(set_result,
			m_member_variable->GetTypeSpecifier(context),
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
		if (struct_value) {
			shared_ptr<SymbolContext> definition =
					struct_value->GetDefinition();

			const auto new_parent_context = SymbolContextList::From(context,
					context->GetParent());
			auto new_context = context->WithContents(definition)->WithParent(
					new_parent_context);

			const_shared_ptr<Variable> new_variable = GetMemberVariable();
			errors = new_variable->AssignValue(new_context, expression, op);
		} else {
			//TODO: consider a more descriptive error message.
			//the basic idea is that sum types don't have re-assignable members
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::NOT_A_COMPOUND_TYPE,
							m_container->GetLocation().begin.line,
							m_container->GetLocation().begin.column,
							*m_container->GetName()), errors);
		}
	}

	return errors;
}

const ErrorListRef MemberVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<ComplexTypeSpecifier> type,
		const_shared_ptr<Record> value) const {
	ErrorListRef errors(ErrorList::GetTerminator());

	const_shared_ptr<Result> container_result = m_container->Evaluate(context);

	SetResult set_result = NO_SET_RESULT;

	errors = container_result->GetErrors();
	if (ErrorList::Reverse(errors)) {
		const_shared_ptr<TypeSpecifier> container_type =
				m_container->GetTypeSpecifier(context);
		const_shared_ptr<RecordTypeSpecifier> as_record =
				std::dynamic_pointer_cast<const RecordTypeSpecifier>(
						container_type);

		if (as_record) {
			auto instance = container_result->GetData<Record>();
			const std::string member_name = *(m_member_variable->GetName());
			set_result = instance->GetDefinition()->SetSymbol(member_name, type,
					value, context->GetTypeTable());
		} else {
			set_result = INCOMPATIBLE_TYPE;
		}
	} else {
		return errors;
	}

	return ToErrorListRef(set_result,
			m_member_variable->GetTypeSpecifier(context), type);
}

const ErrorListRef MemberVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<Array> value) const {
	ErrorListRef errors(ErrorList::GetTerminator());

	const_shared_ptr<Result> container_result = m_container->Evaluate(context);

	SetResult set_result = NO_SET_RESULT;

	errors = container_result->GetErrors();
	if (ErrorList::Reverse(errors)) {
		const_shared_ptr<TypeSpecifier> container_type =
				m_container->GetTypeSpecifier(context);
		const_shared_ptr<RecordTypeSpecifier> as_record =
				std::dynamic_pointer_cast<const RecordTypeSpecifier>(
						container_type);

		if (as_record) {
			auto instance = container_result->GetData<Record>();
			const std::string member_name = *(m_member_variable->GetName());
			set_result = instance->GetDefinition()->SetSymbol(member_name,
					value, context->GetTypeTable());
		} else {
			set_result = INCOMPATIBLE_TYPE;
		}
	} else {
		return errors;
	}

	return ToErrorListRef(set_result,
			m_member_variable->GetTypeSpecifier(context),
			value->GetTypeSpecifier());
}

const ErrorListRef MemberVariable::Validate(
		const shared_ptr<ExecutionContext> context) const {
	ErrorListRef errors(ErrorList::GetTerminator());

	auto symbol = context->GetSymbol(m_container->GetName(), DEEP);
	if (symbol && symbol != Symbol::GetDefaultSymbol()) {
		const_shared_ptr<TypeSpecifier> container_type_specifier =
				m_container->GetTypeSpecifier(context);

		auto type = container_type_specifier->GetType(context->GetTypeTable(),
				RESOLVE);

		auto as_complex = dynamic_pointer_cast<const ComplexType>(type);
		if (as_complex) {
			auto member_definition = as_complex->GetDefinition()->GetType<
					TypeDefinition>(m_member_variable->GetName(), SHALLOW,
					RESOLVE);
			if (!member_definition) {
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::UNDECLARED_MEMBER,
								m_member_variable->GetLocation().begin.line,
								m_member_variable->GetLocation().begin.column,
								*m_member_variable->ToString(context),
								container_type_specifier->ToString()), errors);
			}
		} else {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::NOT_A_COMPOUND_TYPE,
							m_container->GetLocation().begin.line,
							m_container->GetLocation().begin.column,
							*m_container->ToString(context)), errors);
		}
	} else {
		auto as_sum_type = context->GetTypeTable()->GetType<SumType>(
				m_container->GetName(), DEEP, RESOLVE);
		if (as_sum_type) {
			auto constructor = as_sum_type->GetConstructors()->GetSymbol(
					*m_member_variable->GetName());

			if (!constructor) {
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::UNDECLARED_MEMBER,
								m_member_variable->GetLocation().begin.line,
								m_member_variable->GetLocation().begin.column,
								*m_member_variable->ToString(context),
								*m_container->ToString(context)), errors);
			}
		} else {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::UNDECLARED_VARIABLE,
							m_container->GetLocation().begin.line,
							m_container->GetLocation().begin.column,
							*m_container->ToString(context)), errors);
		}
	}

	return errors;
}

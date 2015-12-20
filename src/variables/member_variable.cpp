/*
 * member_variable.cpp
 *
 *  Created on: Aug 29, 2015
 *      Author: caleb
 */

#include <compound_type.h>
#include <member_variable.h>
#include <execution_context.h>
#include <symbol_table.h>
#include <sstream>
#include <result.h>
#include <member_definition.h>

MemberVariable::MemberVariable(const Variable* container,
		const Variable* member_variable) :
		Variable(container->GetName(), container->GetLocation()), m_container(
				container), m_member_variable(member_variable) {
}

MemberVariable::~MemberVariable() {
}

const_shared_ptr<TypeSpecifier> MemberVariable::GetType(
		const ExecutionContext* context) const {
	const_shared_ptr<TypeSpecifier> container_type_specifier =
			m_container->GetType(context);
	const_shared_ptr<CompoundTypeSpecifier> as_compound_type =
			std::dynamic_pointer_cast<const CompoundTypeSpecifier>(
					container_type_specifier);
	if (as_compound_type) {
		const string type_name = as_compound_type->GetTypeName();

		const_shared_ptr<CompoundTypeInstance> instance =
				std::static_pointer_cast<const CompoundTypeInstance>(
						as_compound_type->DefaultValue(
								*context->GetTypeTable()));
		ExecutionContext* new_context = context->WithSymbolContext(
				instance->GetDefinition());
		const_shared_ptr<TypeSpecifier> result = m_member_variable->GetType(
				new_context);
		delete new_context;
		return result;
	}

	return PrimitiveTypeSpecifier::GetNone();
}

const std::string* MemberVariable::ToString(
		const ExecutionContext* context) const {
	ostringstream buffer;
	buffer << "<" << *GetName() << "." << *m_member_variable->ToString(context)
			<< ">";
	return new string(buffer.str());
}

const Result* MemberVariable::Evaluate(const ExecutionContext* context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::GetTerminator();
	const void* result_value = nullptr;

	const_shared_ptr<TypeSpecifier> container_type = m_container->GetType(
			context);
	const_shared_ptr<CompoundTypeSpecifier> as_compound =
			std::dynamic_pointer_cast<const CompoundTypeSpecifier>(
					container_type);

	if (container_type != PrimitiveTypeSpecifier::GetNone()) {
		if (as_compound != nullptr) {
			const Result* container_result = m_container->Evaluate(context);

			errors = container_result->GetErrors();
			if (errors == LinkedList<const Error*>::GetTerminator()) {
				auto instance = static_pointer_cast<const CompoundTypeInstance>(
						container_result->GetData());
				SymbolContext* new_symbol_context = instance->GetDefinition();
				ExecutionContext* new_context = context->WithSymbolContext(
						new_symbol_context);
				const Result* member_result = m_member_variable->Evaluate(
						new_context);
				return member_result;
			}
		} else {
			errors = errors->With(
					new Error(Error::SEMANTIC,
							Error::VARIABLE_NOT_A_COMPOUND_TYPE,
							m_container->GetLocation().begin.line,
							m_container->GetLocation().begin.column,
							*(GetName())));
		}
	} else {
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						m_container->GetLocation().begin.line,
						m_container->GetLocation().begin.column, *(GetName())));
	}

	auto wrapper = const_shared_ptr<void>(result_value);
	const Result* result = new Result(wrapper, errors);
	return result;
}

const LinkedList<const Error*>* MemberVariable::SetSymbol(
		const ExecutionContext* context, const_shared_ptr<bool> value) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::GetTerminator();

	const Result* container_result = m_container->Evaluate(context);

	SetResult set_result = NO_SET_RESULT;

	errors = container_result->GetErrors();
	if (errors == LinkedList<const Error*>::GetTerminator()) {
		const_shared_ptr<TypeSpecifier> container_type = m_container->GetType(
				context);
		const_shared_ptr<CompoundTypeSpecifier> as_compound =
				std::dynamic_pointer_cast<const CompoundTypeSpecifier>(
						container_type);

		if (as_compound != nullptr) {
			auto instance = static_pointer_cast<const CompoundTypeInstance>(
					container_result->GetData());
			const string member_name = *(m_member_variable->GetName());
			set_result = instance->GetDefinition()->SetSymbol(member_name,
					value);
		} else {
			set_result = INCOMPATIBLE_TYPE;
		}
	} else {
		return errors;
	}

	return ToErrorList(set_result, m_member_variable->GetType(context),
			PrimitiveTypeSpecifier::GetBoolean());
}

const LinkedList<const Error*>* MemberVariable::SetSymbol(
		const ExecutionContext* context, const_shared_ptr<int> value) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::GetTerminator();

	const Result* container_result = m_container->Evaluate(context);

	SetResult set_result = NO_SET_RESULT;

	errors = container_result->GetErrors();
	if (errors == LinkedList<const Error*>::GetTerminator()) {
		const_shared_ptr<TypeSpecifier> container_type = m_container->GetType(
				context);
		const_shared_ptr<CompoundTypeSpecifier> as_compound =
				std::dynamic_pointer_cast<const CompoundTypeSpecifier>(
						container_type);

		if (as_compound != nullptr) {
			auto instance = static_pointer_cast<const CompoundTypeInstance>(
					container_result->GetData());
			const string member_name = *(m_member_variable->GetName());
			set_result = instance->GetDefinition()->SetSymbol(member_name,
					value);
		} else {
			set_result = INCOMPATIBLE_TYPE;
		}
	} else {
		return errors;
	}

	return ToErrorList(set_result, m_member_variable->GetType(context),
			PrimitiveTypeSpecifier::GetInt());
}

const LinkedList<const Error*>* MemberVariable::SetSymbol(
		const ExecutionContext* context, const_shared_ptr<double> value) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::GetTerminator();

	const Result* container_result = m_container->Evaluate(context);

	SetResult set_result = NO_SET_RESULT;

	errors = container_result->GetErrors();
	if (errors == LinkedList<const Error*>::GetTerminator()) {
		const_shared_ptr<TypeSpecifier> container_type = m_container->GetType(
				context);
		const_shared_ptr<CompoundTypeSpecifier> as_compound =
				dynamic_pointer_cast<const CompoundTypeSpecifier>(
						container_type);

		if (as_compound != nullptr) {
			auto instance = static_pointer_cast<const CompoundTypeInstance>(
					container_result->GetData());
			const string member_name = *(m_member_variable->GetName());
			set_result = instance->GetDefinition()->SetSymbol(member_name,
					value);
		} else {
			set_result = INCOMPATIBLE_TYPE;
		}
	} else {
		return errors;
	}

	return ToErrorList(set_result, m_member_variable->GetType(context),
			PrimitiveTypeSpecifier::GetDouble());
}

const LinkedList<const Error*>* MemberVariable::SetSymbol(
		const ExecutionContext* context, const_shared_ptr<string> value) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::GetTerminator();

	const Result* container_result = m_container->Evaluate(context);

	SetResult set_result = NO_SET_RESULT;

	errors = container_result->GetErrors();
	if (errors == LinkedList<const Error*>::GetTerminator()) {
		const_shared_ptr<TypeSpecifier> container_type = m_container->GetType(
				context);
		const_shared_ptr<CompoundTypeSpecifier> as_compound =
				std::dynamic_pointer_cast<const CompoundTypeSpecifier>(
						container_type);

		if (as_compound != nullptr) {
			auto instance = static_pointer_cast<const CompoundTypeInstance>(
					container_result->GetData());
			const string member_name = *(m_member_variable->GetName());
			set_result = instance->GetDefinition()->SetSymbol(member_name,
					value);
		} else {
			set_result = INCOMPATIBLE_TYPE;
		}
	} else {
		return errors;
	}

	return ToErrorList(set_result, m_member_variable->GetType(context),
			PrimitiveTypeSpecifier::GetString());
}

const LinkedList<const Error*>* MemberVariable::AssignValue(
		const ExecutionContext* context, const Expression* expression,
		const AssignmentType op) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::GetTerminator();

	const Result* container_evaluation = GetContainer()->Evaluate(context);
	errors = container_evaluation->GetErrors();
	if (errors == LinkedList<const Error*>::GetTerminator()) {
		//we're assigning a struct member reference
		auto struct_value = static_pointer_cast<const CompoundTypeInstance>(
				container_evaluation->GetData());
		const SymbolContext* definition = struct_value->GetDefinition();

		SymbolContext* symbol_context = context->GetSymbolContext();
		const auto parent_context = symbol_context->GetParent();
		const auto new_parent_context = parent_context->With(symbol_context);

		SymbolContext* new_definition = new SymbolContext(
				definition->GetModifiers(), new_parent_context,
				definition->GetTable());

		ExecutionContext* new_context = context->WithSymbolContext(
				new_definition);
		const Variable* new_variable = GetMemberVariable();

		errors = new_variable->AssignValue(new_context, expression, op);
		delete (new_definition);
		delete (new_context);
	}

	return errors;
}

const LinkedList<const Error*>* MemberVariable::SetSymbol(
		const ExecutionContext* context,
		const_shared_ptr<CompoundTypeInstance> value) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::GetTerminator();

	const Result* container_result = m_container->Evaluate(context);

	SetResult set_result = NO_SET_RESULT;

	errors = container_result->GetErrors();
	if (errors == LinkedList<const Error*>::GetTerminator()) {
		const_shared_ptr<TypeSpecifier> container_type = m_container->GetType(
				context);
		const_shared_ptr<CompoundTypeSpecifier> as_compound =
				std::dynamic_pointer_cast<const CompoundTypeSpecifier>(
						container_type);

		if (as_compound != nullptr) {
			auto instance = static_pointer_cast<const CompoundTypeInstance>(
					container_result->GetData());
			const string member_name = *(m_member_variable->GetName());
			set_result = instance->GetDefinition()->SetSymbol(member_name,
					value);
		} else {
			set_result = INCOMPATIBLE_TYPE;
		}
	} else {
		return errors;
	}

	return ToErrorList(set_result, m_member_variable->GetType(context),
			value->GetTypeSpecifier());
}

const LinkedList<const Error*>* MemberVariable::SetSymbol(
		const ExecutionContext* context, const_shared_ptr<Array> value) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::GetTerminator();

	const Result* container_result = m_container->Evaluate(context);

	SetResult set_result = NO_SET_RESULT;

	errors = container_result->GetErrors();
	if (errors == LinkedList<const Error*>::GetTerminator()) {
		const_shared_ptr<TypeSpecifier> container_type = m_container->GetType(
				context);
		const_shared_ptr<CompoundTypeSpecifier> as_compound =
				std::dynamic_pointer_cast<const CompoundTypeSpecifier>(
						container_type);

		if (as_compound != nullptr) {
			auto instance = static_pointer_cast<const CompoundTypeInstance>(
					container_result->GetData());
			const string member_name = *(m_member_variable->GetName());
			set_result = instance->GetDefinition()->SetSymbol(member_name,
					value);
		} else {
			set_result = INCOMPATIBLE_TYPE;
		}
	} else {
		return errors;
	}

	return ToErrorList(set_result, m_member_variable->GetType(context),
			value->GetTypeSpecifier());
}

const LinkedList<const Error*>* MemberVariable::Validate(
		const ExecutionContext* context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::GetTerminator();

	const SymbolContext* symbol_context = context->GetSymbolContext();
	const Symbol* symbol = symbol_context->GetSymbol(m_container->GetName(),
			DEEP);

	if (symbol != nullptr && symbol != Symbol::GetDefaultSymbol()) {
		const_shared_ptr<TypeSpecifier> container_type = m_container->GetType(
				context);
		const_shared_ptr<CompoundTypeSpecifier> as_compound =
				std::dynamic_pointer_cast<const CompoundTypeSpecifier>(
						container_type);

		if (as_compound != nullptr) {
			const_shared_ptr<TypeSpecifier> variable_type = GetType(context);

			if (variable_type == PrimitiveTypeSpecifier::GetNone()) {
				errors = errors->With(
						new Error(Error::SEMANTIC, Error::UNDECLARED_MEMBER,
								m_member_variable->GetLocation().begin.line,
								m_member_variable->GetLocation().begin.column,
								*m_member_variable->GetName(),
								as_compound->GetTypeName()));
			}
		} else {
			errors = errors->With(
					new Error(Error::SEMANTIC,
							Error::VARIABLE_NOT_A_COMPOUND_TYPE,
							m_container->GetLocation().begin.line,
							m_container->GetLocation().begin.column,
							*m_container->GetName()));
		}
	} else {
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						m_container->GetLocation().begin.line,
						m_container->GetLocation().begin.column, *(GetName())));
	}

	return errors;
}

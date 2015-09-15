/*
 * member_variable.cpp
 *
 *  Created on: Aug 29, 2015
 *      Author: caleb
 */

#include <compound_type_instance.h>
#include <member_variable.h>
#include <execution_context.h>
#include <symbol_table.h>
#include <sstream>
#include <result.h>

MemberVariable::MemberVariable(const Variable* container,
		const Variable* member_variable) :
		Variable(container->GetName(), container->GetLocation()), m_container(
				container), m_member_variable(member_variable) {

}

MemberVariable::~MemberVariable() {
}

const TypeSpecifier* MemberVariable::GetType(
		const ExecutionContext* context) const {
	const Result* container_result = m_container->Evaluate(context);

	if (container_result->GetErrors() == LinkedList<const Error*>::Terminator) {
		const CompoundTypeInstance* container =
				(const CompoundTypeInstance*) container_result->GetData();
		SymbolContext* new_context = container->GetDefinition();
		const ExecutionContext* temp_context = context->WithSymbolContext(
				new_context);
		const TypeSpecifier* result = m_member_variable->GetType(temp_context);
		delete (temp_context);
		return result;
	} else {
		return PrimitiveTypeSpecifier::GetNone();
	}
}

const std::string* MemberVariable::ToString(
		const ExecutionContext* context) const {
	ostringstream buffer;
	buffer << "<" << *GetName() << "." << *m_member_variable << ">";
	return new string(buffer.str());
}

const Result* MemberVariable::Evaluate(const ExecutionContext* context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;
	const void* result_value = nullptr;

	const TypeSpecifier* container_type = m_container->GetType(context);
	const CompoundTypeSpecifier* as_compound =
			dynamic_cast<const CompoundTypeSpecifier*>(container_type);

	if (as_compound != nullptr) {
		const Result* container_result = m_container->Evaluate(context);

		errors = container_result->GetErrors();
		if (errors == LinkedList<const Error*>::Terminator) {
			const CompoundTypeInstance* instance =
					(const CompoundTypeInstance*) container_result->GetData();
			SymbolContext* new_symbol_context = instance->GetDefinition();
			const ExecutionContext* new_context = context->WithSymbolContext(
					new_symbol_context);
			const Result* member_result = m_member_variable->Evaluate(
					new_context);
			return member_result;
		}
	} else {
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::VARIABLE_NOT_A_COMPOUND_TYPE,
						GetLocation().first_line, GetLocation().first_column,
						*(GetName())));
	}

	const Result* result = new Result(result_value, errors);
	return result;
}

const LinkedList<const Error*>* MemberVariable::SetSymbol(
		const ExecutionContext* context, const bool* value) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	const Result* container_result = m_container->Evaluate(context);

	SetResult set_result = NO_SET_RESULT;

	errors = container_result->GetErrors();
	if (errors == LinkedList<const Error*>::Terminator) {
		const TypeSpecifier* container_type = m_container->GetType(context);
		const CompoundTypeSpecifier* as_compound =
				dynamic_cast<const CompoundTypeSpecifier*>(container_type);

		if (as_compound != nullptr) {
			const CompoundTypeInstance* instance =
					(const CompoundTypeInstance*) container_result->GetData();
			const string member_name = *(m_member_variable->GetName());
			set_result = instance->GetDefinition()->SetSymbol(member_name,
					value);
		} else {
			set_result = INCOMPATIBLE_TYPE;
		}
	} else {
		return errors;
	}

	return ToErrorList(set_result);
}

const LinkedList<const Error*>* MemberVariable::SetSymbol(
		const ExecutionContext* context, const int* value) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	const Result* container_result = m_container->Evaluate(context);

	SetResult set_result = NO_SET_RESULT;

	errors = container_result->GetErrors();
	if (errors == LinkedList<const Error*>::Terminator) {
		const TypeSpecifier* container_type = m_container->GetType(context);
		const CompoundTypeSpecifier* as_compound =
				dynamic_cast<const CompoundTypeSpecifier*>(container_type);

		if (as_compound != nullptr) {
			const CompoundTypeInstance* instance =
					(const CompoundTypeInstance*) container_result->GetData();
			const string member_name = *(m_member_variable->GetName());
			set_result = instance->GetDefinition()->SetSymbol(member_name,
					value);
		} else {
			set_result = INCOMPATIBLE_TYPE;
		}
	} else {
		return errors;
	}

	return ToErrorList(set_result);
}

const LinkedList<const Error*>* MemberVariable::SetSymbol(
		const ExecutionContext* context, const double* value) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	const Result* container_result = m_container->Evaluate(context);

	SetResult set_result = NO_SET_RESULT;

	errors = container_result->GetErrors();
	if (errors == LinkedList<const Error*>::Terminator) {
		const TypeSpecifier* container_type = m_container->GetType(context);
		const CompoundTypeSpecifier* as_compound =
				dynamic_cast<const CompoundTypeSpecifier*>(container_type);

		if (as_compound != nullptr) {
			const CompoundTypeInstance* instance =
					(const CompoundTypeInstance*) container_result->GetData();
			const string member_name = *(m_member_variable->GetName());
			set_result = instance->GetDefinition()->SetSymbol(member_name,
					value);
		} else {
			set_result = INCOMPATIBLE_TYPE;
		}
	} else {
		return errors;
	}

	return ToErrorList(set_result);
}

const LinkedList<const Error*>* MemberVariable::SetSymbol(
		const ExecutionContext* context, const string* value) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	const Result* container_result = m_container->Evaluate(context);

	SetResult set_result = NO_SET_RESULT;

	errors = container_result->GetErrors();
	if (errors == LinkedList<const Error*>::Terminator) {
		const TypeSpecifier* container_type = m_container->GetType(context);
		const CompoundTypeSpecifier* as_compound =
				dynamic_cast<const CompoundTypeSpecifier*>(container_type);

		if (as_compound != nullptr) {
			const CompoundTypeInstance* instance =
					(const CompoundTypeInstance*) container_result->GetData();
			const string member_name = *(m_member_variable->GetName());
			set_result = instance->GetDefinition()->SetSymbol(member_name,
					value);
		} else {
			set_result = INCOMPATIBLE_TYPE;
		}
	} else {
		return errors;
	}

	return ToErrorList(set_result);
}

const LinkedList<const Error*>* MemberVariable::SetSymbol(
		const ExecutionContext* context,
		const CompoundTypeInstance* value) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	const Result* container_result = m_container->Evaluate(context);

	SetResult set_result = NO_SET_RESULT;

	errors = container_result->GetErrors();
	if (errors == LinkedList<const Error*>::Terminator) {
		const TypeSpecifier* container_type = m_container->GetType(context);
		const CompoundTypeSpecifier* as_compound =
				dynamic_cast<const CompoundTypeSpecifier*>(container_type);

		if (as_compound != nullptr) {
			const CompoundTypeInstance* instance =
					(const CompoundTypeInstance*) container_result->GetData();
			const string member_name = *(m_member_variable->GetName());
			set_result = instance->GetDefinition()->SetSymbol(member_name,
					value);
		} else {
			set_result = INCOMPATIBLE_TYPE;
		}
	} else {
		return errors;
	}

	return ToErrorList(set_result);
}

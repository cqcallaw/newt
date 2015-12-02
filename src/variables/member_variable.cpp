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

const TypeSpecifier* MemberVariable::GetType(
		const ExecutionContext* context) const {
	const TypeSpecifier* container_type_specifier = m_container->GetType(
			context);
	const CompoundTypeSpecifier* as_compound_type =
			dynamic_cast<const CompoundTypeSpecifier*>(container_type_specifier);
	if (as_compound_type) {
		const string type_name = as_compound_type->GetTypeName();

		const CompoundTypeInstance* instance =
				static_cast<const CompoundTypeInstance*>(as_compound_type->DefaultValue(
						context->GetTypeTable()));
		ExecutionContext* new_context = context->WithSymbolContext(
				instance->GetDefinition());
		const TypeSpecifier* result = m_member_variable->GetType(new_context);
		delete instance;
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
			LinkedList<const Error*>::Terminator;
	const void* result_value = nullptr;

	const TypeSpecifier* container_type = m_container->GetType(context);
	const CompoundTypeSpecifier* as_compound =
			dynamic_cast<const CompoundTypeSpecifier*>(container_type);

	if (container_type != PrimitiveTypeSpecifier::GetNone()) {
		if (as_compound != nullptr) {
			const Result* container_result = m_container->Evaluate(context);

			errors = container_result->GetErrors();
			if (errors == LinkedList<const Error*>::Terminator) {
				const CompoundTypeInstance* instance =
						(const CompoundTypeInstance*) container_result->GetData();
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
							m_container->GetLocation().first_line,
							m_container->GetLocation().first_column,
							*(GetName())));
		}
	} else {
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						m_container->GetLocation().first_line,
						m_container->GetLocation().first_column, *(GetName())));
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

	return ToErrorList(set_result, m_member_variable->GetType(context),
			PrimitiveTypeSpecifier::GetBoolean());
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

	return ToErrorList(set_result, m_member_variable->GetType(context),
			PrimitiveTypeSpecifier::GetInt());
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

	return ToErrorList(set_result, m_member_variable->GetType(context),
			PrimitiveTypeSpecifier::GetDouble());
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

	return ToErrorList(set_result, m_member_variable->GetType(context),
			PrimitiveTypeSpecifier::GetString());
}

const LinkedList<const Error*>* MemberVariable::AssignValue(
		const ExecutionContext* context, const Expression* expression,
		const AssignmentType op) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	const Result* container_evaluation = GetContainer()->Evaluate(context);
	errors = container_evaluation->GetErrors();
	if (errors == LinkedList<const Error*>::Terminator) {
		//we're assigning a struct member reference
		const CompoundTypeInstance* struct_value =
				(const CompoundTypeInstance*) container_evaluation->GetData();
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

	return ToErrorList(set_result, m_member_variable->GetType(context),
			value->GetTypeSpecifier());
}

const LinkedList<const Error*>* MemberVariable::SetSymbol(
		const ExecutionContext* context, const Array* value) const {
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

	return ToErrorList(set_result, m_member_variable->GetType(context),
			value->GetTypeSpecifier());
}

const LinkedList<const Error*>* MemberVariable::Validate(
		const ExecutionContext* context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	const SymbolContext* symbol_context = context->GetSymbolContext();
	const Symbol* symbol = symbol_context->GetSymbol(m_container->GetName(),
			DEEP);

	if (symbol != nullptr && symbol != Symbol::DefaultSymbol) {
		const TypeSpecifier* container_type = m_container->GetType(context);
		const CompoundTypeSpecifier* as_compound =
				dynamic_cast<const CompoundTypeSpecifier*>(container_type);

		if (as_compound != nullptr) {
			const TypeSpecifier* variable_type = GetType(context);

			if (variable_type == PrimitiveTypeSpecifier::GetNone()) {
				errors = errors->With(
						new Error(Error::SEMANTIC, Error::UNDECLARED_MEMBER,
								m_member_variable->GetLocation().first_line,
								m_member_variable->GetLocation().first_column,
								*m_member_variable->GetName(),
								as_compound->GetTypeName()));
			}
		} else {
			errors = errors->With(
					new Error(Error::SEMANTIC,
							Error::VARIABLE_NOT_A_COMPOUND_TYPE,
							m_container->GetLocation().first_line,
							m_container->GetLocation().first_column,
							*m_container->GetName()));
		}
	} else {
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						m_container->GetLocation().first_line,
						m_container->GetLocation().first_column, *(GetName())));
	}

	return errors;
}

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

TypedResult<TypeSpecifier> MemberVariable::GetTypeSpecifier(
		const shared_ptr<ExecutionContext> context,
		AliasResolution resolution) const {
	shared_ptr<const TypeSpecifier> result = PrimitiveTypeSpecifier::GetNone();
	auto container_type_specifier_result = m_container->GetTypeSpecifier(
			context);

	auto type_table = *context->GetTypeTable();
	auto errors = container_type_specifier_result.GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto container_type_specifier =
				container_type_specifier_result.GetData();

		auto container_type_result = container_type_specifier->GetType(
				type_table, RESOLVE);
		errors = container_type_result->GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto container_type =
					container_type_result->GetData<TypeDefinition>();
			auto as_record = dynamic_pointer_cast<const RecordType>(
					container_type);
			auto member_name = m_member_variable->GetName();
			if (as_record) {
				auto member_type = as_record->GetDefinition()->GetType<
						TypeDefinition>(*member_name, SHALLOW, RESOLVE);

				if (member_type) {
					auto output = make_shared<NestedTypeSpecifier>(
							container_type_specifier, member_name);

					if (resolution == AliasResolution::RETURN) {
						result = output;
					} else {
						return NestedTypeSpecifier::Resolve(output, type_table);
					}
				} else {
					errors =
							ErrorList::From(
									make_shared<Error>(Error::SEMANTIC,
											Error::UNDECLARED_MEMBER,
											GetMemberVariable()->GetLocation().begin.line,
											GetMemberVariable()->GetLocation().begin.column,
											*(GetMemberVariable()->GetName()),
											container_type_specifier->ToString()),
									errors);
				}
			}

			auto as_sum = dynamic_pointer_cast<const SumType>(container_type);
			if (as_sum) {
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::CANNOT_REFERENCE_SUM_VARIANT_AS_DATA,
								GetMemberVariable()->GetLocation().begin.line,
								GetMemberVariable()->GetLocation().begin.column,
								*(GetMemberVariable()->GetName()),
								container_type_specifier->ToString()), errors);
			}
		}
	}

	// try type constructor lookup
	// here we lookup the container as a type specifier instead of a symbol specifier;
	// this is valid semantics for sum type type constructors
	auto as_sum = context->GetTypeTable()->GetType<SumType>(
			m_container->GetName(), DEEP, RESOLVE);
	if (as_sum) {
		auto constructors = as_sum->GetConstructors();
		auto constructor_symbol = constructors->GetSymbol(
				*m_member_variable->GetName());
		if (constructor_symbol != Symbol::GetDefaultSymbol()) {
			auto type_specifier = constructor_symbol->GetTypeSpecifier();
			return type_specifier;
		} else {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::UNDECLARED_MEMBER,
							GetMemberVariable()->GetLocation().begin.line,
							GetMemberVariable()->GetLocation().begin.column,
							*(GetMemberVariable()->GetName()),
							*m_container->GetName()),
					ErrorList::GetTerminator());
		}
	}

	return TypedResult<TypeSpecifier>(result, errors);
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

	auto container_type_specifier_result = m_container->GetTypeSpecifier(
			context);

	// N.B. we are explicitly ignoring any errors in the container's GetTypeSpecifier invocation
	auto container_type_specifier = container_type_specifier_result.GetData();

	if (container_type_specifier
			&& container_type_specifier != PrimitiveTypeSpecifier::GetNone()) {
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
	const_shared_ptr<Result> container_result = m_container->Evaluate(context);

	auto errors = container_result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		SetResult set_result = NO_SET_RESULT;
		auto container_type_result = m_container->GetTypeSpecifier(context);

		errors = container_type_result.GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto container_type = container_type_result.GetData();
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

			auto member_variable_type_specifier_result =
					m_member_variable->GetTypeSpecifier(context);

			errors = member_variable_type_specifier_result.GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				auto member_variable_type_specifier =
						member_variable_type_specifier_result.GetData();
				errors = ToErrorListRef(set_result,
						member_variable_type_specifier,
						PrimitiveTypeSpecifier::GetBoolean());
			}
		}
	}

	return errors;
}

const ErrorListRef MemberVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<int> value) const {
	const_shared_ptr<Result> container_result = m_container->Evaluate(context);

	auto errors = container_result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		SetResult set_result = NO_SET_RESULT;
		auto container_type_result = m_container->GetTypeSpecifier(context);

		errors = container_type_result.GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto container_type = container_type_result.GetData();
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

			auto member_variable_type_specifier_result =
					m_member_variable->GetTypeSpecifier(context);

			errors = member_variable_type_specifier_result.GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				auto member_variable_type_specifier =
						member_variable_type_specifier_result.GetData();
				errors = ToErrorListRef(set_result,
						member_variable_type_specifier,
						PrimitiveTypeSpecifier::GetInt());
			}
		}
	}

	return errors;
}

const ErrorListRef MemberVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<double> value) const {
	const_shared_ptr<Result> container_result = m_container->Evaluate(context);

	auto errors = container_result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		SetResult set_result = NO_SET_RESULT;
		auto container_type_result = m_container->GetTypeSpecifier(context);

		errors = container_type_result.GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto container_type = container_type_result.GetData();
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

			auto member_variable_type_specifier_result =
					m_member_variable->GetTypeSpecifier(context);

			errors = member_variable_type_specifier_result.GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				auto member_variable_type_specifier =
						member_variable_type_specifier_result.GetData();
				errors = ToErrorListRef(set_result,
						member_variable_type_specifier,
						PrimitiveTypeSpecifier::GetDouble());
			}
		}
	}

	return errors;
}

const ErrorListRef MemberVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<std::string> value) const {
	const_shared_ptr<Result> container_result = m_container->Evaluate(context);

	auto errors = container_result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		SetResult set_result = NO_SET_RESULT;
		auto container_type_result = m_container->GetTypeSpecifier(context);

		errors = container_type_result.GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto container_type = container_type_result.GetData();
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

			auto member_variable_type_specifier_result =
					m_member_variable->GetTypeSpecifier(context);

			errors = member_variable_type_specifier_result.GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				auto member_variable_type_specifier =
						member_variable_type_specifier_result.GetData();
				errors = ToErrorListRef(set_result,
						member_variable_type_specifier,
						PrimitiveTypeSpecifier::GetString());
			}
		}
	}

	return errors;
}

const ErrorListRef MemberVariable::AssignValue(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure,
		const_shared_ptr<Expression> expression,
		const AssignmentType op) const {
	assert(context->GetModifiers() & Modifier::MUTABLE);
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

			auto new_context = ExecutionContext::GetEmptyChild(context,
					Modifier::Type::MUTABLE, TEMPORARY);
			new_context = context->WithContents(definition);

			const_shared_ptr<Variable> new_variable = GetMemberVariable();
			errors = new_variable->AssignValue(new_context, closure, expression,
					op);
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
	const_shared_ptr<Result> container_result = m_container->Evaluate(context);

	SetResult set_result = NO_SET_RESULT;

	auto errors = container_result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto container_type_specifier_result = m_container->GetTypeSpecifier(
				context);

		errors = container_type_specifier_result.GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto container_type_specifier =
					container_type_specifier_result.GetData();
			const_shared_ptr<RecordTypeSpecifier> as_record =
					std::dynamic_pointer_cast<const RecordTypeSpecifier>(
							container_type_specifier);

			if (as_record) {
				auto instance = container_result->GetData<Record>();
				const std::string member_name = *(m_member_variable->GetName());
				set_result = instance->GetDefinition()->SetSymbol(member_name,
						type, value, context->GetTypeTable());
			} else {
				set_result = INCOMPATIBLE_TYPE;
			}
		}

		auto member_variable_type_specifier_result =
				m_member_variable->GetTypeSpecifier(context);
		errors = member_variable_type_specifier_result.GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto member_variable_type_specifier =
					member_variable_type_specifier_result.GetData();
			errors = ToErrorListRef(set_result, member_variable_type_specifier,
					type);
		}
	}

	return errors;
}

const ErrorListRef MemberVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<Array> value) const {
	const_shared_ptr<Result> container_result = m_container->Evaluate(context);

	SetResult set_result = NO_SET_RESULT;

	auto errors = container_result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto container_type_specifier_result = m_container->GetTypeSpecifier(
				context);

		errors = container_type_specifier_result.GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto container_type_specifier =
					container_type_specifier_result.GetData();
			const_shared_ptr<RecordTypeSpecifier> as_record =
					std::dynamic_pointer_cast<const RecordTypeSpecifier>(
							container_type_specifier);

			if (as_record) {
				auto instance = container_result->GetData<Record>();
				const std::string member_name = *(m_member_variable->GetName());
				set_result = instance->GetDefinition()->SetSymbol(member_name,
						value, context->GetTypeTable());
			} else {
				set_result = INCOMPATIBLE_TYPE;
			}

			auto container_type_specifier_result =
					m_container->GetTypeSpecifier(context);

			errors = container_type_specifier_result.GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				auto member_variable_type_specifier_result =
						m_member_variable->GetTypeSpecifier(context);
				errors = member_variable_type_specifier_result.GetErrors();
				if (ErrorList::IsTerminator(errors)) {
					auto member_variable_type_specifier =
							member_variable_type_specifier_result.GetData();
					errors = ToErrorListRef(set_result,
							member_variable_type_specifier,
							value->GetTypeSpecifier());
				}
			}
		}
	}

	return errors;
}

const ErrorListRef MemberVariable::Validate(
		const shared_ptr<ExecutionContext> context) const {
	auto errors = ErrorList::GetTerminator();

	auto symbol = context->GetSymbol(m_container->GetName(), DEEP);
	if (symbol && symbol != Symbol::GetDefaultSymbol()) {
		auto container_type_specifier_result = m_container->GetTypeSpecifier(
				context);

		errors = container_type_specifier_result.GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto container_type_specifier =
					container_type_specifier_result.GetData();

			auto container_type_result = container_type_specifier->GetType(
					context->GetTypeTable(), RESOLVE);

			errors = container_type_result->GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				auto type = container_type_result->GetData<TypeDefinition>();
				auto as_complex = dynamic_pointer_cast<const ComplexType>(type);
				if (as_complex) {
					auto member_definition =
							as_complex->GetDefinition()->GetType<TypeDefinition>(
									m_member_variable->GetName(), SHALLOW,
									RESOLVE);
					if (!member_definition) {
						errors =
								ErrorList::From(
										make_shared<Error>(Error::SEMANTIC,
												Error::UNDECLARED_MEMBER,
												m_member_variable->GetLocation().begin.line,
												m_member_variable->GetLocation().begin.column,
												*m_member_variable->ToString(
														context),
												container_type_specifier->ToString()),
										errors);
					}
				} else {
					errors = ErrorList::From(
							make_shared<Error>(Error::SEMANTIC,
									Error::NOT_A_COMPOUND_TYPE,
									m_container->GetLocation().begin.line,
									m_container->GetLocation().begin.column,
									*m_container->ToString(context)), errors);
				}
			}
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

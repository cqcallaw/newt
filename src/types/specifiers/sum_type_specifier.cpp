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

#include <complex_instantiation_statement.h>
#include <sum_type_specifier.h>
#include <sum.h>
#include <complex_type_specifier.h>
#include <maybe_type_specifier.h>
#include <sum_type.h>

SumTypeSpecifier::SumTypeSpecifier(const ComplexTypeSpecifier& complex) :
		SumTypeSpecifier(complex.GetTypeName(), complex.GetNamespace()) {
}

SumTypeSpecifier::SumTypeSpecifier(
		const_shared_ptr<ComplexTypeSpecifier> complex) :
		SumTypeSpecifier(complex->GetTypeName(), complex->GetNamespace()) {
}

const AnalysisResult SumTypeSpecifier::AnalyzeAssignmentTo(
		const_shared_ptr<TypeSpecifier> other,
		const TypeTable& type_table) const {
	auto complex_result = ComplexTypeSpecifier::AnalyzeAssignmentTo(other,
			type_table);
	if (complex_result) {
		return complex_result;
	}

	auto other_type = other->GetType(type_table, RESOLVE);
	auto other_type_as_sum = dynamic_pointer_cast<const SumType>(other_type);
	if (other_type_as_sum) {
		auto other_specifier_as_complex = dynamic_pointer_cast<
				const ComplexTypeSpecifier>(other);
		if (other_specifier_as_complex) {
			auto analysis = other_type_as_sum->AnalyzeConversion(
					*other_specifier_as_complex, *this);
			return analysis;
		}

		auto other_specifier_as_maybe = dynamic_pointer_cast<
				const MaybeTypeSpecifier>(other);
		if (other_specifier_as_maybe) {
			return AnalyzeAssignmentTo(other_specifier_as_maybe->GetTypeSpecifier(),
					type_table);
		}
	}

	return AnalysisResult::INCOMPATIBLE;
}

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

#ifndef DEFAULTS_H_
#define DEFAULTS_H_

#include <string>
#include "yyltype.h"

class Symbol;
class Variable;
class ForStatement;
class Expression;
class AssignmentStatement;
class IfStatement;
class ExitStatement;
class PrintStatement;
class StatementBlock;

extern const YYLTYPE DefaultLocation;
/*extern const Variable* DefaultVariable;
extern const Expression* DefaultExpression;
extern const AssignmentStatement* DefaultAssignmentStatement;
extern const ForStatement* DefaultForStatement;
extern const ExitStatement* DefaultExitStatement;
extern const IfStatement* DefaultIfStatement;
extern const PrintStatement* DefaultPrintStatement;
extern const StatementBlock* DefaultStatementBlock;*/

#endif /* DEFAULTS_H_ */

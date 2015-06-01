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

#include <assignment_statement.h>
#include <defaults.h>
#include <symbol.h>
#include <expression.h>
#include <for_statement.h>
#include <if_statement.h>
#include <exit_statement.h>
#include <print_statement.h>
#include <statement_block.h>
#include <statement_list.h>
#include <variable.h>
#include "type.h"

const YYLTYPE DefaultLocation = YYLTYPE { -1, -1, -1, -1 };

const Variable* DefaultVariable = new Variable(&(Symbol::DefaultSymbolName),
		DefaultLocation);

const Expression* DefaultExpression = new Expression(NONE);

const AssignmentStatement* DefaultAssignmentStatement = new AssignmentStatement(
		DefaultVariable, AssignmentStatement::ASSIGN, DefaultExpression);

const ExitStatement* DefaultExitStatement = new ExitStatement(-1,
		DefaultExpression);

const StatementBlock* DefaultStatementBlock = new StatementBlock(
		StatementList::Terminator);

const IfStatement* DefaultIfStatement = new IfStatement(DefaultExpression,
		DefaultStatementBlock);

const ForStatement* DefaultForStatement = new ForStatement(
		DefaultAssignmentStatement, DefaultExpression,
		DefaultAssignmentStatement, DefaultStatementBlock);

const PrintStatement* DefaultPrintStatement = new PrintStatement(-1,
		DefaultExpression);

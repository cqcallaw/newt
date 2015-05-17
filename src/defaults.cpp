/*
 * defaults.cpp
 *
 *  Created on: May 15, 2015
 *      Author: caleb
 */

#include <defaults.h>
#include <symbol.h>
#include <expression.h>
#include <assignment_statement.h>
#include <for_statement.h>
#include <if_statement.h>
#include <exit_statement.h>
#include <print_statement.h>
#include <statement_block.h>
#include "type.h"

const string DefaultSymbolName = string("[!!_DEFAULT_!!]");
const Symbol* DefaultSymbol = new Symbol(NONE, DefaultSymbolName, NULL);

const Variable* DefaultVariable = new Variable(&DefaultSymbolName);

const Expression* DefaultExpression = new Expression(NONE);

const AssignmentStatement* DefaultAssignmentStatement = new AssignmentStatement(
		DefaultVariable, AssignmentStatement::ASSIGN, DefaultExpression);

const ExitStatement* DefaultExitStatement = new ExitStatement(-1,
		DefaultExpression);

const StatementBlock* DefaultStatementBlock = new StatementBlock(
		StatementList::Terminator);

const IfStatement* DefaultIfStatement = new IfStatement(DefaultExpression,
		DefaultStatementBlock);

const static ForStatement* DefaultForStatement = new ForStatement(
		DefaultAssignmentStatement, DefaultExpression,
		DefaultAssignmentStatement, DefaultStatementBlock);

const PrintStatement* DefaultPrintStatement = new PrintStatement(-1,
		DefaultExpression);

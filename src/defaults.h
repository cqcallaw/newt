/*
 * defaults.h
 *
 * This file exists so that default values are collected into a single compilation unit,
 * so that the order in which they are initialized can be well-defined.
 */

#ifndef DEFAULTS_H_
#define DEFAULTS_H_

class ForStatement;
class Expression;
class AssignmentStatement;
class IfStatement;
class ExitStatement;
class PrintStatement;
class StatementBlock;

const static Expression* DefaultExpression;
const static AssignmentStatement* DefaultAssignmentStatement;
const static ForStatement* DefaultForStatement;
const static ExitStatement* DefaultExitStatement;
const static IfStatement* DefaultIfStatement;
const static PrintStatement* DefaultPrintStatement;
const static StatementBlock* DefaultStatementBlock;

//suppress un-used variable compiler warnings
//references to the defaults are generated from the Bison file,
//and these references are not detected by gcc
const static void* ref_expr = (void*) (DefaultExpression);
const static void* ref_assignment = (void*) (DefaultAssignmentStatement);
const static void* ref_exit = (void*) (DefaultExitStatement);
const static void* ref_for = (void*) (DefaultForStatement);
const static void* ref_if = (void*) (DefaultIfStatement);
const static void* ref_print = (void*) (DefaultPrintStatement);
const static void* ref_statement = (void*) (DefaultStatementBlock);

#endif /* DEFAULTS_H_ */

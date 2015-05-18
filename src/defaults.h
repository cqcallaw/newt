/*
 * defaults.h
 *
 * This file exists so that default values are collected into a single compilation unit,
 * so that the order in which they are initialized can be well-defined.
 */

#ifndef DEFAULTS_H_
#define DEFAULTS_H_

#include <string>

class Symbol;
class Variable;
class ForStatement;
class Expression;
class AssignmentStatement;
class IfStatement;
class ExitStatement;
class PrintStatement;
class StatementBlock;

extern const Variable* DefaultVariable;
extern const Expression* DefaultExpression;
extern const AssignmentStatement* DefaultAssignmentStatement;
extern const ForStatement* DefaultForStatement;
extern const ExitStatement* DefaultExitStatement;
extern const IfStatement* DefaultIfStatement;
extern const PrintStatement* DefaultPrintStatement;
extern const StatementBlock* DefaultStatementBlock;

#endif /* DEFAULTS_H_ */

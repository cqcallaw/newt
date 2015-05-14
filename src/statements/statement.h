/*
 * statement.h
 *
 *  Created on: Mar 20, 2015
 *      Author: caleb
 */

#ifndef STATEMENT_H_
#define STATEMENT_H_

class Statement {
public:
	Statement();
	virtual ~Statement();

	virtual void execute() const = 0;
};

#endif /* STATEMENT_H_ */

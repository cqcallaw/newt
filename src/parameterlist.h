/*
 * parameterList.h
 *
 *  Created on: Mar 9, 2015
 *      Author: caleb
 */

#ifndef PARAMETERLIST_H_
#define PARAMETERLIST_H_

#include "linked_list.h"

class Parameter;

class ParameterList: public LinkedList<const Parameter*> {
public:
	ParameterList(const Parameter* data, const ParameterList* next) :
			LinkedList(data, next) {
	}

	ParameterList(const Parameter* data) :
			ParameterList(data, ParameterList::Terminator) {
	}

	static const ParameterList* Terminator;
};

#endif /* PARAMETERLIST_H_ */

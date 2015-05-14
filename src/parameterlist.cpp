/*
 * parameterList.cpp
 *
 *  Created on: Mar 9, 2015
 *      Author: caleb
 */

#include "parameterlist.h"

const ParameterList* ParameterList::Terminator = new ParameterList(nullptr,
		nullptr);

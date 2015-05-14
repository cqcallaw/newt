/*
 * utils.h
 *
 *  Created on: Feb 20, 2015
 *      Author: caleb
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <string>

using namespace std;

string* AsString(bool value);

string* AsString(int value);

string* AsString(double value);

string* AsString(bool* value);

string* AsString(int* value);

string* AsString(double* value);

bool EndsWith(std::string const &fullString, std::string const &ending);
#endif /* UTILS_H_ */

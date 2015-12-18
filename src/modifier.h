/*
 * modifier.h
 *
 *  Created on: Aug 28, 2015
 *      Author: caleb
 */

#ifndef MODIFIER_H_
#define MODIFIER_H_

#include <defaults.h>

class Modifier {
public:
	enum Type {
		NONE = 0, READONLY = 1
	};

	Modifier(const Type type, const YYLTYPE type_position) :
			m_type(type), m_type_position(type_position) {

	}

	const Type GetType() const {
		return m_type;
	}

	const YYLTYPE GetTypePosition() const {
		return m_type_position;
	}

private:
	const Type m_type;
	const YYLTYPE m_type_position;
};

typedef LinkedList<const Modifier*> ModifierList;

#endif /* MODIFIER_H_ */

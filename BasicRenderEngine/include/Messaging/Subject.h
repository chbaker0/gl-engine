/*
 * Subject.h
 *
 *  Created on: Mar 15, 2015
 *      Author: Collin
 */

#ifndef MESSAGING_SUBJECT_H_INCLUDED
#define MESSAGING_SUBJECT_H_INCLUDED

#include <vector>

#include "Listener.h"

class Subject
{
protected:
	std::vector<Listener*> listeners;

	~Subject() {}

public:
	virtual void registerListener(Listener*);
	virtual void unregisterListener(Listener*);
};

#endif /* MESSAGING_SUBJECT_H_INCLUDED */

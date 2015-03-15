/*
 * Listener.h
 *
 *  Created on: Mar 15, 2015
 *      Author: Collin
 */

#ifndef MESSAGING_LISTENER_H_INCLUDED
#define MESSAGING_LISTENER_H_INCLUDED

#include "Message.h"

class Listener
{
public:
	virtual ~Listener() {}
	virtual void acceptMessage(Message&) = 0;
};

#endif /* MESSAGING_LISTENER_H_INCLUDED */

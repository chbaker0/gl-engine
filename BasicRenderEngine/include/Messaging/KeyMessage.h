/*
 * KeyMessage.h
 *
 *  Created on: Mar 31, 2015
 *      Author: Collin
 */

#ifndef MESSAGING_KEY_MESSAGE_H_INCLUDED
#define MESSAGING_KEY_MESSAGE_H_INCLUDED

#include "Message.h"
#include "MessageTypes.h"

class KeyMessage : Message
{
private:
	unsigned int key;

public:
	KeyMessage(Subject *subject): Message(MessageType::KeyboardInput, subject) {}

	unsigned int getKey() const noexcept {return key;}

	static void* operator new(std::size_t);
	static void operator delete(void*);
};

#endif /* MESSAGING_KEY_MESSAGE_H_INCLUDED */

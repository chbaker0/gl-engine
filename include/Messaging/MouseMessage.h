/*
 * MouseMessage.h
 *
 *  Created on: Apr 1, 2015
 *      Author: Collin
 */

#ifndef MESSAGING_MOUSE_MESSAGE_H_INCLUDED
#define MESSAGING_MOUSE_MESSAGE_H_INCLUDED

#include <glm/glm.hpp>

#include "Message.h"
#include "MessageTypes.h"

class MouseMessage : public Message
{
private:
	glm::vec2 position, change;

public:
	MouseMessage(Subject *subject_in, float newX, float newY,
	             float dX, float dY):
	            	 Message(MessageType::MouseInput, subject_in),
	            	 position(newX, newY), change(dX, dY) {}

	glm::vec2 getPosition() const noexcept
	{
		return position;
	}
	glm::vec2 getChange() const noexcept
	{
		return change;
	}

	static void* operator new(std::size_t);
	static void operator delete(void*);
};

#endif /* MESSAGING_MOUSE_MESSAGE_H_INCLUDED */

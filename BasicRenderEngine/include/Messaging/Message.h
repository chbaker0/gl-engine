/*
 * Message.h
 *
 *  Created on: Mar 15, 2015
 *      Author: Collin
 */

#ifndef MESSAGING_MESSAGE_H_INCLUDED
#define MESSAGING_MESSAGE_H_INCLUDED

enum class MessageType : unsigned long;

class Subject;

class Message
{
protected:
	MessageType type;
	Subject *subject;

public:
	Message(MessageType type_in, Subject *subject_in): type(type_in), subject(subject_in) {}
	virtual ~Message() {}

	virtual MessageType getType() const noexcept
	{
		return type;
	}
	Subject* getSubject() const noexcept
	{
		return subject;
	}
};

#endif /* MESSAGING_MESSAGE_H_INCLUDED */

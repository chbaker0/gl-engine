#include <iostream>
#include <stdexcept>

#include <boost/pool/object_pool.hpp>

#include "Messaging/Message.h"
#include "Messaging/KeyMessage.h"
#include "Messaging/MouseMessage.h"

static boost::object_pool<Message> messagePool;

void* Message::operator new(std::size_t s)
{
//	if(s != sizeof(Message))
//		throw std::logic_error("Attempted to allocate a different object with Message operator new overload! All subclasses of Message should have an operator new overload");
//	std::cout << "Allocating Message" << std::endl;
	return messagePool.malloc();
}

void Message::operator delete(void *ptr)
{
	messagePool.free(static_cast<Message*>(ptr));
//	std::cout << "Freed Message" << std::endl;
}

static boost::object_pool<KeyMessage> keyMessagePool;

void* KeyMessage::operator new(std::size_t s)
{
//	if(s != sizeof(KeyMessage))
//		throw std::logic_error("Attempted to allocate a different object with KeyMessage operator new overload! All subclasses of KeyMessage should have an operator new overload");
//	std::cout << "Allocating KeyMessage" << std::endl;
	return keyMessagePool.malloc();
}

void KeyMessage::operator delete(void *ptr)
{
	keyMessagePool.free(static_cast<KeyMessage*>(ptr));
//	std::cout << "Freed KeyMessage" << std::endl;
}

static boost::object_pool<MouseMessage> mouseMessagePool;

void* MouseMessage::operator new(std::size_t s)
{
//	if(s != sizeof(MouseMessage))
//		throw std::logic_error("Attempted to allocate a different object with KeyMessage operator new overload! All subclasses of KeyMessage should have an operator new overload");
//	std::cout << "Allocating MouseMessage" << std::endl;
	return mouseMessagePool.malloc();
}

void MouseMessage::operator delete(void *ptr)
{
	mouseMessagePool.free(static_cast<MouseMessage*>(ptr));
//	std::cout << "Freed MouseMessage" << std::endl;
}

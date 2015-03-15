/*
 * Subject.cpp
 *
 *  Created on: Mar 15, 2015
 *      Author: Collin
 */

#include <algorithm>

#include "Messaging/Subject.h"

void Subject::registerListener(Listener *listener)
{
	listeners.push_back(listener);
}

void Subject::unregisterListener(Listener *listener)
{
	auto newEnd = std::remove(listeners.begin(), listeners.end(), listener);
	listeners.erase(newEnd, listeners.end());
}

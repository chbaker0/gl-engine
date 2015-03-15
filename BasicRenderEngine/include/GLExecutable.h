/*
 * GLExecutable.h
 *
 *  Created on: Mar 9, 2015
 *      Author: Collin
 */

#ifndef GL_EXECUTABLE_H_INCLUDED
#define GL_EXECUTABLE_H_INCLUDED

class GLExecutable
{
protected:
	static thread_local GLExecutable *activeExecutable;

public:
	virtual ~GLExecutable() = 0;

	virtual void use() noexcept = 0;

	static GLExecutable *getActiveExecutable() noexcept
	{
		return activeExecutable;
	}
};

inline GLExecutable::~GLExecutable() {}

#endif /* GL_EXECUTABLE_H_INCLUDED */

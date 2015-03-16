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
public:
	virtual ~GLExecutable() = 0;

	virtual void use() noexcept = 0;
};

inline GLExecutable::~GLExecutable() {}

#endif /* GL_EXECUTABLE_H_INCLUDED */

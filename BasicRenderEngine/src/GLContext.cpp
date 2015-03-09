/*
 * GLContext.cpp
 *
 *  Created on: Mar 8, 2015
 *      Author: Collin
 */

#include <iostream>

#include "GLContext.h"

std::unique_ptr<GLMutableBuffer> GLContext::getMutableBuffer(GLenum target, GLenum usage, GLsizeiptr size, void *data)
{
	using std::cout; using std::endl;
	GLuint handle;
	GLBufferTargetSaver s(target);
	glGenBuffers(1, &handle);
	glBindBuffer(target, handle);
	glBufferData(target, size, data, usage);
	return std::unique_ptr<GLMutableBuffer>(new GLMutableBuffer(target, handle, size, usage));
}

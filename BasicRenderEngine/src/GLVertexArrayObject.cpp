/*
 * GLVertexArrayObject.cpp
 *
 *  Created on: Mar 8, 2015
 *      Author: Collin
 */

#include "GLVertexArrayObject.h"

GLVertexArrayObject::~GLVertexArrayObject()
{
	glDeleteVertexArrays(1, &handle);
}

void GLVertexArrayObject::setAttrib(GLuint i, GLBuffer& buf, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *offset) noexcept
{
	glBindVertexArray(handle);
	glEnableVertexAttribArray(i);
	buf.bindTo(GL_ARRAY_BUFFER);
	glVertexAttribPointer(i, size, type, normalized, stride, offset);
}

void GLVertexArrayObject::disableAttrib(GLuint i) noexcept
{
	glBindVertexArray(handle);
	glDisableVertexAttribArray(i);
}

void GLVertexArrayObject::setElementArrayBinding(GLBuffer& buf) noexcept
{
	bind();
	buf.bindTo(GL_ELEMENT_ARRAY_BUFFER);
	unBind();
}

void GLVertexArrayObject::bind() noexcept
{
	glBindVertexArray(handle);
}

void GLVertexArrayObject::unBind() noexcept
{
	glBindVertexArray(0);
}

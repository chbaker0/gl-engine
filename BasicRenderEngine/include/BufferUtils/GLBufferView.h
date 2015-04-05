/*
 * GLBufferView.h
 *
 *  Created on: Apr 5, 2015
 *      Author: Collin
 */

#ifndef BUFFER_UTILS_GL_BUFFER_VIEW_H_INCLUDED
#define BUFFER_UTILS_GL_BUFFER_VIEW_H_INCLUDED

#include <GL/glew.h>

#include "GLBuffer.h"

class GLBufferView
{
protected:
	GLBuffer *buffer;
	GLsizeiptr offset, size;

public:
	GLBufferView(GLBuffer *buffer_in, GLsizeiptr offset_in, GLsizeiptr size_in):
		buffer(buffer_in), offset(offset_in), size(size_in) {}

	GLBuffer& getBuffer() noexcept {return *buffer;}
	const GLBuffer& getBuffer() const noexcept {return *buffer;}

	void bindBase(GLUniformBlockBinding index) noexcept
	{
		buffer->bindRange(index, offset, size);
	}
	void bindRange(GLUniformBlockBinding index, GLintptr viewOffset, GLsizeiptr size) noexcept
	{
		buffer->bindRange(index, offset + viewOffset, size);
	}

	void copy(GLBuffer& other, GLintptr roffset, GLintptr woffset, GLsizeiptr size) const noexcept
	{
		buffer->copy(other, roffset + offset, woffset, size);
	}
	void copy(GLBufferView& other, GLintptr roffset, GLintptr woffset, GLsizeiptr size) const noexcept
	{
		buffer->copy(*other.buffer, roffset + offset, woffset + other.offset, size);
	}
};

class GLMappableBufferView : public GLBufferView
{
public:
	GLMappableBufferView(GLMappableBuffer *buffer_in, GLsizeiptr offset_in, GLsizeiptr size_in):
		GLBufferView(buffer_in, offset_in, size_in) {}

	bool isReadable() const noexcept
	{
		return static_cast<GLMappableBuffer*>(buffer)->isReadable();
	}
	bool isWritable() const noexcept
	{
		return static_cast<GLMappableBuffer*>(buffer)->isWritable();
	}
	bool isPersistent() const noexcept
	{
		return static_cast<GLMappableBuffer*>(buffer)->isPersistent();
	}
	bool isCoherent() const noexcept
	{
		return static_cast<GLMappableBuffer*>(buffer)->isCoherent();
	}

	void* mapRangeRaw(GLintptr viewOffset, GLsizeiptr size, GLbitfield access)
	{
		return static_cast<GLMappableBuffer*>(buffer)->mapRangeRaw(offset + viewOffset, size, access);
	}
	GLBufferMapping mapRange(GLintptr viewOffset, GLsizeiptr size, GLbitfield access)
	{
		return static_cast<GLMappableBuffer*>(buffer)->mapRange(offset + viewOffset, size, access);
	}
	void flushRange(GLintptr viewOffset, GLsizeiptr size) noexcept
	{
		static_cast<GLMappableBuffer*>(buffer)->flushRange(offset + viewOffset, size);
	}
};

#endif /* BUFFER_UTILS_GL_BUFFER_VIEW_H_INCLUDED */

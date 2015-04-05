/*
 * GLBufferView.h
 *
 *  Created on: Apr 5, 2015
 *      Author: Collin
 */

#ifndef BUFFER_UTILS_GL_BUFFER_VIEW_H_INCLUDED
#define BUFFER_UTILS_GL_BUFFER_VIEW_H_INCLUDED

#include <memory>

#include <GL/glew.h>

#include "GLBuffer.h"

template <typename GLBufferPtr>
class GLBufferViewImpl
{
protected:
	GLBufferPtr buffer;
	GLsizeiptr offset, size;

public:
	GLBufferViewImpl(GLBufferPtr buffer_in, GLsizeiptr offset_in, GLsizeiptr size_in):
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
	void copy(GLBufferViewImpl& other, GLintptr roffset, GLintptr woffset, GLsizeiptr size) const noexcept
	{
		buffer->copy(*other.buffer, roffset + offset, woffset + other.offset, size);
	}
};

using GLBufferView = GLBufferViewImpl<GLBuffer*>;
using GLBufferSharedView = GLBufferViewImpl<std::shared_ptr<GLBuffer>>;

extern template class GLBufferViewImpl<GLBuffer*>;
extern template class GLBufferViewImpl<std::shared_ptr<GLBuffer*>>;

template <typename GLMappableBufferPtr>
class GLMappableBufferViewImpl : public GLBufferViewImpl<GLMappableBufferPtr>
{
	using GLBufferViewImpl<GLMappableBufferPtr>::buffer;
	using GLBufferViewImpl<GLMappableBufferPtr>::offset;
public:
	GLMappableBufferViewImpl(GLMappableBufferPtr buffer_in, GLsizeiptr offset_in, GLsizeiptr size_in):
		GLBufferViewImpl<GLMappableBufferPtr>(buffer_in, offset_in, size_in) {}

	bool isReadable() const noexcept
	{
		return buffer->isReadable();
	}
	bool isWritable() const noexcept
	{
		return buffer->isWritable();
	}
	bool isPersistent() const noexcept
	{
		return buffer->isPersistent();
	}
	bool isCoherent() const noexcept
	{
		return buffer->isCoherent();
	}

	void* mapRangeRaw(GLintptr viewOffset, GLsizeiptr size, GLbitfield access)
	{
		return buffer->mapRangeRaw(offset + viewOffset, size, access);
	}
	GLBufferMapping mapRange(GLintptr viewOffset, GLsizeiptr size, GLbitfield access)
	{
		return buffer->mapRange(offset + viewOffset, size, access);
	}
	void flushRange(GLintptr viewOffset, GLsizeiptr size) noexcept
	{
		buffer->flushRange(offset + viewOffset, size);
	}
};

using GLMappableBufferView = GLMappableBufferViewImpl<GLMappableBuffer*>;
using GLMappableBufferSharedView = GLMappableBufferViewImpl<std::shared_ptr<GLMappableBuffer>>;

extern template class GLMappableBufferViewImpl<GLMappableBuffer*>;
extern template class GLMappableBufferViewImpl<std::shared_ptr<GLMappableBuffer>>;

#endif /* BUFFER_UTILS_GL_BUFFER_VIEW_H_INCLUDED */

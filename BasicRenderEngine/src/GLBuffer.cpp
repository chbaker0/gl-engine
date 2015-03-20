/*
 * GLBuffer.cpp
 *
 *  Created on: Mar 7, 2015
 *      Author: Collin
 */

#include <stdexcept>

#include "GLBuffer.h"

GLBufferTargetSaver::GLBufferTargetSaver(GLenum target_in)
{
	target = target_in;
	GLenum pname = GL_INVALID_ENUM;
	switch(target)
	{
	case GL_ARRAY_BUFFER:
		pname = GL_ARRAY_BUFFER_BINDING;
		break;
	case GL_ELEMENT_ARRAY_BUFFER:
		pname = GL_ELEMENT_ARRAY_BUFFER_BINDING;
		break;
	case GL_UNIFORM_BUFFER:
		pname = GL_UNIFORM_BUFFER_BINDING;
		break;
	default:
		throw std::logic_error("Invalid target passed to GLBufferTargetSaver constructor");
		break;
	}
	GLint temp;
	glGetIntegerv(pname, &temp);
	handle = temp;
}

GLBufferTargetSaver::~GLBufferTargetSaver()
{
	glBindBuffer(target, handle);
}

GLBuffer::~GLBuffer()
{
	glDeleteBuffers(1, &handle);
}

void GLBuffer::getData(int64_t offset, uint64_t size, void *data) const noexcept
{
	glBindBuffer(target, handle);
	glGetBufferSubData(target, offset, size, data);
}

void GLBuffer::copy(GLBuffer& other, int64_t roffset, int64_t woffset, uint64_t size) const noexcept
{
	glBindBuffer(GL_COPY_READ_BUFFER, handle);
	glBindBuffer(GL_COPY_WRITE_BUFFER, other.handle);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, roffset, woffset, size);
}

void GLBuffer::bind() noexcept
{
	glBindBuffer(target, handle);
}
void GLBuffer::bindTo(GLenum target) noexcept
{
	glBindBuffer(target, handle);
}
void GLBuffer::bindBase(GLUniformBlockBinding index) noexcept
{
	glBindBufferBase(target, (GLuint)index, handle);
}
void GLBuffer::bindRange(GLUniformBlockBinding index, GLintptr offset, GLsizeiptr size) noexcept
{
	glBindBufferRange(target, (GLuint)index, handle, offset, size);
}
void GLBuffer::unBind() noexcept
{
	glBindBuffer(target, 0);
}
void GLBuffer::unBindFrom(GLenum target) noexcept
{
	glBindBuffer(target, 0);
}

void* GLMappableBuffer::mapRange(int64_t offset, uint64_t size, GLbitfield access)
{
	glBindBuffer(target, handle);
	void *ptr = glMapBufferRange(target, offset, size, access);
	if(ptr == nullptr)
		throw GLBufferMapException();
	return ptr;
}

void GLMappableBuffer::flushRange(int64_t offset, uint64_t size) noexcept
{
	glBindBuffer(target, handle);
	glFlushMappedBufferRange(target, offset, size);
}

void GLMappableBuffer::unmap()
{
	glBindBuffer(GL_COPY_READ_BUFFER, handle);
	if(glUnmapBuffer(GL_COPY_READ_BUFFER) != GL_TRUE)
		throw GLBufferCorruptionException();
}


void GLMutableBuffer::reallocate(GLsizeiptr newSize, const GLvoid *newData, GLenum newUsage)
{
	glBindBuffer(target, handle);
	glBufferData(target, newSize, newData, newUsage);
	size = newSize;
}

void GLMutableBuffer::invalidate() noexcept
{
	reallocate(size, nullptr, usage);
}

/*
 * GLBuffer.h
 *
 *  Created on: Mar 5, 2015
 *      Author: Collin
 */

#ifndef GL_BUFFER_H_INCLUDED
#define GL_BUFFER_H_INCLUDED

#include <exception>
#include <cstdint>
#include <GL/glew.h>

class GLBuffer
{
protected:
	GLuint handle;
	GLenum target;
	GLsizeiptr size;

public:
	GLBuffer(GLenum target_in, GLuint handle_in, GLsizeiptr size_in) noexcept: target(target_in), handle(handle_in), size(size_in) {}
	virtual ~GLBuffer();

	GLenum getTarget() const noexcept
	{
		return target;
	}
	void setTarget(GLenum newTarget) noexcept
	{
		target = newTarget;
	}
	GLuint getHandle() const noexcept
	{
		return handle;
	}
	GLsizeiptr getSize() const noexcept
	{
		return size;
	}
	virtual void bind() noexcept;
	virtual void bindTo(GLenum target) noexcept;
	virtual void unBind() noexcept;
	virtual void unBindFrom(GLenum target) noexcept;
	virtual void invalidate() noexcept = 0;
	virtual void copy(GLBuffer& other, int64_t roffset, int64_t woffset, uint64_t size) const noexcept;
	virtual void getData(int64_t offset, uint64_t size, void *data) const noexcept;
};

class GLBufferTargetSaver
{
private:
	GLenum target;
	GLuint handle;
public:
	GLBufferTargetSaver(GLenum target_in) noexcept;
	GLBufferTargetSaver(const GLBuffer& targetSource) noexcept: GLBufferTargetSaver(targetSource.getTarget()) {}
	GLBufferTargetSaver(const GLBufferTargetSaver&) = delete;
	~GLBufferTargetSaver();
};

class GLMappableBuffer : public GLBuffer
{
public:
	using GLBuffer::GLBuffer;
	virtual ~GLMappableBuffer() {}

	virtual bool isReadable() const noexcept = 0;
	virtual bool isWritable() const noexcept = 0;
	virtual bool isPersistent() const noexcept = 0;
	virtual bool isCoherent() const noexcept = 0;
	virtual void* mapRange(int64_t offset, uint64_t size, GLbitfield access);
	virtual void flushRange(int64_t offset, uint64_t size) noexcept;
	virtual void unmap();
};

class GLBufferMapException : public std::exception
{
public:
	GLBufferMapException() {}
	const char* what() const noexcept override
	{
		return "Error attempting to bind buffer";
	}
};

class GLBufferCorruptionException : public std::exception
{
public:
	GLBufferCorruptionException() {}
	const char* what() const noexcept override
	{
		return "Contents of buffer were corrupted while mapped";
	}
};

class GLMutableBuffer : public GLMappableBuffer
{
protected:
	GLenum usage;

public:
	GLMutableBuffer(GLenum target_in, GLuint handle_in, GLsizeiptr size_in, GLenum usage_in): GLMappableBuffer(target_in, handle_in, size_in), usage(usage_in) {}
	virtual ~GLMutableBuffer() {}

	GLenum getUsage() const noexcept
	{
		return usage;
	}
	virtual bool isReadable() const noexcept override
	{
		return true;
	}
	virtual bool isWritable() const noexcept override
	{
		return true;
	}
	virtual bool isPersistent() const noexcept override
	{
		return false;
	}
	virtual bool isCoherent() const noexcept override
	{
		return false;
	}
	virtual void reallocate(GLsizeiptr newSize, const GLvoid *newData, GLenum newUsage);
	virtual void invalidate() noexcept override;
};

#endif /* GL_BUFFER_H_INCLUDED */

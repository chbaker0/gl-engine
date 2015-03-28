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

#include "GLBlockBinding.h"

class GLBuffer
{
protected:
	GLenum target;
	GLuint handle;
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
	virtual void bindBase(GLUniformBlockBinding index) noexcept;
	virtual void bindRange(GLUniformBlockBinding index, GLintptr offset, GLsizeiptr size) noexcept;
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
	GLBufferTargetSaver(GLenum target_in);
	GLBufferTargetSaver(const GLBuffer& targetSource): GLBufferTargetSaver(targetSource.getTarget()) {}
	GLBufferTargetSaver(const GLBufferTargetSaver&) = delete;
	~GLBufferTargetSaver();
};

class GLBufferMapping;

class GLMappableBuffer : public GLBuffer
{
public:
	using GLBuffer::GLBuffer;
	virtual ~GLMappableBuffer() {}

	virtual bool isReadable() const noexcept = 0;
	virtual bool isWritable() const noexcept = 0;
	virtual bool isPersistent() const noexcept = 0;
	virtual bool isCoherent() const noexcept = 0;
	virtual void* mapRangeRaw(int64_t offset, uint64_t size, GLbitfield access);
	virtual GLBufferMapping mapRange(int64_t offset, uint64_t size, GLbitfield access);
	virtual void flushRange(int64_t offset, uint64_t size) noexcept;
	virtual void unmap();
};

class GLBufferMapping
{
private:
	GLMappableBuffer& buf;
	void *ptr;
	int64_t offset;
	uint64_t size;
	GLbitfield access;

public:
	GLBufferMapping(GLMappableBuffer& buf_in, void *ptr_in, int64_t offset_in, uint64_t size_in, GLbitfield access_in) noexcept:
		buf(buf_in), ptr(ptr_in), offset(offset_in), size(size_in), access(access_in) {}
	~GLBufferMapping()
	{
		buf.unmap();
	}

	void* getMapPtr() noexcept {return ptr;}
	const void* getMapPtr() const noexcept {return ptr;}
	int64_t getMapOffset() const noexcept {return offset;}
	uint64_t getMapSize() const noexcept {return size;}
	GLbitfield getMapAccessFlags() const noexcept {return access;}

	unsigned char& operator[](std::size_t i) noexcept
	{
		return static_cast<unsigned char*>(ptr)[i];
	}
	const unsigned char& operator[](std::size_t i) const noexcept
	{
		return static_cast<unsigned char*>(ptr)[i];
	}
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

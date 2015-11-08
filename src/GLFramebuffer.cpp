/*
 * GLFramebuffer.cpp
 *
 *  Created on: Mar 28, 2015
 *      Author: Collin
 */

#include <stdexcept>
#include "GLFramebuffer.h"

GLFramebufferTargetSaver::GLFramebufferTargetSaver(GLenum target_in): target(target_in)
{
	GLenum pname;
	switch(target)
	{
	case GL_FRAMEBUFFER:
	case GL_DRAW_FRAMEBUFFER:
		pname = GL_DRAW_FRAMEBUFFER_BINDING;
		break;
	case GL_READ_FRAMEBUFFER:
		pname = GL_READ_FRAMEBUFFER_BINDING;
		break;
	default:
		throw std::logic_error("Invalid framebuffer target passed to GLFramebufferTargetSaver constructor");
		break;
	}
	GLint temp;
	glGetIntegerv(pname, &temp);
	handle = temp;
}

GLFramebufferTargetSaver::~GLFramebufferTargetSaver()
{
	glBindFramebuffer(target, handle);
}

GLRenderbuffer::~GLRenderbuffer()
{
	glDeleteRenderbuffers(1, &handle);
}

void GLRenderbuffer::bind() noexcept
{
	glBindRenderbuffer(GL_RENDERBUFFER, handle);
}

GLFramebuffer::~GLFramebuffer()
{
	glDeleteFramebuffers(1, &handle);
}

void GLFramebuffer::bindTo(GLenum target) noexcept
{
	glBindFramebuffer(target, handle);
}

void GLFramebuffer::textureColorAttachment(GLuint i, GLuint textureHandle, GLint level)
{
	GLFramebufferTargetSaver saver(GL_READ_FRAMEBUFFER);
	bindTo(GL_READ_FRAMEBUFFER);
	glFramebufferTexture(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, textureHandle, level);
}

void GLFramebuffer::renderbufferDepthAttachment(GLuint renderbufferHandle)
{
	GLFramebufferTargetSaver saver(GL_READ_FRAMEBUFFER);
	bindTo(GL_READ_FRAMEBUFFER);
	glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbufferHandle);
}

void GLFramebuffer::renderbufferColorAttachment(GLuint i, GLuint renderbufferHandle)
{
	GLFramebufferTargetSaver saver(GL_READ_FRAMEBUFFER);
	bindTo(GL_READ_FRAMEBUFFER);
	glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_RENDERBUFFER, renderbufferHandle);
}

void GLFramebuffer::blitToCurrent(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1,
                                  GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1,
                                  GLbitfield mask, GLenum filter)
{
	bindTo(GL_READ_FRAMEBUFFER);
	glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
}

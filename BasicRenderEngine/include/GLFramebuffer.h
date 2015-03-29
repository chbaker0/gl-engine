/*
 * GLFramebuffer.h
 *
 *  Created on: Mar 28, 2015
 *      Author: Collin
 */

#ifndef GL_FRAMEBUFFER_H_INCLUDED
#define GL_FRAMEBUFFER_H_INCLUDED

#include <GL/glew.h>

#include "GLRenderTarget.h"
#include "GLTexture.h"

class GLFramebufferTargetSaver
{
private:
	GLenum target;
	GLuint handle;

public:
	GLFramebufferTargetSaver(GLenum target_in);
	~GLFramebufferTargetSaver();
};

class GLRenderbuffer
{
private:
	GLuint handle;
	GLenum internalFormat;
	GLsizei width, height;
	GLsizei samples;

public:
	GLRenderbuffer(GLuint handle_in, GLenum internalFormat_in,
	               GLsizei width_in, GLsizei height_in, GLsizei samples_in):
	            	   handle(handle_in), internalFormat(internalFormat_in),
	            	   width(width_in), height(height_in), samples(samples_in) {}
	~GLRenderbuffer();

	GLuint getHandle() noexcept {return handle;}

	void bind() noexcept;
};

class GLFramebuffer : public GLRenderTarget
{
private:
	GLuint handle;

public:
	GLFramebuffer(GLuint handle_in): handle(handle_in) {}
	~GLFramebuffer();

	virtual void drawTo() override final {bindTo(GL_DRAW_FRAMEBUFFER);}

	void blitToCurrent(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1,
	                   GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1,
	                   GLbitfield mask, GLenum filter);

	void bindTo(GLenum target) noexcept;

	void textureColorAttachment(GLuint i, GLuint textureHandle, GLint level);
	void textureColorAttachment(GLuint i, GLTexture *texture, GLint level) {textureColorAttachment(i, texture->getHandle(), level);}

	void renderbufferDepthAttachment(GLuint renderbufferHandle);
	void renderbufferDepthAttachment(GLRenderbuffer *renderbuffer) {renderbufferDepthAttachment(renderbuffer->getHandle());}
};

#endif /* GL_FRAMEBUFFER_H_INCLUDED */

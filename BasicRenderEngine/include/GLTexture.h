/*
 * GLTexture.h
 *
 *  Created on: Mar 20, 2015
 *      Author: Collin
 */

#ifndef GL_TEXTURE_H_INCLUDED
#define GL_TEXTURE_H_INCLUDED

#include <iostream>
#include <GL/glew.h>

class GLTexture
{
protected:
	GLuint handle;
	GLenum internalFormat;
public:
	GLTexture(GLuint handle_in, GLenum internalFormat_in) noexcept: handle(handle_in), internalFormat(internalFormat_in) {}
	virtual ~GLTexture();

	virtual GLenum getTarget() const noexcept = 0;
	GLuint getHandle() const noexcept {return handle;}
	GLenum getInternalFormat() const noexcept {return internalFormat;}
	void bind() noexcept;
	void unBind() noexcept;
};

class GLTexture2D : public GLTexture
{
protected:
	GLsizei baseWidth, baseHeight;
	GLint mipmapLevels;

public:
	GLTexture2D(GLuint handle_in, GLenum internalFormat_in,
	            GLsizei baseWidth_in, GLsizei baseHeight_in,
	            GLint mipmapLevels_in) noexcept:
	            GLTexture(handle_in, internalFormat_in), baseWidth(baseWidth_in), baseHeight(baseHeight_in), mipmapLevels(mipmapLevels_in) {}
	virtual ~GLTexture2D() {}

	virtual GLenum getTarget() const noexcept override final {return GL_TEXTURE_2D;}

	void subImage(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, void *data);
};

class GLTextureTargetSaver
{
private:
	GLenum target;
	GLuint handle;
public:
	GLTextureTargetSaver() = delete;
	GLTextureTargetSaver(GLenum target_in);
	GLTextureTargetSaver(const GLTextureTargetSaver&) = delete;
	~GLTextureTargetSaver();
};

#endif /* GL_TEXTURE_H_INCLUDED */

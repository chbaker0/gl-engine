/*
 * GLTexture.h
 *
 *  Created on: Mar 20, 2015
 *      Author: Collin
 */

#ifndef GL_TEXTURE_H_INCLUDED
#define GL_TEXTURE_H_INCLUDED

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
	GLenum getInternalFormat() const noexcept {return internalFormat;}
	void bind() noexcept;
	void unBind() noexcept;
};

class GLTexture2D : public GLTexture
{
public:
	GLTexture2D(GLuint handle_in, GLenum internalFormat_in) noexcept: GLTexture(handle, internalFormat) {}
	virtual ~GLTexture2D() {}

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

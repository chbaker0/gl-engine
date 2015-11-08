/*
 * GLTexture.cpp
 *
 *  Created on: Mar 20, 2015
 *      Author: Collin
 */

#include <stdexcept>

#include "GLTexture.h"

GLTexture::~GLTexture()
{
	glDeleteTextures(1, &handle);
}

void GLTexture::bind() noexcept
{
	glBindTexture(this->getTarget(), handle);
}

void GLTexture::unBind() noexcept
{
	glBindTexture(this->getTarget(), 0);
}

GLTextureTargetSaver::GLTextureTargetSaver(GLenum target_in)
{
	target = target_in;
	GLenum pname = GL_INVALID_ENUM;
	switch(target)
	{
	case GL_TEXTURE_2D:
		pname = GL_TEXTURE_BINDING_2D;
		break;
	default:
		throw std::logic_error("Invalid target passed to GLTextureTargetSaver constructor");
		break;
	}
	GLint temp;
	glGetIntegerv(pname, &temp);
	handle = temp;
}

GLTextureTargetSaver::~GLTextureTargetSaver()
{
	glBindTexture(target, handle);
}

void GLTexture2D::subImage(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, void *data)
{
	GLTextureTargetSaver(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, handle);
	glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, format, type, data);
}

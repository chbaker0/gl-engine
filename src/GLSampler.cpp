/*
 * GLSampler.cpp
 *
 *  Created on: Apr 1, 2015
 *      Author: Collin
 */

#include <glm/gtc/type_ptr.hpp>

#include "GLSampler.h"

struct AnisotropicExtensionChecker
{
	bool isAvailable;

	AnisotropicExtensionChecker()
	{
		if(GLEW_EXT_texture_filter_anisotropic)
			isAvailable = true;
		else
			isAvailable = false;
	}
};

GLSampler::~GLSampler()
{
	glDeleteSamplers(1, &handle);
}

void GLSampler::setMagFilter(GLenum setting)
{
	glSamplerParameteri(handle, GL_TEXTURE_MAG_FILTER, setting);
}
void GLSampler::setMinFilter(GLenum setting)
{
	glSamplerParameteri(handle, GL_TEXTURE_MIN_FILTER, setting);
}

void GLSampler::setAnisotropy(GLfloat anisotropy)
{
	// This is safe because static initialization happens
	// upon first entry to a method (so this will happen
	// after a context is created)
	static AnisotropicExtensionChecker checker;

	if(checker.isAvailable)
		glSamplerParameterf(handle, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
}

void GLSampler::setWrapModeS(GLenum setting)
{
	glSamplerParameteri(handle, GL_TEXTURE_WRAP_S, setting);
}
void GLSampler::setWrapModeT(GLenum setting)
{
	glSamplerParameteri(handle, GL_TEXTURE_WRAP_T, setting);
}
void GLSampler::setWrapModeR(GLenum setting)
{
	glSamplerParameteri(handle, GL_TEXTURE_WRAP_R, setting);
}
void GLSampler::setWrapMode(GLenum setting)
{
	setWrapModeS(setting);
	setWrapModeT(setting);
	setWrapModeR(setting);
}

void GLSampler::setBorderColor(glm::vec4 color)
{
	glSamplerParameterfv(handle, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(color));
}

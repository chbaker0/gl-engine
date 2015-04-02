/*
 * GLSampler.h
 *
 *  Created on: Apr 1, 2015
 *      Author: Collin
 */

#ifndef GL_SAMPLER_H_INCLUDED
#define GL_SAMPLER_H_INCLUDED

#include <GL/glew.h>
#include <glm/glm.hpp>

class GLSampler
{
private:
	GLuint handle;

public:
	GLSampler(GLuint handle_in): handle(handle_in) {}
	~GLSampler();

	GLuint getHandle() noexcept
	{
		return handle;
	}

	void setMagFilter(GLenum setting);
	void setMinFilter(GLenum setting);

	void setWrapModeS(GLenum setting);
	void setWrapModeT(GLenum setting);
	void setWrapModeR(GLenum setting);
	void setWrapMode(GLenum setting);

	void setBorderColor(glm::vec4 color);

	void setAnisotropy(GLfloat anisotropy);
};

#endif /* GL_SAMPLER_H_INCLUDED */

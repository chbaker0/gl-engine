/*
 * GLShader.h
 *
 *  Created on: Mar 8, 2015
 *      Author: Collin
 */

#ifndef GL_SHADER_H_INCLUDED
#define GL_SHADER_H_INCLUDED

#include <GL/glew.h>

class GLShader
{
protected:
	GLuint handle;

public:
	GLShader(GLuint handle_in): handle(handle_in) {}
	virtual ~GLShader();

	GLuint getHandle() const noexcept
	{
		return handle;
	}
};

#endif /* GL_SHADER_H_INCLUDED */

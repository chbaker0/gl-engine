/*
 * GLShaderProgram.h
 *
 *  Created on: Mar 8, 2015
 *      Author: Collin
 */

#ifndef GL_SHADER_PROGRAM_H_INCLUDED
#define GL_SHADER_PROGRAM_H_INCLUDED

#include <vector>

#include <GL/glew.h>

#include "GLExecutable.h"

class GLShaderProgram : public GLExecutable
{
protected:
	GLuint handle;

public:
	GLShaderProgram(GLuint handle_in): handle(handle_in) {}
	virtual ~GLShaderProgram();

	GLuint getHandle() const noexcept
	{
		return handle;
	}
	virtual void use() noexcept override;
};

#endif /* GL_SHADER_PROGRAM_H_INCLUDED */

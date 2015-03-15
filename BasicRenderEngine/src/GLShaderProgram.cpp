/*
 * GLShaderProgram.cpp
 *
 *  Created on: Mar 9, 2015
 *      Author: Collin
 */


#include "GLShaderProgram.h"

GLShaderProgram::~GLShaderProgram()
{
	glDeleteProgram(handle);
}

void GLShaderProgram::use() noexcept
{
	if(activeExecutable != this)
	{
		glUseProgram(handle);
		activeExecutable = this;
	}
}

void GLShaderProgram::setUniformBlockBinding(GLProgramUniformBlockIndex index, GLUniformBlockBinding binding) noexcept
{
	glUniformBlockBinding(handle, (GLuint)index, (GLuint)binding);
}

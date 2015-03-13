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
	glUseProgram(handle);
}

void GLShaderProgram::setUniformBlockBinding(GLProgramUniformBlockIndex index, GLUniformBlockBinding binding) noexcept
{
	glUniformBlockBinding(handle, (GLuint)index, (GLuint)binding);
}

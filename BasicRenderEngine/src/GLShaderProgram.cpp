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

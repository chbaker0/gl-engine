/*
 * GLShaderProgram.h
 *
 *  Created on: Mar 8, 2015
 *      Author: Collin
 */

#ifndef GL_SHADER_PROGRAM_H_INCLUDED
#define GL_SHADER_PROGRAM_H_INCLUDED

#include <vector>

class GLShaderProgram
{
protected:
	GLuint handle;

public:
	GLShaderProgram(GLuint handle_in): handle(handle_in) {}
	virtual ~GLShaderProgram();
};

class GLShaderProgramBuilder
{

};

#endif /* GL_SHADER_PROGRAM_H_INCLUDED */

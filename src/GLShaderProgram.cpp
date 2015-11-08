/*
 * GLShaderProgram.cpp
 *
 *  Created on: Mar 9, 2015
 *      Author: Collin
 */

#include <stdexcept>

#include "GLShaderProgram.h"
#include "GLContext.h"

static const char* translateIndex(GLProgramUniformBlockIndex i)
{
	switch(i)
	{
	case GLProgramUniformBlockIndex::GlobalBlock:
		return "GlobalBlock";
		break;
	case GLProgramUniformBlockIndex::ModelBlock:
		return "ModelBlock";
		break;
	default:
		throw std::logic_error("Unimplemented block index passed to translateIndex");
	}
}

GLShaderProgram::GLShaderProgram(GLuint handle_in): handle(handle_in)
{
	for(unsigned int i = 0; i < (unsigned int)GLProgramUniformBlockIndex::ElementCount; ++i)
	{
		const char *blockName = translateIndex((GLProgramUniformBlockIndex) i);
		blockIndices[i] = glGetUniformBlockIndex(handle, blockName);
	}
}

GLShaderProgram::~GLShaderProgram()
{
	glDeleteProgram(handle);
}

void GLShaderProgram::use() noexcept
{
	glUseProgram(handle);
}

GLint GLShaderProgram::getUniformLocation(const char *name)
{
	return glGetUniformLocation(handle, name);
}

struct ProgramSaver
{
	GLContext *context;
	GLExecutable *executable;

	ProgramSaver()
	{
		context = GLContext::getCurrentContext();
		executable = context->getCurrentExecutable();
	}
	~ProgramSaver()
	{
		context->useExecutable(executable);
	}
};

void GLShaderProgram::setUniform(GLint location, GLint data)
{
	ProgramSaver saver;
	glUseProgram(handle);
	glUniform1i(location, data);
}

void GLShaderProgram::setUniform(GLint location, GLuint data)
{
	ProgramSaver saver;
	glUseProgram(handle);
	glUniform1ui(location, data);
}

void GLShaderProgram::setUniformBlockBinding(GLProgramUniformBlockIndex index, GLUniformBlockBinding binding) noexcept
{
	glUniformBlockBinding(handle, blockIndices[(unsigned int)index], (GLuint)binding);
}

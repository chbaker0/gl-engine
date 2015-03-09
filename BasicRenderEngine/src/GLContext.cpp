/*
 * GLContext.cpp
 *
 *  Created on: Mar 8, 2015
 *      Author: Collin
 */

#include <iostream>

#include "GLContext.h"

std::unique_ptr<GLMutableBuffer> GLContext::getMutableBuffer(GLenum target, GLenum usage, GLsizeiptr size, void *data)
{
	using std::cout; using std::endl;
	GLuint handle;
	GLBufferTargetSaver s(target);
	glGenBuffers(1, &handle);
	glBindBuffer(target, handle);
	glBufferData(target, size, data, usage);
	return std::unique_ptr<GLMutableBuffer>(new GLMutableBuffer(target, handle, size, usage));
}

std::unique_ptr<GLVertexArrayObject> GLContext::getVertexArrayObject()
{
	GLuint handle;
	glGenVertexArrays(1, &handle);
	return std::unique_ptr<GLVertexArrayObject>(new GLVertexArrayObject(handle));
}

std::unique_ptr<GLShaderProgram> GLContext::getStandaloneShaderProgram(GLenum type, const std::string& source, std::string *errorLog_out)
{
	const char *str = source.c_str();
	GLuint handle = glCreateShaderProgramv(type, 1, &str);
	if(handle == 0) throw std::runtime_error("Could not create a program object");
	GLint s;
	glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &s);
	std::string errorLog(s, 0);
	GLsizei temp;
	glGetProgramInfoLog(handle, s, &temp, &errorLog[0]);
	errorLog.pop_back(); // Get rid of null terminator
	GLint status;
	glGetProgramiv(handle, GL_LINK_STATUS, &status);
	if(status == GL_FALSE)
		throw GLLinkerError(errorLog);
	if(errorLog_out != nullptr)
		*errorLog_out = std::move(errorLog);
	return std::unique_ptr<GLShaderProgram>(new GLShaderProgram(handle));
}

std::unique_ptr<GLProgramPipeline> GLContext::getProgramPipeline()
{
	GLuint handle;
	glGenProgramPipelines(1, &handle);
	return std::unique_ptr<GLProgramPipeline>(new GLProgramPipeline(handle));
}

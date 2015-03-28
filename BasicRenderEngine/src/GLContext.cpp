/*
 * GLContext.cpp
 *
 *  Created on: Mar 8, 2015
 *      Author: Collin
 */

#include <iostream>

#include "GLContext.h"

thread_local GLContext* GLContext::currentContext = nullptr;

struct GLContext::Pimpl
{
	GLExecutable *currentExecutable;
	GLVertexArrayObject *currentVao;
	GLRenderTarget *currentRenderTarget;
};

GLContext::GLContext()
{
	pimpl = new Pimpl{nullptr, nullptr};
}

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
	if(errorLog.size())
		errorLog.pop_back(); // Get rid of null terminator
	GLint status;
	glGetProgramiv(handle, GL_LINK_STATUS, &status);
	if(status == GL_FALSE)
		throw GLLinkerError(std::move(errorLog));
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

std::unique_ptr<GLTexture2D> GLContext::getTexture2D(GLint levels, bool generateMipmaps, GLint internalFormat,
                                                     GLsizei baseWidth, GLsizei baseHeight,
                                                     GLenum format, GLenum type, const void *data)
{
	GLuint handle;
	glGenTextures(1, &handle);
	GLTextureTargetSaver saver(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, handle);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, baseWidth, baseHeight, 0, format, type, data);
	GLuint totalLevels = 1;
	if(levels == 0)
	{
		GLsizei levelWidth = baseWidth,
				levelHeight = baseHeight;
		while(levelWidth > 1 && levelHeight > 1)
		{
			levelWidth /= 2, levelHeight /= 2;
			if(levelWidth < 1)
				levelWidth = 1;
			if(levelHeight < 1)
				levelHeight = 1;
			glTexImage2D(GL_TEXTURE_2D, totalLevels, internalFormat, levelWidth, levelHeight, 0, format, type, nullptr);
			++totalLevels;
		}
	}
	else
		totalLevels = levels;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, totalLevels-1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if(generateMipmaps)
		glGenerateMipmap(GL_TEXTURE_2D);

	return std::unique_ptr<GLTexture2D>(new GLTexture2D(handle, internalFormat, baseWidth, baseHeight, totalLevels));
}

void GLContext::bindTexture(GLuint textureUnit, GLTexture *texture)
{
	glActiveTexture(GL_TEXTURE0);
	texture->bind();
}

void GLContext::useExecutable(GLExecutable *s) noexcept
{
	if(s == nullptr)
	{
		glUseProgram(0);
		glBindProgramPipeline(0);
		pimpl->currentExecutable = nullptr;
	}
	else if(s != pimpl->currentExecutable)
	{
		s->use();
		pimpl->currentExecutable = s;
	}
}

void GLContext::useVao(GLVertexArrayObject *vao) noexcept
{
	if(vao == nullptr)
	{
		glBindVertexArray(0);
		pimpl->currentVao = nullptr;
	}
	else if(vao != pimpl->currentVao)
	{
		vao->bind();
		pimpl->currentVao = vao;
	}
}

void GLContext::useRenderTarget(GLRenderTarget *rt) noexcept
{
	if(rt != pimpl->currentRenderTarget)
	{
		rt->drawTo();
		pimpl->currentRenderTarget = rt;
	}
}

GLExecutable* GLContext::getCurrentExecutable() const noexcept
{
	return pimpl->currentExecutable;
}
GLVertexArrayObject* GLContext::getCurrentVao() const noexcept
{
	return pimpl->currentVao;
}
GLRenderTarget* GLContext::getCurrentRenderTarget() const noexcept
{
	return pimpl->currentRenderTarget;
}

void GLContext::setClearColor(glm::vec4 color) noexcept
{
	glClearColor(color.r, color.g, color.b, color.a);
}

void GLContext::clear(GLbitfield mask) noexcept
{
	glClear(mask);
}

void GLContext::depthTestEnabled(bool enabled) noexcept
{
	if(enabled)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

void GLContext::faceCullingEnabled(bool enabled) noexcept
{
	if(enabled)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
}

unsigned int GLContext::queryUniformBufferOffsetAlignment() const noexcept
{
	GLint temp;
	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &temp);
	return temp;
}

/* 
 * File:   GLContext.h
 * Author: Collin
 *
 * Created on March 2, 2015, 9:21 PM
 */

#ifndef GL_CONTEXT_H_INCLUDED
#define GL_CONTEXT_H_INCLUDED

#include <memory>
#include <string>
#include <stdexcept>
#include <utility>
#include <iostream>

#include <GL/glew.h>

#include <glm/glm.hpp>

#include "GLBuffer.h"
#include "GLVertexArrayObject.h"
#include "GLShaderProgram.h"
#include "GLProgramPipeline.h"
#include "GLRenderTarget.h"
#include "GLTexture.h"
#include "GLSampler.h"
#include "GLFramebuffer.h"

class GLLinkerError : public std::runtime_error
{
protected:
	std::string errorLog;

public:
	GLLinkerError(std::string errorLog_in): runtime_error("Shader program linking error")
	{
		errorLog = std::move(errorLog_in);
	}
	const std::string& getErrorLog() const noexcept
	{
		return errorLog;
	}
};

class GLContext
{
protected:
	struct Pimpl;
	Pimpl *pimpl;

	static thread_local GLContext *currentContext;

public:
	GLContext();
    virtual ~GLContext() = 0;

    virtual void setSwapInterval(int i) noexcept = 0;

    virtual void setCurrent() = 0;
    virtual void unsetCurrent() = 0;

    virtual std::unique_ptr<GLMutableBuffer> getMutableBuffer(GLenum target, GLenum usage, GLsizeiptr size, void *data);
    virtual std::unique_ptr<GLVertexArrayObject> getVertexArrayObject();
    virtual std::unique_ptr<GLShaderProgram> getStandaloneShaderProgram(GLenum type, const std::string& source, std::string *log);
    virtual std::unique_ptr<GLProgramPipeline> getProgramPipeline();

    virtual std::unique_ptr<GLTexture2D> getTexture2D(GLint levels, bool generateMipmaps, GLint internalFormat,
                                                      GLsizei baseWidth, GLsizei baseHeight,
                                                      GLenum format, GLenum type, const void *data);
    virtual std::unique_ptr<GLSampler> getSampler();

    virtual void bindTexture(GLuint textureUnit, GLTexture *texture);
    virtual void bindSampler(GLuint textureUnit, GLSampler *sampler);

    virtual std::unique_ptr<GLRenderbuffer> getRenderbuffer(GLenum internalFormat, GLsizei width, GLsizei height, GLsizei samples);
    virtual std::unique_ptr<GLFramebuffer> getFramebuffer();

    virtual void useExecutable(GLExecutable *s) noexcept;
    virtual void useVao(GLVertexArrayObject *vao) noexcept;
    virtual void useRenderTarget(GLRenderTarget *rt) noexcept;

    virtual GLExecutable* getCurrentExecutable() const noexcept;
    virtual GLVertexArrayObject* getCurrentVao() const noexcept;
    virtual GLRenderTarget* getCurrentRenderTarget() const noexcept;

    virtual void setClearColor(glm::vec4 color) noexcept;
    virtual void clear(GLbitfield mask) noexcept;

    virtual void srgbWriteEnabled(bool enabled) noexcept;
    virtual void depthTestEnabled(bool enabled) noexcept;
    virtual void faceCullingEnabled(bool enabled) noexcept;

    virtual unsigned int queryUniformBufferOffsetAlignment() const noexcept;

    virtual std::pair<unsigned int, unsigned int> getGLVersion() const noexcept = 0;

    static GLContext* getCurrentContext() noexcept
	{
    	return currentContext;
	}
};

inline GLContext::~GLContext() {}

#endif // GL_CONTEXT_H_INCLUDED

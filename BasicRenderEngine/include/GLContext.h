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

#include <GL/glew.h>

#include "GLBuffer.h"
#include "GLVertexArrayObject.h"
#include "GLShaderProgram.h"
#include "GLProgramPipeline.h"

class GLLinkerError : public std::runtime_error
{
protected:
	std::string errorLog;

public:
	GLLinkerError(std::string errorLog_in): runtime_error("Shader program linking error"), errorLog(std::move(errorLog)) {}
	const std::string& getErrorLog() const noexcept
	{
		return errorLog;
	}
};

class GLContext
{
public:
    virtual ~GLContext() = 0;

    virtual void setCurrent() = 0;
    virtual void unsetCurrent() = 0;

    virtual std::unique_ptr<GLMutableBuffer> getMutableBuffer(GLenum target, GLenum usage, GLsizeiptr size, void *data);
    virtual std::unique_ptr<GLVertexArrayObject> getVertexArrayObject();
    virtual std::unique_ptr<GLShaderProgram> getStandaloneShaderProgram(GLenum type, const std::string& source, std::string *log);
    virtual std::unique_ptr<GLProgramPipeline> getProgramPipeline();
};

inline GLContext::~GLContext() {}

#endif // GL_CONTEXT_H_INCLUDED

/* 
 * File:   GLContext.h
 * Author: Collin
 *
 * Created on March 2, 2015, 9:21 PM
 */

#ifndef GL_CONTEXT_H_INCLUDED
#define GL_CONTEXT_H_INCLUDED

#include <memory>

#include <GL/glew.h>

#include "GLBuffer.h"

class GLContext
{
public:
    virtual ~GLContext() = 0;

    virtual void setCurrent() = 0;
    virtual void unsetCurrent() = 0;

    virtual std::unique_ptr<GLMutableBuffer> getMutableBuffer(GLenum target, GLenum usage, GLsizeiptr size, void *data);
};

inline GLContext::~GLContext() {}

#endif // GL_CONTEXT_H_INCLUDED

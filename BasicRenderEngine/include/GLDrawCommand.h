/*
 * GLDrawCommand.h
 *
 *  Created on: Mar 12, 2015
 *      Author: Collin
 */

#ifndef GL_DRAW_COMMAND_H_INCLUDED
#define GL_DRAW_COMMAND_H_INCLUDED

#include "GLVertexArrayObject.h"
#include "GLExecutable.h"

class GLContext;

class GLDrawCommand
{
protected:
	GLVertexArrayObject *vao;
	GLsizei elementCount;
	GLenum primType;

public:
	GLDrawCommand(GLVertexArrayObject *vao_in, GLsizei elementCount_in, GLenum primType_in):
		vao(vao_in), elementCount(elementCount_in), primType(primType_in) {}
	virtual void draw(GLContext *context) = 0;

	GLVertexArrayObject* getVao() const
	{
		return vao;
	}
	GLsizei getElementCount() const
	{
		return elementCount;
	}
	GLenum getPrimType() const
	{
		return primType;
	}
};

#endif /* GL_DRAW_COMMAND_H_INCLUDED */

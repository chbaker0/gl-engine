/*
 * GLDrawIndexedCommand.h
 *
 *  Created on: Mar 12, 2015
 *      Author: Collin
 */

#ifndef GL_DRAW_INDEXED_COMMAND
#define GL_DRAW_INDEXED_COMMAND

#include "GLDrawCommand.h"

class GLDrawIndexedCommand : public GLDrawCommand
{
protected:
	const void *indexOffset;
	GLenum indexType;

public:
	GLDrawIndexedCommand(GLVertexArrayObject *vao_in, GLsizei elementCount_in, GLenum primType_in, const void *indexOffset_in, GLenum indexType_in):
		GLDrawCommand(vao_in, elementCount_in, primType_in), indexOffset(indexOffset_in), indexType(indexType_in) {}
	virtual void draw(GLContext *context) override;
};

#endif /* GL_DRAW_INDEXED_COMMAND */

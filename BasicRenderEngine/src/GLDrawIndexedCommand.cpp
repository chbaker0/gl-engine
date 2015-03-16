/*
 * GLDrawIndexedCommand.cpp
 *
 *  Created on: Mar 12, 2015
 *      Author: Collin
 */

#include "GLDrawIndexedCommand.h"
#include "GLContext.h"

void GLDrawIndexedCommand::draw(GLContext *context)
{
	context->useExecutable(shaderExe);
	context->useVao(vao);
	glDrawElements(primType, elementCount, indexType, indexOffset);
}

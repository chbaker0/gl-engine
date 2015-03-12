/*
 * GLDrawIndexedCommand.cpp
 *
 *  Created on: Mar 12, 2015
 *      Author: Collin
 */

#include "GLDrawIndexedCommand.h"

void GLDrawIndexedCommand::draw()
{
	shaderExe->use();
	vao->bind();
	glDrawElements(primType, elementCount, indexType, indexOffset);
}

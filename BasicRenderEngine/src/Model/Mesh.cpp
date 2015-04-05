/*
 * Mesh.cpp
 *
 *  Created on: Apr 5, 2015
 *      Author: Collin
 */

#include <iostream>

#include <stdexcept>

#include <cassert>
#include <cstddef>
#include <cstring>

#include "Model/Mesh.h"

unsigned int getTypeSize(GLenum type)
{
	switch(type)
	{
	case GL_FLOAT:
		return sizeof(GLfloat);
		break;
	case GL_UNSIGNED_SHORT:
		return sizeof(GLushort);
		break;
	default:
		throw std::logic_error("Invalid or unimplemented type");
	}
}

Mesh loadMeshFromMem(GLContext *context, const void *vertexData, MeshVertexAttribDescriptor *attribs, unsigned int attribCount, GLsizei vertexCount,
                     const void *indexData, GLenum indexType, GLsizei indexCount, GLenum primType,
                     GLMappableBufferView& vertexBuffer, GLMappableBufferView& indexBuffer)
{
	std::shared_ptr<GLVertexArrayObject> meshVao = context->getVertexArrayObject();

	assert(vertexBuffer.isWritable());
	assert(indexBuffer.isWritable());
	{
		auto vertexMap = vertexBuffer.mapRange(0, vertexBuffer.getSize(), GL_MAP_WRITE_BIT);
		char *vertexMapPtr = (char*) vertexMap.getMapPtr();
		for(unsigned int a = 0; a < attribCount; ++a)
		{
			auto attribTypeSize = getTypeSize(attribs[a].type);

			char *readPtr = (char*) vertexData + attribs[a].offset;
			char *writePtr = (char*) vertexMapPtr + attribs[a].offset;

			for(unsigned int v = 0; v < vertexCount; ++v)
			{
				assert((writePtr + attribs[a].size - vertexMapPtr) <= vertexBuffer.getSize());
				std::memcpy(writePtr, readPtr, attribs[a].size * attribTypeSize);
				writePtr += attribs[a].stride;
				readPtr += attribs[a].stride;
			}
		}
	}

	for(unsigned int a = 0; a < attribCount; ++a)
	{
		meshVao->setAttrib(attribs[a].attrib, vertexBuffer.getBuffer(),
		                   attribs[a].size, attribs[a].type, attribs[a].normalize,
		                   attribs[a].stride, (char*)attribs[a].offset + vertexBuffer.getOffset());
	}

	{
		auto indexMap = indexBuffer.mapRange(0, indexBuffer.getSize(), GL_MAP_WRITE_BIT);
		auto indexTypeSize = getTypeSize(indexType);
		std::memcpy(indexMap.getMapPtr(), indexData, indexTypeSize * indexCount);
	}

	meshVao->setElementArrayBinding(indexBuffer.getBuffer());

	return Mesh(meshVao, primType, indexType, indexBuffer.getOffset(), indexCount);
}

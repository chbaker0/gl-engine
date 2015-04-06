/*
 * Mesh.h
 *
 *  Created on: Apr 5, 2015
 *      Author: Collin
 */

#ifndef MODEL_MESH_H_INCLUDED
#define MODEL_MESH_H_INCLUDED

#include <memory>

#include "GLContext.h"
#include "GLVertexArrayObject.h"
#include "BufferUtils/GLBufferView.h"

class Mesh
{
private:
	std::shared_ptr<GLVertexArrayObject> vao;
	GLenum primType;
	GLenum indexType;
	GLsizeiptr indexOffset;
	GLsizeiptr indexCount;

public:
	Mesh(std::shared_ptr<GLVertexArrayObject> vao_in,
	     GLenum primType_in, GLenum indexType_in,
	     GLsizeiptr indexOffset_in, GLsizeiptr indexCount_in):
		vao(vao_in), primType(primType_in), indexType(indexType_in),
		indexOffset(indexOffset_in), indexCount(indexCount_in) {}

	GLsizeiptr getIndexCount() const
	{
		return indexCount;
	}

	GLsizeiptr getIndexOffset() const
	{
		return indexOffset;
	}

	GLenum getIndexType() const
	{
		return indexType;
	}

	GLenum getPrimType() const
	{
		return primType;
	}

	const GLVertexArrayObject* getVao() const
	{
		return vao.get();
	}
	GLVertexArrayObject* getVao()
	{
		return vao.get();
	}
};

struct MeshVertexAttribDescriptor
{
	GLVertexArrayObject::Attribute attrib;
	GLint size;
	GLenum type;
	GLboolean normalize;
	GLsizei stride;
	GLsizeiptr offset;
};

Mesh loadMeshFromMem(GLContext *context, const void *vertexData, MeshVertexAttribDescriptor *attribs, unsigned int attribCount, GLsizei vertexCount,
                     const void *indexData, GLenum indexType, GLsizei indexCount, GLenum primType,
                     GLMappableBufferView &vertexBuffer, GLMappableBufferView &indexBuffer);

Mesh loadMeshFromFile(GLContext *context, std::string filename, std::unique_ptr<GLMappableBuffer>& buffer);

#endif /* MODEL_MESH_H_INCLUDED */

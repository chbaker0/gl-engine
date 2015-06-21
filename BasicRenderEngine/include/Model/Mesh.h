/*
 * Mesh.h
 *
 *  Created on: Apr 5, 2015
 *      Author: Collin
 */

#ifndef MODEL_MESH_H_INCLUDED
#define MODEL_MESH_H_INCLUDED

#include <memory>
#include <stdexcept>
#include <utility>

#include "GLContext.h"
#include "GLVertexArrayObject.h"
#include "BufferUtils/GLBufferView.h"

class MeshVertexData
{
public:
	virtual ~MeshVertexData() {};

	virtual void release(GLsizeiptr indexOffset, GLsizeiptr indexCount) = 0;
	virtual GLVertexArrayObject* getVAO() = 0;
	virtual GLBuffer* getBuffer() = 0;
};

class Mesh
{
private:
	MeshVertexData *vertexData;
	GLenum primType;
	GLenum indexType;
	GLsizeiptr indexOffset;
	GLsizeiptr indexCount;

public:
	Mesh(): vertexData(nullptr), primType(GL_INVALID_ENUM), indexType(GL_INVALID_ENUM),
			indexOffset(0), indexCount(0) {}
	Mesh(const Mesh&) = delete;
	Mesh(Mesh&& other)
	{
		vertexData = other.vertexData;
		primType = other.primType;
		indexType = other.indexType;
		indexOffset = other.indexOffset;
		indexCount = other.indexCount;
		other.vertexData = nullptr;
	}

	Mesh(MeshVertexData *vaoWrapper_in,
	     GLenum primType_in, GLenum indexType_in,
	     GLsizeiptr indexOffset_in, GLsizeiptr indexCount_in):
	    vertexData(vaoWrapper_in), primType(primType_in), indexType(indexType_in),
		indexOffset(indexOffset_in), indexCount(indexCount_in) {}

	~Mesh()
	{
		if(vertexData != nullptr)
			vertexData->release(indexOffset, indexCount);
	}

	Mesh& operator= (Mesh other)
	{
		swap(other);
		return *this;
	}

	void swap(Mesh& other)
	{
		using std::swap;
		swap(vertexData, other.vertexData);
		swap(primType, other.primType);
		swap(indexType, other.indexType);
		swap(indexOffset, other.indexOffset);
		swap(indexCount, other.indexCount);
	}

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
		return vertexData->getVAO();
	}
	GLVertexArrayObject* getVao()
	{
		return vertexData->getVAO();
	}

	void draw(GLContext& context);
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

class MeshLoaderError : public std::runtime_error
{
public:
	MeshLoaderError(const char *errorString): runtime_error(errorString) {}
};

Mesh createMeshFromExisting(GLVertexArrayObject *vao, GLenum primType, GLenum indexType, GLsizeiptr indexOffset, GLsizeiptr indexCount);

Mesh loadMeshFromMem(GLContext *context, const void *vertexData, const MeshVertexAttribDescriptor *attribs, unsigned int attribCount, GLsizei vertexCount,
                     const void *indexData, GLenum indexType, GLsizei indexCount, GLenum primType);

Mesh loadMeshFromFile(GLContext *context, std::string filename);

#endif /* MODEL_MESH_H_INCLUDED */

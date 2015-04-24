/*
 * Mesh.cpp
 *
 *  Created on: Apr 5, 2015
 *      Author: Collin
 */

#include <iostream>
#include <map>
#include <unordered_map>
#include <memory>
#include <utility>

#include <stdexcept>

#include <cassert>
#include <cstddef>
#include <cstring>

#include "Model/Mesh.h"

void Mesh::draw(GLContext& context)
{
	context.useVao(vao.get());
	glDrawElements(primType, indexCount, indexType, (void*) indexOffset);
}

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

///*class MeshBufferManager
//{
//private:
//	// Buffer pointer and ref count
//	std::map<GLMappableBuffer*, unsigned int> bufferPool;
//	GLContext *context;
//
//public:
//	class MeshBufferView
//	{
//	private:
//		GLMappableBufferView *bufferView;
//		MeshBufferManager *manager;
//
//	public:
//		MeshBufferView(GLMappableBufferView *bufferView_in, MeshBufferManager *manager_in):
//			bufferView(bufferView_in), manager(manager_in) {}
//		~MeshBufferView()
//		{
//			auto p = manager->bufferPool.find(&(bufferView->getBuffer()));
//			if(--(p->second) == 0)
//			{
//				auto ptr = p->first;
//				manager->bufferPool.erase(p);
//				delete ptr;
//			}
//		}
//
//		GLMappableBufferView* getBufferView()
//		{
//			return bufferView;
//		}
//	};
//
//	MeshBufferManager(GLContext *context_in): context(context_in) {}
//
//	MeshBufferView getBufferView(GLsizei size)
//	{
//		// Later will do some actual management. For now, just allocate a buffer every call:
//		auto p = bufferPool.emplace(context->getMutableBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, size, nullptr).release(), 1);
//		assert(("wat", p.second));
//		return MeshBufferView(new GLMappableBufferView(p.first, 0, size), this);
//	}
//};*/

Mesh loadMeshFromMem(GLContext *context, const void *vertexData, MeshVertexAttribDescriptor *attribs, unsigned int attribCount, GLsizei vertexCount,
                     const void *indexData, GLenum indexType, GLsizei indexCount, std::size_t maxIndex, GLenum primType)
{
//	static std::unordered_map<GLContext*, MeshBufferManager> contextManagerMap;

	std::size_t bufferSize = 0;
	for(unsigned int i = 0; i < attribCount; ++i)
	{
		std::size_t attribSize =
				attribs[i].offset + attribs[i].
	}

	std::shared_ptr<GLVertexArrayObject> meshVao = context->getVertexArrayObject();
	std::shared_ptr<G>
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

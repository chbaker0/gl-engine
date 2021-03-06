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
#include <string>
#include <utility>
#include <vector>
#include <algorithm>

#include <stdexcept>

#include <cassert>
#include <cstddef>
#include <cstring>

#include <assimp/Logger.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Model/Mesh.h"

void Mesh::draw(GLContext& context)
{
	context.useVao(vertexData->getVAO());
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

// Can be extended for sharing VAOs and VBOs later. For now... just this
class MeshVertexDataFromMem : public MeshVertexData
{
public:
	std::unique_ptr<GLBuffer> buf;
	std::unique_ptr<GLVertexArrayObject> vao;

	MeshVertexDataFromMem(std::unique_ptr<GLBuffer> buf_in, std::unique_ptr<GLVertexArrayObject> vao_in):
		buf(std::move(buf_in)), vao(std::move(vao_in)) {}

	virtual void release(GLsizeiptr, GLsizeiptr)
	{
		delete this;	// Evil voodoo
	}

	virtual GLVertexArrayObject* getVAO()
	{
		return vao.get();
	}
	virtual GLBuffer* getBuffer()
	{
		return buf.get();
	}
};

constexpr std::size_t alignedOffsetPow2(std::size_t offset, std::size_t alignment)
{
	return (offset + alignment - 1) & ~(alignment - 1);
}

Mesh loadMeshFromMem(GLContext *context, const void *vertexData, const MeshVertexAttribDescriptor *attribs, unsigned int attribCount,GLsizei vertexCount,
                     const void *indexData, GLenum indexType, GLsizei indexCount, GLenum primType)
{
	constexpr std::size_t MIN_ALIGNMENT = 4;

	auto *sortedAttribs = new MeshVertexAttribDescriptor[attribCount];
	std::copy(attribs, attribs + attribCount, sortedAttribs);
	std::sort(sortedAttribs, sortedAttribs + attribCount,
	          [](const MeshVertexAttribDescriptor &lhs,
					const MeshVertexAttribDescriptor &rhs)
				{
					return lhs.attrib < rhs.attrib;
				});

	std::size_t calculatedSize = 0;
	for(unsigned int i = 0; i < attribCount; ++i)
	{
		calculatedSize += attribs[i].size * getTypeSize(attribs[i].type) * vertexCount;
		calculatedSize = alignedOffsetPow2(calculatedSize, MIN_ALIGNMENT);
	}

	std::vector<std::size_t> offsets(attribCount);

	std::unique_ptr<char[]> tempBuffer(new char[calculatedSize + indexCount * getTypeSize(indexType)]);
	char *head = tempBuffer.get();

	for(unsigned int i = 0; i < attribCount; ++i)
	{
		std::size_t offset = head - tempBuffer.get();
		offset = alignedOffsetPow2(offset, MIN_ALIGNMENT);
		offsets[i] = offset;
		head = tempBuffer.get() + offset;

		const char *vertexDataHead = static_cast<const char*>(vertexData) + sortedAttribs[i].offset;
		const std::size_t attribByteSize = sortedAttribs[i].size * getTypeSize(sortedAttribs[i].type);
		const std::size_t stride = sortedAttribs[i].stride == 0 ? sortedAttribs[i].size : sortedAttribs[i].stride;

		assert(head < tempBuffer.get() + calculatedSize);

		for(GLsizei v = 0; v < vertexCount; ++v)
		{
			std::memcpy(head, vertexDataHead, attribByteSize);
			vertexDataHead += stride;
			head += attribByteSize;
		}
	}

	std::memcpy(tempBuffer.get() + calculatedSize, indexData, indexCount * getTypeSize(indexType));

	auto buffer = context->getMutableBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, calculatedSize + indexCount * getTypeSize(indexType), tempBuffer.get());
	tempBuffer.reset(nullptr);
	auto vao = context->getVertexArrayObject();

	for(unsigned int i = 0; i < attribCount; ++i)
	{
		vao->setAttrib(sortedAttribs[i].attrib, *buffer.get(), sortedAttribs[i].size, sortedAttribs[i].type, sortedAttribs[i].normalize, 0, (void*)offsets[i]);
	}

	vao->setElementArrayBinding(*buffer.get());

	MeshVertexDataFromMem *meshVertexData =
			new MeshVertexDataFromMem(std::move(buffer), std::move(vao));

	return Mesh(meshVertexData, primType, indexType, calculatedSize, indexCount);
}

Mesh loadMeshFromFile(GLContext *context, std::string filename)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(filename.c_str(),
	                                         aiProcess_Triangulate
	                                         | aiProcess_FindDegenerates
	                                         | aiProcess_JoinIdenticalVertices
	                                         | aiProcess_SortByPType
	                                         | aiProcess_ImproveCacheLocality
	                                         | aiProcess_FindInvalidData
	                                         | aiProcess_GenSmoothNormals
	                                         | aiProcess_OptimizeMeshes
	                                         | aiProcess_ValidateDataStructure);
	if(scene == nullptr)
		throw MeshLoaderError("Assimp failed to load model file");

	if(!scene->HasMeshes())
		throw MeshLoaderError("No meshes in loaded file");

	aiMesh *mesh = scene->mMeshes[0];

	std::vector<MeshVertexAttribDescriptor> attribs;
	std::vector<float> data;

	attribs.emplace_back(MeshVertexAttribDescriptor{GLVertexArrayObject::Position, 4, GL_FLOAT, GL_FALSE, 0, 0});
	for(std::size_t vIndex = 0; vIndex < mesh->mNumVertices; ++vIndex)
	{
		for(unsigned int i = 0; i < 3; ++i)
			data.push_back(mesh->mVertices[vIndex][i]);
		data.push_back(1.0f);
	}
	if(mesh->HasNormals())
	{
		attribs.emplace_back(MeshVertexAttribDescriptor{GLVertexArrayObject::Normal, 4, GL_FLOAT, GL_FALSE, 0, data.size()});

		for(std::size_t nIndex = 0; nIndex < mesh->mNumVertices; ++nIndex)
		{
			for(unsigned int i = 0; i < 3; ++i)
				data.push_back(mesh->mNormals[nIndex][i]);
			data.push_back(1.0f);
		}
	}
	if(mesh->HasTextureCoords(0))
	{
		attribs.emplace_back(MeshVertexAttribDescriptor{GLVertexArrayObject::TexCoord, 2, GL_FLOAT, GL_FALSE, 0, data.size()});

		for(std::size_t tIndex = 0; tIndex < mesh->mNumUVComponents[0]; ++tIndex)
		{
			for(unsigned int i = 0; i < 2; ++i)
				data.push_back(mesh->mTextureCoords[0][tIndex][i]);
		}
	}

	std::vector<GLushort> indices;

	for(std::size_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
	{
		aiFace *face = mesh->mFaces + faceIndex;
		if(face->mNumIndices != 3)
			continue;
		for(unsigned int i = 0; i < 3; ++i)
			indices.push_back(face->mIndices[i]);
	}

	return loadMeshFromMem(context, &data[0], &attribs[0], attribs.size(), mesh->mNumVertices, &indices[0], GL_UNSIGNED_SHORT, sizeof(indices), GL_TRIANGLES);
}

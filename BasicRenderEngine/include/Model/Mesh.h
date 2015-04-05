/*
 * Mesh.h
 *
 *  Created on: Apr 5, 2015
 *      Author: Collin
 */

#ifndef MODEL_MESH_H_INCLUDED
#define MODEL_MESH_H_INCLUDED

#include <memory>

#include "GLVertexArrayObject.h"
#include "GLBuffer.h"

class Mesh
{
private:
	std::unique_ptr<GLVertexArrayObject> vao;
	GLsizeiptr indexOffset;

public:
	Mesh(std::unique_ptr<GLVertexArrayObject> vao_in, GLsizeiptr indexOffset_in):
		vao(vao_in), indexOffset(indexOffset_in) {}
};

#endif /* MODEL_MESH_H_INCLUDED */

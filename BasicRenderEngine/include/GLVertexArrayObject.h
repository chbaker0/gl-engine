/*
 * GLVertexArrayObject.h
 *
 *  Created on: Mar 8, 2015
 *      Author: Collin
 */

#ifndef GL_VERTEX_ARRAY_OBJECT_INCLUDED
#define GL_VERTEX_ARRAY_OBJECT_INCLUDED

#include <GL/glew.h>

#include "GLBuffer.h"

class GLVertexArrayObject
{
protected:
	GLuint handle;

public:
	enum Attribute
	{
		PositionAttrib = 0,
		NormalAttrib
	};

	GLVertexArrayObject(GLuint handle_in): handle(handle_in) {}
	~GLVertexArrayObject();

	void setAttrib(GLuint index, GLBuffer& buf, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *offset) noexcept;
	void disableAttrib(GLuint i) noexcept;
	void setElementArrayBinding(GLBuffer& buf) noexcept;

	void bind() noexcept;
	void unBind() noexcept;
};

class GLVertexArrayObjectSaver
{
protected:
	GLuint handle;

public:
	GLVertexArrayObjectSaver();
	~GLVertexArrayObjectSaver();
};

#endif /* GL_VERTEX_ARRAY_OBJECT_INCLUDED */

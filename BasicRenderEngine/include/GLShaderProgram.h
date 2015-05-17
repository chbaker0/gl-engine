/*
 * GLShaderProgram.h
 *
 *  Created on: Mar 8, 2015
 *      Author: Collin
 */

#ifndef GL_SHADER_PROGRAM_H_INCLUDED
#define GL_SHADER_PROGRAM_H_INCLUDED

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "GLExecutable.h"
#include "GLBlockBinding.h"

enum class GLProgramUniformBlockIndex : unsigned int
{
	GlobalBlock = 0,
	ModelBlock,
	ElementCount
};

class GLShaderProgram : public GLExecutable
{
protected:
	GLuint handle;
	GLuint blockIndices[(unsigned int)GLProgramUniformBlockIndex::ElementCount];

public:
	GLShaderProgram(GLuint handle_in);
	virtual ~GLShaderProgram();

	GLuint getHandle() const noexcept
	{
		return handle;
	}
	virtual void use() noexcept override;

	virtual GLint getUniformLocation(const char *name);

	virtual void setUniform(GLint location, GLint data);
	virtual void setUniform(GLint location, GLuint data);
	virtual void setUniformBlockBinding(GLProgramUniformBlockIndex index, GLUniformBlockBinding binding) noexcept;
};

#endif /* GL_SHADER_PROGRAM_H_INCLUDED */

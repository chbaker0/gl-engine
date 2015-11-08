/*
 * GLProgramPipeline.h
 *
 *  Created on: Mar 9, 2015
 *      Author: Collin
 */

#ifndef GL_PROGRAM_PIPELINE_INCLUDED
#define GL_PROGRAM_PIPELINE_INCLUDED

#include <GL/glew.h>

#include "GLExecutable.h"
#include "GLShaderProgram.h"

class GLProgramPipeline : public GLExecutable
{
protected:
	GLuint handle;

public:
	GLProgramPipeline(GLuint handle_in): handle(handle_in) {};
	virtual ~GLProgramPipeline();

	virtual void use() noexcept override;

	virtual void useProgramStages(GLbitfield stages, GLShaderProgram& prog) noexcept;
	virtual void clearStages(GLbitfield stages) noexcept;
};

#endif /* GL_PROGRAM_PIPELINE_INCLUDED */

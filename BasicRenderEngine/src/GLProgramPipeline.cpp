/*
 * GLProgramPipeline.cpp
 *
 *  Created on: Mar 9, 2015
 *      Author: Collin
 */

#include "GLProgramPipeline.h"

GLProgramPipeline::~GLProgramPipeline()
{
	glDeleteProgramPipelines(1, &handle);
}

void GLProgramPipeline::use() noexcept
{
	if(activeExecutable != this)
	{
		glUseProgram(0);
		glBindProgramPipeline(handle);
		activeExecutable = this;
	}
}

void GLProgramPipeline::useProgramStages(GLbitfield stages, GLShaderProgram& prog) noexcept
{
	glUseProgramStages(handle, stages, prog.getHandle());
}

void GLProgramPipeline::clearStages(GLbitfield stages) noexcept
{
	glUseProgramStages(handle, stages, 0);
}

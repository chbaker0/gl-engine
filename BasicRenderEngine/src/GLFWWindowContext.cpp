/*
 * GLFWWindowContext.cpp
 *
 *  Created on: Mar 3, 2015
 *      Author: Collin
 */

#include "GLFWWindowContext.h"

GLFWWindowContext::GLFWWindowContext(GLFWwindow *handle_in) noexcept
{
	handle = handle_in;
}

GLFWWindowContext::~GLFWWindowContext()
{
	// Do nothing
}

void GLFWWindowContext::updateHandle(GLFWwindow *handle_in) noexcept
{
	handle = handle_in;
}

void GLFWWindowContext::setCurrent()
{
	glfwMakeContextCurrent(handle);
}

void GLFWWindowContext::unsetCurrent()
{
	glfwMakeContextCurrent(nullptr);
}

/*
 * GLFWWindowContext.cpp
 *
 *  Created on: Mar 3, 2015
 *      Author: Collin
 */

#include <boost/scope_exit.hpp>

#include "GLFWWindowContext.h"

GLFWWindowContext::GLFWWindowContext(GLFWwindow *handle_in, unsigned int major_in, unsigned int minor_in) noexcept
{
	handle = handle_in;
	major = major_in;
	minor = minor_in;
}

GLFWWindowContext::~GLFWWindowContext()
{
	// Do nothing
}

void GLFWWindowContext::updateHandle(GLFWwindow *handle_in) noexcept
{
	handle = handle_in;
}

void GLFWWindowContext::setSwapInterval(int i) noexcept
{
	GLContext *other = nullptr;
	if(currentContext != this)
	{
		other = currentContext;
		setCurrent();
	}
	glfwSwapInterval(i);
	if(other != nullptr)
	{
		other->setCurrent();
	}
}

void GLFWWindowContext::setCurrent()
{
	glfwMakeContextCurrent(handle);
	currentContext = this;
}

void GLFWWindowContext::unsetCurrent()
{
	glfwMakeContextCurrent(nullptr);
	currentContext = nullptr;
}

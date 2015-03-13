/* 
 * File:   GLFWWindowContext.h
 * Author: Collin
 *
 * Created on March 2, 2015, 9:24 PM
 */

#ifndef GLFW_WINDOW_CONTEXT_H_INCLUDED
#define	GLFW_WINDOW_CONTEXT_H_INCLUDED

#include <GL/glew.h>
#include <glfw/glfw3.h>

#include "GLContext.h"

class GLFWWindowContext : public GLContext
{
protected:
	GLFWwindow *handle;		// Not owned

public:
	GLFWWindowContext(GLFWwindow*) noexcept;
	GLFWWindowContext() noexcept: GLFWWindowContext(nullptr) {};
	virtual ~GLFWWindowContext();

	void updateHandle(GLFWwindow*) noexcept;

	virtual void setSwapInterval(int i) noexcept override;

	virtual void setCurrent() override;
	virtual void unsetCurrent() override;
};

class GLFWWindowContext44 : public GLFWWindowContext
{
private:

public:
	GLFWWindowContext44(GLFWwindow *handle_in) noexcept: GLFWWindowContext(handle_in) {}
	GLFWWindowContext44() noexcept: GLFWWindowContext44(nullptr) {};
};

#endif	/* GLFW_WINDOW_CONTEXT_H_INCLUDED */


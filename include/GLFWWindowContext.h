/* 
 * File:   GLFWWindowContext.h
 * Author: Collin
 *
 * Created on March 2, 2015, 9:24 PM
 */

#ifndef GLFW_WINDOW_CONTEXT_H_INCLUDED
#define	GLFW_WINDOW_CONTEXT_H_INCLUDED

#include <utility>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLContext.h"

class GLFWWindowContext : public GLContext
{
protected:
	GLFWwindow *handle;		// Not owned
	unsigned int major, minor;

public:
	GLFWWindowContext(GLFWwindow *handle_in, unsigned int major_in, unsigned int minor_in) noexcept;
	virtual ~GLFWWindowContext();

	void updateHandle(GLFWwindow*) noexcept;

	virtual void setSwapInterval(int i) noexcept override;

	virtual void setCurrent() override;
	virtual void unsetCurrent() override;

	virtual std::pair<unsigned int, unsigned int> getGLVersion() const noexcept override
	{
		return std::pair<unsigned int, unsigned int>(major, minor);
	}
};

#endif	/* GLFW_WINDOW_CONTEXT_H_INCLUDED */


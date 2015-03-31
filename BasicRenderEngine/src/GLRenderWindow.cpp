#include <iostream>

#include <utility>

#include "GLRenderWindow.h"
#include "GLFWRenderWindow.h"

struct GLRenderWindowCreator::Pimpl
{
    unsigned int xSize, ySize;
    unsigned int glVersionMajor, glVersionMinor;
    bool debug;
    bool srgb;
    std::string title;
};

GLRenderWindowCreator::GLRenderWindowCreator()
{
    pimpl = new Pimpl{0, 0, 3, 3, false};
}
GLRenderWindowCreator::~GLRenderWindowCreator()
{
    delete pimpl;
}

void GLRenderWindowCreator::hintSize(unsigned int x, unsigned int y)
{
    pimpl->xSize = x, pimpl->ySize = y;
}
void GLRenderWindowCreator::hintGLVersion(unsigned int major, unsigned int minor)
{
    pimpl->glVersionMajor = major, pimpl->glVersionMinor = minor;
}
void GLRenderWindowCreator::hintTitle(std::string title)
{
    pimpl->title = std::move(title);
}
void GLRenderWindowCreator::hintDebug(bool debug)
{
    pimpl->debug = debug;
}
void GLRenderWindowCreator::hintSRGB(bool srgb)
{
	pimpl->srgb = srgb;
}

std::unique_ptr<GLRenderWindow> GLRenderWindowCreator::create() const
{
    // Can allow for different implementations. For now, just use GLFW

	GLFWRenderWindow *ptr = new GLFWRenderWindow(pimpl->xSize, pimpl->ySize,
            									 pimpl->glVersionMajor, pimpl->glVersionMinor,
												 pimpl->title.c_str(), pimpl->debug, pimpl->srgb);

    return std::unique_ptr<GLRenderWindow>(ptr);
}

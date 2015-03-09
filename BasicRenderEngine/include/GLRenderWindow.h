#ifndef GL_RENDER_WINDOW_H_INCLUDED
#define GL_RENDER_WINDOW_H_INCLUDED

#include <memory>
#include <string>
#include <stdexcept>

#include "GLContext.h"
#include "GLRenderTarget.h"

class GLRenderWindowException : public std::exception {};

class GLRenderWindow : public GLRenderTarget
{
public:
    virtual ~GLRenderWindow() = 0;

    virtual bool shouldClose() const = 0;
    virtual void present() = 0;
    virtual void handleEvents() = 0;
    virtual double getTime() const = 0;
    virtual GLContext* getContext() = 0;
};

inline GLRenderWindow::~GLRenderWindow() {}

class GLRenderWindowCreator
{
private:
    struct Pimpl;
    Pimpl *pimpl;

public:
    GLRenderWindowCreator();
    ~GLRenderWindowCreator();

    void hintSize(unsigned int x, unsigned int y);
    void hintGLVersion(unsigned int major, unsigned int minor);
    void hintTitle(std::string title);
    void hintDebug(bool debug);

    std::unique_ptr<GLRenderWindow> create() const;
};

#endif // GL_RENDER_WINDOW_H_INCLUDED
#ifndef GL_RENDER_WINDOW_H_INCLUDED
#define GL_RENDER_WINDOW_H_INCLUDED

#include <memory>
#include <string>
#include <stdexcept>

#include "GLContext.h"
#include "GLRenderTarget.h"

#include "Messaging/Subject.h"

class GLRenderWindowException : public std::exception {};

class GLRenderWindow : public GLRenderTarget, public Subject
{
public:
    virtual ~GLRenderWindow() {}

    virtual unsigned int getWidth() const noexcept = 0;
    virtual unsigned int getHeight() const noexcept = 0;
    virtual bool shouldClose() const = 0;
    virtual void present() = 0;
    virtual void handleEvents() = 0;
    virtual double getTime() const = 0;
    virtual GLContext* getContext() = 0;
};

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
    void hintSRGB(bool srgb);

    std::unique_ptr<GLRenderWindow> create() const;
};

#endif // GL_RENDER_WINDOW_H_INCLUDED

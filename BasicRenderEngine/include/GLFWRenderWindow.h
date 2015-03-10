#ifndef GLFW_RENDER_WINDOW_H_INCLUDED
#define GLFW_RENDER_WINDOW_H_INCLUDED

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLRenderWindow.h"
#include "GLRenderTarget.h"
#include "GLFWWindowContext.h"

class GLFWRenderWindow : public GLRenderWindow
{
private:
    GLFWwindow *handle;
    GLFWWindowContext *context;
    unsigned int width, height;

    static void framebufferResizeCallback(GLFWwindow*, int, int);

public:
    GLFWRenderWindow(unsigned int xSize, unsigned int ySize,
                     unsigned int glVersionMajor, unsigned int glVersionMinor,
                     const char *title, bool debug);
    ~GLFWRenderWindow();

    unsigned int getWidth() const {return width;}
    unsigned int getHeight() const {return height;}
    bool shouldClose() const override;
    void present() override;
    void drawTo() override;
    void handleEvents() override;
    double getTime() const override;
    GLContext* getContext() override;
};

#endif // GLFW_RENDER_WINDOW_H_INCLUDED

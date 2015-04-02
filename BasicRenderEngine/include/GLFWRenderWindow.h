#ifndef GLFW_RENDER_WINDOW_H_INCLUDED
#define GLFW_RENDER_WINDOW_H_INCLUDED

#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLRenderWindow.h"
#include "GLRenderTarget.h"
#include "GLFWWindowContext.h"

#include "Messaging/Message.h"

class GLFWRenderWindow : public GLRenderWindow
{
private:
    GLFWwindow *handle;
    GLFWWindowContext *context;
    unsigned int screenWidth, screenHeight;
    unsigned int width, height;

    float lastCursorX, lastCursorY;

    std::vector<Message*> messageQueue;

    static void framebufferResizeCallback(GLFWwindow*, int, int);
    static void windowCloseCallback(GLFWwindow *handle);
    static void scrollCallback(GLFWwindow *handle, double xoffset, double yoffset);
    static void mouseButtonCallback(GLFWwindow *handle, int button, int action, int mods);
    static void cursorEnterCallback(GLFWwindow *handle, int entered);
    static void cursorPosCallback(GLFWwindow *handle, double xpos, double ypos);
    static void keyCallback(GLFWwindow *handle, int key, int scancode, int action, int mods);

public:
    GLFWRenderWindow(unsigned int xSize, unsigned int ySize, bool fullscreen,
                     unsigned int glVersionMajor, unsigned int glVersionMinor,
                     const char *title, bool debug, bool srgb);
    ~GLFWRenderWindow();

    virtual unsigned int getWidth() const noexcept override {return width;}
    virtual unsigned int getHeight() const noexcept override {return height;}
    virtual unsigned int getScreenWidth() const noexcept override {return screenWidth;}
    virtual unsigned int getScreenHeight() const noexcept override {return screenHeight;}
    bool shouldClose() const override;
    void present() override;
    void drawTo() override;
    void handleEvents() override;
    double getTime() const override;
    GLContext* getContext() override;
};

#endif // GLFW_RENDER_WINDOW_H_INCLUDED

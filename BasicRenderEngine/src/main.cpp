#include <memory>

#include <glm/glm.hpp>
#include <GL/glew.h>

#include "GLRenderWindow.h"
#include "GLContext.h"

int main()
{
    using namespace std;

    GLRenderWindowCreator creator;
    creator.hintGLVersion(4, 4);
    creator.hintDebug(true);
    creator.hintSize(800, 600);
    creator.hintTitle("Test Window");
    std::unique_ptr<GLRenderWindow> win(creator.create());
    GLContext *context = win->getContext();
    context->setCurrent();

    auto test = context->getMutableBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, 0, nullptr);

    while(!win->shouldClose())
    {
    	glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
    	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		win->present();
		win->handleEvents();
    }
}

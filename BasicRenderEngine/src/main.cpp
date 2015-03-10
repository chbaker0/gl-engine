#include <fstream>
#include <memory>
#include <iostream>
#include <string>

#include <cstring>

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

    const float testSquareTris[] =
    {
    		-0.5, 0.5, 0.0, 1.0,	// 0
			0.5, 0.5, 0.0, 1.0,		// 1
			0.5, -0.5, 0.0, 1.0,	// 2
			-0.5, -0.5, 0.0, 1.0	// 3
    };
    const GLushort testSquareInd[] = {0, 1, 2, 2, 3, 0};

    auto buffer = context->getMutableBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, sizeof(testSquareTris) + sizeof(testSquareInd), nullptr);
    void *raw = buffer->mapRange(0, buffer->getSize(), GL_MAP_WRITE_BIT);
    memcpy(raw, testSquareTris, sizeof(testSquareTris));
    raw = static_cast<char*>(raw) + sizeof(testSquareTris);
    memcpy(raw, testSquareInd, sizeof(testSquareInd));
    buffer->unmap();

    auto vao = context->getVertexArrayObject();
    vao->setAttrib(0, *buffer, 4, GL_FLOAT, GL_FALSE, 0, 0);

    string vertProgSource;
    {
    	ifstream vertProgFile("resources/shaders/testing/vert.glsl");
    	char c;
    	while(vertProgFile.get(c))
    		vertProgSource.push_back(c);
    }
    string fragProgSource;
    {
    	ifstream fragProgFile("resources/shaders/testing/frag.glsl");
    	char c;
    	while(fragProgFile.get(c))
    		fragProgSource.push_back(c);
    }
    std::unique_ptr<GLShaderProgram> vertProg, fragProg;

	ofstream outLog("log.txt");
	try
    {
		string log;
		vertProg = context->getStandaloneShaderProgram(GL_VERTEX_SHADER, vertProgSource, &log);
		outLog << log << endl;
		fragProg = context->getStandaloneShaderProgram(GL_FRAGMENT_SHADER, fragProgSource, &log);
		outLog << log << endl;
    }
    catch(GLLinkerError& e)
    {
    	for(char c : e.getErrorLog())
    	{
    		outLog.put(c);
    	}
    	return 1;
    }

    auto pipeline = context->getProgramPipeline();
    pipeline->useProgramStages(GL_VERTEX_SHADER_BIT, *vertProg);
    pipeline->useProgramStages(GL_FRAGMENT_SHADER_BIT, *fragProg);

    win->drawTo();
    while(!win->shouldClose())
    {
    	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    	pipeline->use();
    	vao->bind();
    	buffer->bindTo(GL_ELEMENT_ARRAY_BUFFER);
    	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)sizeof(testSquareTris));

		win->present();
		win->handleEvents();
    }
}

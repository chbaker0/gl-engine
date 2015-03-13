#include <fstream>
#include <memory>
#include <iostream>
#include <string>

#include <cstring>

#include <GL/glew.h>

#include "GLRenderWindow.h"
#include "GLContext.h"
#include "GLDrawIndexedCommand.h"

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
	const GLushort testSquareInd[] = {0, 3, 2, 2, 1, 0};

	auto buffer = context->getMutableBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, sizeof(testSquareTris) + sizeof(testSquareInd), nullptr);
	void *raw = buffer->mapRange(0, buffer->getSize(), GL_MAP_WRITE_BIT);
	memcpy(raw, testSquareTris, sizeof(testSquareTris));
	raw = static_cast<char*>(raw) + sizeof(testSquareTris);
	memcpy(raw, testSquareInd, sizeof(testSquareInd));
	buffer->unmap();

	auto vao = context->getVertexArrayObject();
	vao->setAttrib(0, *buffer, 4, GL_FLOAT, GL_FALSE, 0, 0);
	vao->setElementArrayBinding(*buffer);
	vao->unBind();

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
		outLog << e.getErrorLog();
		return 1;
	}

	auto pipeline = context->getProgramPipeline();
	pipeline->useProgramStages(GL_VERTEX_SHADER_BIT, *vertProg);
	pipeline->useProgramStages(GL_FRAGMENT_SHADER_BIT, *fragProg);

	auto squareDrawCommand = unique_ptr<GLDrawIndexedCommand>(new GLDrawIndexedCommand(vao.get(), pipeline.get(), 6, GL_TRIANGLES, (void*)sizeof(testSquareTris), GL_UNSIGNED_SHORT));

	context->faceCullingEnabled(true);
	context->depthTestEnabled(true);
	context->setClearColor(glm::vec4(0.0, 0.0, 1.0, 1.0));

	win->drawTo();
	while(!win->shouldClose())
	{
		context->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		squareDrawCommand->draw();

		win->present();
		win->handleEvents();
	}
}

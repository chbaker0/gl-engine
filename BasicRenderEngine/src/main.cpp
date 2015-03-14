#include <algorithm>
#include <fstream>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <string>

#include <cstring>
#include <cmath>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLRenderWindow.h"
#include "GLContext.h"
#include "GLDrawIndexedCommand.h"

#include "Utilities.h"

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
	// Enable vsync (hopefully)
	context->setSwapInterval(1);

	// Vertices for a square specified clockwise from top left
	const float testSquareTris[] =
	{
	 -0.5, 0.5, 0.0, 1.0,	// 0
	 0.5, 0.5, 0.0, 1.0,		// 1
	 0.5, -0.5, 0.0, 1.0,	// 2
	 -0.5, -0.5, 0.0, 1.0	// 3
	};
	// Triangle fan indices for square, CCW
	const GLushort testSquareInd[] = {0, 3, 2, 1};

	// Load square vertices and indices into buffer
	auto buffer = context->getMutableBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, sizeof(testSquareTris) + sizeof(testSquareInd), nullptr);
	void *raw = buffer->mapRange(0, buffer->getSize(), GL_MAP_WRITE_BIT);
	memcpy(raw, testSquareTris, sizeof(testSquareTris));
	raw = static_cast<char*>(raw) + sizeof(testSquareTris);
	memcpy(raw, testSquareInd, sizeof(testSquareInd));
	buffer->unmap();

	auto squareUniformBuffer = context->getMutableBuffer(GL_UNIFORM_BUFFER, GL_STREAM_DRAW, 64, nullptr);
	auto globalUniformBuffer = context->getMutableBuffer(GL_UNIFORM_BUFFER, GL_DYNAMIC_DRAW, 64, nullptr);
	globalUniformBuffer->bindBase(GLUniformBlockBinding::GlobalBlock);
	squareUniformBuffer->bindBase(GLUniformBlockBinding::ModelBlock);

	auto vao = context->getVertexArrayObject();
	vao->setAttrib(0, *buffer, 4, GL_FLOAT, GL_FALSE, 0, 0);
	vao->setElementArrayBinding(*buffer);
	vao->unBind();

	std::unique_ptr<GLShaderProgram> vertProg, fragProg;

	ofstream outLog("log.txt");
	try
	{
		string vertProgSource = readFile("resources/shaders/vert.glsl");
		string fragProgSource = readFile("resources/shaders/frag.glsl");

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

	vertProg->setUniformBlockBinding(GLProgramUniformBlockIndex::GlobalBlock, GLUniformBlockBinding::GlobalBlock);
	vertProg->setUniformBlockBinding(GLProgramUniformBlockIndex::ModelBlock, GLUniformBlockBinding::ModelBlock);

	auto squareDrawCommand = unique_ptr<GLDrawIndexedCommand>(new GLDrawIndexedCommand(vao.get(), pipeline.get(), 6, GL_TRIANGLE_FAN, (void*)sizeof(testSquareTris), GL_UNSIGNED_SHORT));

	context->faceCullingEnabled(true);
	context->depthTestEnabled(true);
	context->setClearColor(glm::vec4(0.0, 0.0, 1.0, 1.0));

	win->drawTo();

	glm::mat4 modelWorldMat(1.0);
	glm::mat4 worldCameraMat;
	glm::mat4 cameraClipMat = glm::perspective(70.0f, 800 / (float)600, 0.5f, 15.0f);
	while(!win->shouldClose())
	{
		context->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		worldCameraMat = glm::lookAt(glm::vec3(0.0, sin(win->getTime()), 1.0),
		                             glm::vec3(0.0, 0.0, 0.0),
		                             glm::vec3(0.0, 1.0, 0.0));

		raw = globalUniformBuffer->mapRange(0, globalUniformBuffer->getSize(), GL_MAP_WRITE_BIT);
		memcpy(raw, glm::value_ptr(cameraClipMat), sizeof(cameraClipMat));
		globalUniformBuffer->unmap();

		glm::mat4 modelCameraMat = worldCameraMat * modelWorldMat;
		raw = squareUniformBuffer->mapRange(0, squareUniformBuffer->getSize(), GL_MAP_WRITE_BIT);
		memcpy(raw, glm::value_ptr(modelCameraMat), sizeof(modelCameraMat));
		squareUniformBuffer->unmap();

		squareDrawCommand->draw();

		win->present();
		win->handleEvents();
	}
}

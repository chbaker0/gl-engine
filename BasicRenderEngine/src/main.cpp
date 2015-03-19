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

#include <boost/fusion/include/adapt_struct.hpp>

#include "GLRenderWindow.h"
#include "GLContext.h"
#include "GLDrawIndexedCommand.h"

#include "Camera.h"

#include "Utilities.h"
#include "Utilities/AlignedWriter.h"

#include "Messaging/Listener.h"
#include "Messaging/MessageTypes.h"

struct GlobalBlock
{
	glm::mat4 cameraClipMat;
};
BOOST_FUSION_ADAPT_STRUCT
(GlobalBlock,
 (glm::mat4, cameraClipMat)
);

struct ModelBlock
{
	glm::mat4 modelCameraMat;
	glm::vec3 color;
};
BOOST_FUSION_ADAPT_STRUCT
(ModelBlock,
 (glm::mat4, modelCameraMat)
 (glm::vec3, color)
);


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

	auto squareUniformBuffer = context->getMutableBuffer(GL_UNIFORM_BUFFER, GL_STREAM_DRAW, 256, nullptr);
	auto globalUniformBuffer = context->getMutableBuffer(GL_UNIFORM_BUFFER, GL_DYNAMIC_DRAW, 256, nullptr);
	globalUniformBuffer->bindBase(GLUniformBlockBinding::GlobalBlock);
	squareUniformBuffer->bindBase(GLUniformBlockBinding::ModelBlock);

	auto vao = context->getVertexArrayObject();
	vao->setAttrib(0, *buffer, 4, GL_FLOAT, GL_FALSE, 0, 0);
	vao->setElementArrayBinding(*buffer);

	std::unique_ptr<GLShaderProgram> vertProg, fragProg;

	ofstream outLog("log.txt");
	try
	{
		string vertProgSource = readFile("resources/shaders/testing/vert.glsl");
		string fragProgSource = readFile("resources/shaders/testing/frag.glsl");

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

	auto squareDrawCommand = unique_ptr<GLDrawIndexedCommand>(new GLDrawIndexedCommand(vao.get(), pipeline.get(), 4, GL_TRIANGLE_FAN, (void*)sizeof(testSquareTris), GL_UNSIGNED_SHORT));

	context->faceCullingEnabled(true);
	context->depthTestEnabled(true);
	context->setClearColor(glm::vec4(0.0, 0.0, 1.0, 1.0));

	context->useRenderTarget(win.get());

	Camera cam;
	cam.setNearZ(0.5f);
	cam.setFarZ(10.0f);
	cam.setFov(70.0f);
	cam.setTarget(glm::vec3(0.0f, 0.0f, 0.0f));
	cam.setUpDirection(glm::vec3(0.0, 1.0, 0.0));

	struct CameraAspectUpdater : public Listener
	{
		Camera& cam;
		CameraAspectUpdater(Camera& cam_in): cam(cam_in) {}
		virtual void acceptMessage(Message& m) override
		{
			if(m.getType() == MessageType::Window_Resized)
			{
				GLRenderWindow *subj = static_cast<GLRenderWindow*>(m.getSubject());
				float newAspect = (float) subj->getWidth() / (float) subj->getHeight();
				cam.setAspectRatio(newAspect);
			}
		}
	};
	CameraAspectUpdater cameraAspectUpdater(cam);
	win->registerListener(&cameraAspectUpdater);

	cam.setAspectRatio((float) win->getWidth() / (float) win->getHeight());

	glm::mat4 modelWorldMat(1.0);
	glm::mat4 worldCameraMat;
	glm::mat4 cameraClipMat = glm::perspective(70.0f, 800 / (float)600, 0.5f, 15.0f);

	GlobalBlock globalBlock;
	ModelBlock modelBlock;

	while(!win->shouldClose())
	{
		context->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cam.setPosition(glm::vec3(0.0, sin(win->getTime()), 1.0));
		modelBlock.modelCameraMat = cam.calcViewMatrix() * modelWorldMat;
		globalBlock.cameraClipMat = cam.calcPerspectiveMatrix();

		modelBlock.color = glm::mix(glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0), sin(win->getTime())*0.5+0.5);

		// Update uniform buffer with perspective projection matrix
		raw = globalUniformBuffer->mapRange(0, globalUniformBuffer->getSize(), GL_MAP_WRITE_BIT);
		AlignedWriter globalWriter(raw);
		globalWriter.write<AlignedWriterLayoutSTD140>(globalBlock);
		globalUniformBuffer->unmap();

		// Update modelview matrix
		raw = squareUniformBuffer->mapRange(0, squareUniformBuffer->getSize(), GL_MAP_WRITE_BIT);
		AlignedWriter modelWriter(raw);
		modelWriter.write<AlignedWriterLayoutSTD140>(modelBlock);
		squareUniformBuffer->unmap();

		squareDrawCommand->draw(context);

		win->present();
		win->handleEvents();
	}
}

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
#include "Messaging/KeyMessage.h"
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
	creator.hintSRGB(true);
	creator.hintSize(800, 600);
	creator.hintTitle("Test Window");
	std::unique_ptr<GLRenderWindow> win(creator.create());
	GLContext *context = win->getContext();
	context->setCurrent();
	// Enable vsync (hopefully)
	context->setSwapInterval(1);

	context->srgbWriteEnabled(true);

	// Vertices for a square specified clockwise from top left
	const float testSquareTriPos[] =
	{
	 -0.5, 0.5, 0.0, 1.0,	// 0
	 0.5, 0.5, 0.0, 1.0,		// 1
	 0.5, -0.5, 0.0, 1.0,	// 2
	 -0.5, -0.5, 0.0, 1.0	// 3
	};
	const float testSquareTriUV[] =
	{
	 0.0f, 0.0f,
	 0.0f, 1.0f,
	 1.0f, 1.0f,
	 1.0f, 0.0f
	};
	// Triangle fan indices for square, CCW
	const GLushort testSquareInd[] = {0, 3, 2, 1};

	// Load square vertices and indices into buffer
	auto buffer = context->getMutableBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, sizeof(testSquareTriPos) + sizeof(testSquareTriUV) + sizeof(testSquareInd), nullptr);

	{
		GLBufferMapping map = buffer->mapRange(0, buffer->getSize(), GL_MAP_WRITE_BIT);
		void *raw = map.getMapPtr();
		memcpy(raw, testSquareTriPos, sizeof(testSquareTriPos));
		raw = static_cast<char*>(raw) + sizeof(testSquareTriPos);
		memcpy(raw, testSquareTriUV, sizeof(testSquareTriUV));
		raw = static_cast<char*>(raw) + sizeof(testSquareTriUV);
		memcpy(raw, testSquareInd, sizeof(testSquareInd));
	}

	auto squareUniformBuffer = context->getMutableBuffer(GL_UNIFORM_BUFFER, GL_STREAM_DRAW, 256, nullptr);
	auto globalUniformBuffer = context->getMutableBuffer(GL_UNIFORM_BUFFER, GL_DYNAMIC_DRAW, 256, nullptr);
	globalUniformBuffer->bindBase(GLUniformBlockBinding::GlobalBlock);
	squareUniformBuffer->bindBase(GLUniformBlockBinding::ModelBlock);

	auto vao = context->getVertexArrayObject();
	vao->setAttrib(0, *buffer, 4, GL_FLOAT, GL_FALSE, 0, 0);
	vao->setAttrib(1, *buffer, 2, GL_FLOAT, GL_FALSE, 0, (const void*) sizeof(testSquareTriPos));
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

	auto squareDrawCommand = unique_ptr<GLDrawIndexedCommand>(new GLDrawIndexedCommand(vao.get(), pipeline.get(), 4, GL_TRIANGLE_FAN,
	                                                                                   (void*)(sizeof(testSquareTriPos) + sizeof(testSquareTriUV)), GL_UNSIGNED_SHORT));

	context->faceCullingEnabled(true);
	context->depthTestEnabled(true);
	context->setClearColor(glm::vec4(0.0, 0.0, 0.5, 1.0));

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

	float texData[] =
	{
	 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.0f, 1.0f,
	 0.0f, 0.5f, 0.0f, 1.0f, 0.5f, 0.0f, 0.0f, 1.0f
	};
	auto testTexture = context->getTexture2D(1, false, GL_RGB8, 2, 2, GL_RGBA, GL_FLOAT, texData);
	auto testTextureSRGB = context->getTexture2D(1, false, GL_SRGB8, 2, 2, GL_RGBA, GL_FLOAT, texData);

	bool useSRGBTexture = false;

	struct SRGBFramebufferControl : public Listener
	{
		GLContext& context;
		bool& useSRGBTexture;
		SRGBFramebufferControl(GLContext& context_in, bool& useSRGBTexture_in):
			context(context_in), useSRGBTexture(useSRGBTexture_in) {}
		virtual void acceptMessage(Message& m) override
		{
			if(m.getType() == MessageType::KeyboardInput)
			{
				KeyMessage& km = static_cast<KeyMessage&>(m);
				if(km.getKey() == '[')
					context.srgbWriteEnabled(true);
				else if(km.getKey() == ']')
					context.srgbWriteEnabled(false);
				else if(km.getKey() == ';')
					useSRGBTexture = false;
				else if(km.getKey() == '\'')
					useSRGBTexture = true;
			}
		}
	};
	SRGBFramebufferControl srgbFramebufferControl(*context, useSRGBTexture);
	win->registerListener(&srgbFramebufferControl);

	auto fbTexture = context->getTexture2D(1, false, GL_RGBA32F, 800, 600, GL_RGBA, GL_FLOAT, nullptr);
	auto fbDepthRenderbuffer = context->getRenderbuffer(GL_DEPTH_COMPONENT32F, 800, 600, 1);
	auto fb = context->getFramebuffer();
	fb->textureColorAttachment(0, fbTexture.get(), 0);
	fb->renderbufferDepthAttachment(fbDepthRenderbuffer.get());

	glm::mat4 modelWorldMat(1.0);
	glm::mat4 worldCameraMat;
	glm::mat4 cameraClipMat = glm::perspective(70.0f, 800 / (float)600, 0.5f, 15.0f);

	GlobalBlock globalBlock;
	ModelBlock modelBlock;

	while(!win->shouldClose())
	{
		// Draw to framebuffer object
		context->useRenderTarget(win.get());
		context->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cam.setPosition(glm::vec3(0.0, sin(win->getTime()), 1.0));
		modelBlock.modelCameraMat = cam.calcViewMatrix() * modelWorldMat;
		globalBlock.cameraClipMat = cam.calcPerspectiveMatrix();

		modelBlock.color = glm::mix(glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0), sin(win->getTime())*0.5+0.5);

		// Update uniform buffer with perspective projection matrix
		{
			GLBufferMapping map = globalUniformBuffer->mapRange(0, globalUniformBuffer->getSize(), GL_MAP_WRITE_BIT);
			void *raw = map.getMapPtr();
			AlignedWriter globalWriter(raw);
			globalWriter.write<AlignedWriterLayoutSTD140>(globalBlock);
		}

		// Update modelview matrix
		{
			GLBufferMapping map = squareUniformBuffer->mapRange(0, squareUniformBuffer->getSize(), GL_MAP_WRITE_BIT);
			void *raw = map.getMapPtr();
			AlignedWriter modelWriter(raw);
			modelWriter.write<AlignedWriterLayoutSTD140>(modelBlock);
		}

		if(useSRGBTexture)
			context->bindTexture(0, testTextureSRGB.get());
		else
			context->bindTexture(0, testTexture.get());
		squareDrawCommand->draw(context);

//		// Blit to backbuffer
//		context->useRenderTarget(win.get());
//		fb->blitToCurrent(0, 0, 800, 600, 0, 0, 800, 600, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		win->present();
		win->handleEvents();
	}
}

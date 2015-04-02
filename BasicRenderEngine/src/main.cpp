#include <algorithm>
#include <fstream>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

#include <cstring>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
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
#include "Messaging/MouseMessage.h"
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

	context->faceCullingEnabled(false);
	context->depthTestEnabled(true);
	context->setClearColor(glm::vec4(0.0, 0.0, 0.5, 1.0));

	context->useRenderTarget(win.get());

	Camera cam;
	cam.setNearZ(0.5f);
	cam.setFarZ(10.0f);
	cam.setFov(70.0f);
	cam.setTarget(glm::vec3(0.0f, 0.0f, 0.0f));
	cam.setUpDirection(glm::vec3(0.0, 1.0, 0.0));

	struct CameraUpdater : public Listener
	{
		Camera& cam;
		struct Movement
		{
			float pitchMouseChange;
			float yawMouseChange;
			bool forwardPressed : 1;
			bool backwardPressed : 1;
			bool rightPressed : 1;
			bool leftPressed : 1;
			bool rotRightPressed : 1;
			bool rotLeftPressed : 1;
		} mutable movement;
		CameraUpdater(Camera& cam_in): cam(cam_in), movement{0.0f, 0.0f, false, false, false, false, false, false} {}
		virtual void acceptMessage(Message& m) override
		{
			if(m.getType() == MessageType::Window_Resized)
			{
				GLRenderWindow *subj = static_cast<GLRenderWindow*>(m.getSubject());
				float newAspect = (float) subj->getWidth() / (float) subj->getHeight();
				cam.setAspectRatio(newAspect);
			}
			else if(m.getType() == MessageType::MouseInput)
			{
				MouseMessage& mm = static_cast<MouseMessage&>(m);
				glm::vec2 mouseChange = mm.getChange();
				movement.pitchMouseChange += mouseChange.y;
				movement.yawMouseChange += mouseChange.x;
				cout << movement.pitchMouseChange << ' ' << movement.yawMouseChange << endl;
			}
			else if(m.getType() == MessageType::KeyboardInput)
			{
				KeyMessage& km = static_cast<KeyMessage&>(m);
				switch(km.getKey())
				{
				case 'W':
					movement.forwardPressed =
							km.getAction() == KeyMessage::Release ? false : true;
					break;
				case 'A':
					movement.leftPressed =
							km.getAction() == KeyMessage::Release ? false : true;
					break;
				case 'S':
					movement.backwardPressed =
							km.getAction() == KeyMessage::Release ? false : true;
					break;
				case 'D':
					movement.rightPressed =
							km.getAction() == KeyMessage::Release ? false : true;
					break;
				case 'Q':
					movement.rotLeftPressed =
							km.getAction() == KeyMessage::Release ? false : true;
					break;
				case 'E':
					movement.rotRightPressed =
							km.getAction() == KeyMessage::Release ? false : true;
					break;
				case '=':
					if(km.getAction() != KeyMessage::Release)
						if(cam.getFov() < 100.0f)
							cam.setFov(cam.getFov() + 1.0f);
					break;
				case '-':
					if(km.getAction() != KeyMessage::Release)
						if(cam.getFov() > 10.0f)
							cam.setFov(cam.getFov() - 1.0f);
					break;
				default:
					break;
				}
			}
		}
		void calcMovement(glm::vec3& linearV, glm::vec3& angularV) const
		{
			int forwardDir = movement.forwardPressed ?
					1 : movement.backwardPressed ? -1 : 0;
			int rightDir = movement.rightPressed ?
					1 : movement.leftPressed ? -1 : 0;
			glm::vec3 camForwardDir = cam.calcForwardDirection();
			glm::vec3 camUpDir = cam.getUpDirection();
			glm::vec3 camRightDir = -glm::cross(camUpDir, camForwardDir);

			int rotRightDir = movement.rotRightPressed ?
					1 : movement.rotLeftPressed ? -1 : 0;
			angularV = glm::vec3(rotRightDir) * camForwardDir;
			angularV += glm::vec3(-movement.pitchMouseChange) * camRightDir;
			angularV += glm::vec3(-movement.yawMouseChange) * camUpDir;

			// Zero out mouse changes after processing them
			movement.pitchMouseChange = 0.0f;
			movement.yawMouseChange = 0.0f;

			linearV = glm::vec3(0.0f);
			linearV += glm::vec3((float)forwardDir) * camForwardDir;
			linearV += glm::vec3((float)rightDir) * camRightDir;
		}
	};
	CameraUpdater cameraUpdater(cam);
	win->registerListener(&cameraUpdater);

	cam.setAspectRatio((float) win->getWidth() / (float) win->getHeight());

	float texData[] =
	{
	 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.0f, 1.0f,
	 0.0f, 0.5f, 0.0f, 1.0f, 0.5f, 0.0f, 0.0f, 1.0f
	};
	auto testTexture = context->getTexture2D(1, false, GL_RGB8, 2, 2, GL_RGBA, GL_FLOAT, texData);
	auto testTextureSRGB = context->getTexture2D(1, false, GL_SRGB8, 2, 2, GL_RGBA, GL_FLOAT, texData);

	auto testSampler = context->getSampler();
	testSampler->setAnisotropy(4.0f);
	testSampler->setMagFilter(GL_NEAREST);
	testSampler->setMinFilter(GL_NEAREST);
	testSampler->setWrapMode(GL_CLAMP_TO_EDGE);

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

	auto fbColorRenderbuffer = context->getRenderbuffer(GL_RGBA8, 800, 600, 1);
	auto fbDepthRenderbuffer = context->getRenderbuffer(GL_DEPTH_COMPONENT16, 800, 600, 1);
	auto fb = context->getFramebuffer();
	fb->renderbufferColorAttachment(0, fbColorRenderbuffer.get());
	fb->renderbufferDepthAttachment(fbDepthRenderbuffer.get());

	struct FramebufferResizeHandler : public Listener
	{
		GLContext& context;
		unique_ptr<GLRenderbuffer>& rbc;
		unique_ptr<GLRenderbuffer>& rb;
		unique_ptr<GLFramebuffer>& fb;
		FramebufferResizeHandler(GLContext& context_in, unique_ptr<GLRenderbuffer>& rbc_in,
		                         unique_ptr<GLRenderbuffer>& rb_in, unique_ptr<GLFramebuffer>& fb_in):
			context(context_in), rbc(rbc_in), rb(rb_in), fb(fb_in) {}
		virtual void acceptMessage(Message& m)
		{
			if(m.getType() == MessageType::Window_Resized)
			{
				GLRenderWindow *win = static_cast<GLRenderWindow*>(m.getSubject());
				unsigned int x = win->getWidth(),
							 y = win->getHeight();
				rbc = context.getRenderbuffer(GL_RGBA8, x, y, 1);
				rb = context.getRenderbuffer(GL_DEPTH_COMPONENT16, x, y, 1);
				fb->renderbufferColorAttachment(0, rbc.get());
				fb->renderbufferDepthAttachment(rb.get());
			}
		}
	};
	FramebufferResizeHandler framebufferResizeHandler(*context, fbColorRenderbuffer, fbDepthRenderbuffer, fb);
	win->registerListener(&framebufferResizeHandler);

	glm::mat4 modelWorldMat(1.0);

	GlobalBlock globalBlock;
	ModelBlock modelBlock;

	double prevFrameTime = 0.0;
	double curFrameTime = 0.0;
	double elapsedTime = 0.0;

	while(!win->shouldClose())
	{
		prevFrameTime = curFrameTime;
		curFrameTime = win->getTime();
		elapsedTime = curFrameTime - prevFrameTime;
		// Draw to framebuffer object
		context->useRenderTarget(fb.get());
		context->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::vec3 linearV;
		glm::vec3 angularV;
		cameraUpdater.calcMovement(linearV, angularV);
		cam.offsetCamera(glm::vec3((float)elapsedTime * 3.0f) * linearV);
		if(glm::length(angularV) > 0.01f)
		{
			cam.orientCamera(glm::rotate(glm::quat(), (float)elapsedTime * length(angularV) * 2.0f, angularV));
		}

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
		context->bindSampler(0, testSampler.get());
		squareDrawCommand->draw(context);

		// Blit to backbuffer
		context->useRenderTarget(win.get());
		fb->blitToCurrent(0, 0, win->getWidth(), win->getHeight(),
		                  0, 0, win->getWidth(), win->getHeight(),
		                  GL_COLOR_BUFFER_BIT, GL_NEAREST);

		win->present();
		win->handleEvents();
	}
}

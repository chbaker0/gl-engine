#include <algorithm>
#include <fstream>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>

#include <cstring>
#include <cstdlib>
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
#include "BufferUtils/AlignedWriter.h"

#include "Model/Mesh.h"

#include "Messaging/Listener.h"
#include "Messaging/KeyMessage.h"
#include "Messaging/MouseMessage.h"
#include "Messaging/MessageTypes.h"

struct GlobalBlock
{
	glm::mat4 cameraClipMat;
	float time;
};
BOOST_FUSION_ADAPT_STRUCT
(GlobalBlock,
 (glm::mat4, cameraClipMat)
 (float, time)
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

std::unique_ptr<GLRenderWindow> createWindow(int argc, char **argv);

int main(int argc, char **argv)
{
try
{
	using namespace std;

	std::unique_ptr<GLRenderWindow> win = createWindow(argc, argv);
	GLContext *context = win->getContext();
	context->setCurrent();
	// Enable vsync (hopefully)
	context->setSwapInterval(1);

	context->srgbWriteEnabled(true);

	// Vertices for a square specified clockwise from top left
	const float testSquareTriAttribs[] =
	{
	 -0.5, 0.5, 0.0, 1.0,	0.0f, 0.0f,		// 0
	 0.5, 0.5, 0.0, 1.0,	0.0f, 1.0f,		// 1
	 0.5, -0.5, 0.0, 1.0,	1.0f, 1.0f,		// 2
	 -0.5, -0.5, 0.0, 1.0,	1.0f, 0.0f		// 3
	};
	// Triangle indices for square, CCW
	const GLushort testSquareInd[] = {0, 1, 3, 3, 2, 1};

	// Load square vertices and indices into buffer
	auto buffer = context->getMutableBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, sizeof(testSquareTriAttribs) + sizeof(testSquareInd), nullptr);

	GLMappableBufferView bufferVertexView(buffer.get(), 0, sizeof(testSquareTriAttribs));
	GLMappableBufferView bufferIndexView(buffer.get(), sizeof(testSquareTriAttribs), sizeof(testSquareInd));

	MeshVertexAttribDescriptor squareAttribDesc[] =
	{
	 {GLVertexArrayObject::Position, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0},
	 {GLVertexArrayObject::TexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, sizeof(float) * 4}
	};
	Mesh squareMesh = loadMeshFromMem(context, testSquareTriAttribs, squareAttribDesc, 2, 4,
	                                  testSquareInd, GL_UNSIGNED_SHORT, 6, GL_TRIANGLES,
	                                  bufferVertexView, bufferIndexView);
//	{
//		GLBufferMapping map = buffer->mapRange(0, buffer->getSize(), GL_MAP_WRITE_BIT);
//		void *raw = map.getMapPtr();
//		memcpy(raw, testSquareTriAttribs, sizeof(testSquareTriAttribs));
//		raw = static_cast<char*>(raw) + sizeof(testSquareTriAttribs);
//		memcpy(raw, testSquareInd, sizeof(testSquareInd));
//	}
//
//	std::shared_ptr<GLVertexArrayObject> vao = context->getVertexArrayObject();
//	vao->setAttrib(0, *buffer, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
//	vao->setAttrib(2, *buffer, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*) (sizeof(float) * 4));
//	vao->setElementArrayBinding(*buffer);
//
//	Mesh squareMesh(vao, GL_TRIANGLES, GL_UNSIGNED_SHORT, sizeof(testSquareTriAttribs), 6);

	auto squareUniformBuffer = context->getMutableBuffer(GL_UNIFORM_BUFFER, GL_STREAM_DRAW, 256, nullptr);
	auto globalUniformBuffer = context->getMutableBuffer(GL_UNIFORM_BUFFER, GL_DYNAMIC_DRAW, 256, nullptr);
	globalUniformBuffer->bindBase(GLUniformBlockBinding::GlobalBlock);
	squareUniformBuffer->bindBase(GLUniformBlockBinding::ModelBlock);

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

	auto squareDrawCommand =
			unique_ptr<GLDrawIndexedCommand>(new GLDrawIndexedCommand(squareMesh.getVao(), squareMesh.getIndexCount(), squareMesh.getPrimType(),
	                                                                  (const void*) squareMesh.getIndexOffset(), squareMesh.getIndexType()));

	cout << squareMesh.getIndexCount() << ' ' << squareMesh.getPrimType() << ' ' << squareMesh.getIndexOffset() << ' ' << squareMesh.getIndexType() << endl;

	context->faceCullingEnabled(false);
	context->depthTestEnabled(true);
	context->setClearColor(glm::vec4(0.0, 0.0, 0.5, 1.0));

	context->useRenderTarget(win.get());

	Camera cam;
	cam.setNearZ(0.2f);
	cam.setFarZ(50.0f);
	cam.setFov(0.608f);
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
			bool upPressed : 1;
			bool downPressed : 1;
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
				case 'R':
					movement.upPressed =
							km.getAction() == KeyMessage::Release ? false : true;
					break;
				case 'F':
					movement.downPressed =
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
						if(cam.getFov() < 1.0f)
							cam.setFov(cam.getFov() + 0.01f);
					break;
				case '-':
					if(km.getAction() != KeyMessage::Release)
						if(cam.getFov() > 0.1f)
							cam.setFov(cam.getFov() - 0.01f);
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
			int upDir = movement.upPressed ?
					1 : movement.downPressed ? -1 : 0;
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
			linearV += glm::vec3((float)upDir) * camUpDir;
		}
	};
	CameraUpdater cameraUpdater(cam);
	win->registerListener(&cameraUpdater);

	cam.setAspectRatio((float) win->getWidth() / (float) win->getHeight());

//	float texData[] =
//	{
//	 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.0f, 1.0f,
//	 0.0f, 0.5f, 0.0f, 1.0f, 0.5f, 0.0f, 0.0f, 1.0f
//	};
	vector<float> texData;
	const unsigned int colorBlocks = 5;
	for(unsigned int i = 0; i < colorBlocks; ++i)
	{
		for(unsigned int j = 0; j < 3; ++j)
			texData.push_back((float) i / (float) (colorBlocks-1.0f));
		texData.push_back(1.0f);
	}
	auto testTexture = context->getTexture2D(0, true, GL_RGB8, colorBlocks, 1, GL_RGBA, GL_FLOAT, &texData[0]);
	auto testTextureSRGB = context->getTexture2D(0, true, GL_SRGB8, colorBlocks, 1, GL_RGBA, GL_FLOAT, &texData[0]);

	auto testSampler = context->getSampler();
	testSampler->setMagFilter(GL_NEAREST);
	testSampler->setMinFilter(GL_NEAREST_MIPMAP_NEAREST);
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

	auto fbColorRenderbuffer = context->getRenderbuffer(GL_SRGB8_ALPHA8, win->getWidth(), win->getHeight(), 4);
	auto fbDepthRenderbuffer = context->getRenderbuffer(GL_DEPTH_COMPONENT16, win->getWidth(), win->getHeight(), 4);
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
				rbc = context.getRenderbuffer(rbc->getInternalFormat(), x, y, rbc->getSamples());
				rb = context.getRenderbuffer(rb->getInternalFormat(), x, y, rb->getSamples());
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
		globalBlock.time = curFrameTime;
		// Draw to framebuffer object
		// On my system this is the only way I can get SRGB to work...
		context->useRenderTarget(fb.get());
		context->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::vec3 linearV;
		glm::vec3 angularV;
		cameraUpdater.calcMovement(linearV, angularV);
		cam.offsetCamera(glm::vec3((float)elapsedTime * 3.0f) * linearV);
		if(glm::length(angularV) > 0.01f)
		{
			cam.orientCamera(glm::rotate(glm::quat(), (float)elapsedTime * length(angularV) * 3.2f, angularV));
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

		context->useExecutable(pipeline.get());
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
} // try
catch(std::exception& e)
{
	std::cout << e.what() << std::endl;
	return 1;
}
}

std::unique_ptr<GLRenderWindow> createWindow(int argc, char **argv)
{
	unsigned int x = 0, y = 0;
	bool fullscreen = true;

	if(argc >= 3)
	{
		x = std::strtoul(argv[1], nullptr, 10);
		y = std::strtoul(argv[2], nullptr, 10);
	}
	if(argc >= 4)
	{
		if(argv[3][0] == 'f' || argv[3][0] == 'F')
			fullscreen = true;
		else if(argv[3][0] == 'w' || argv[3][0] == 'W')
			fullscreen = false;
	}

	GLRenderWindowCreator creator;
	creator.hintGLVersion(4, 2);
	creator.hintDebug(true);
	creator.hintFullscreen(fullscreen);
	creator.hintSize(x, y);
	creator.hintTitle("Test Window");

	return creator.create();
}

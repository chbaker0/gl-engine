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

class TestApp : private Listener
{
private:
	GLRenderWindow *window;
	GLContext *context;

	void initState()
	{
		context->srgbWriteEnabled(true);
		context->faceCullingEnabled(false);
		context->depthTestEnabled(false);
		context->setClearColor(glm::vec4(0.0, 0.0, 0.5, 1.0));
		context->useRenderTarget(window);
	}

	std::unique_ptr<GLDrawIndexedCommand> squareDrawCommand;
	Mesh squareMesh;

	void loadMesh()
	{
		// Vertices for a square specified clockwise from top left
		const float testSquareTriAttribs[] =
		{
		 -0.5, 0.5, 0.0, 1.0,	0.0f, 0.0f,		// 0
		 0.5, 0.5, 0.0, 1.0,	0.0f, 1.0f,		// 1
		 0.5, -0.5, 0.0, 1.0,	1.0f, 1.0f,		// 2
		 -0.5, -0.5, 0.0, 1.0,	1.0f, 0.0f		// 3
		};
		const GLushort testSquareInd[] = {0, 1, 3, 3, 2, 1};

		MeshVertexAttribDescriptor squareAttribDesc[] =
		{
		 {GLVertexArrayObject::Position, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0},
		 {GLVertexArrayObject::TexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, sizeof(float) * 4}
		};
		squareMesh = loadMeshFromMem(context, testSquareTriAttribs, squareAttribDesc, 2, 4,
		                                  testSquareInd, GL_UNSIGNED_SHORT, 6, GL_TRIANGLES);
	}

	std::unique_ptr<GLMutableBuffer> squareUniformBuffer, globalUniformBuffer;

	void loadBuffers()
	{
		squareUniformBuffer = context->getMutableBuffer(GL_UNIFORM_BUFFER, GL_STREAM_DRAW, 256, nullptr);
		globalUniformBuffer = context->getMutableBuffer(GL_UNIFORM_BUFFER, GL_DYNAMIC_DRAW, 256, nullptr);
		squareUniformBuffer->bindBase(GLUniformBlockBinding::ModelBlock);
		globalUniformBuffer->bindBase(GLUniformBlockBinding::GlobalBlock);
	}

	std::unique_ptr<GLShaderProgram> vertProg, fragProg;
	std::unique_ptr<GLProgramPipeline> pipeline;

	std::unique_ptr<GLTexture2D> testTexture;
	std::unique_ptr<GLSampler> testSampler;

	void loadTextures()
	{
		std::vector<unsigned char> texData;
		const unsigned int colorBlocks = 5;
		for(unsigned int i = 0; i < colorBlocks; ++i)
		{
			for(unsigned int j = 0; j < 3; ++j)
				texData.push_back(255*i/(colorBlocks-1));
			texData.push_back(255);
		}
		testTexture = context->getTexture2D(0, true, GL_RGB8, colorBlocks, 1, GL_RGBA, GL_UNSIGNED_BYTE, &texData[0]);

		testSampler = context->getSampler();
		testSampler->setMagFilter(GL_NEAREST);
		testSampler->setMinFilter(GL_NEAREST_MIPMAP_NEAREST);
		testSampler->setWrapMode(GL_CLAMP_TO_EDGE);
	}

	void loadShaders()
	{
		using namespace std;

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
			throw;
		}

		pipeline = context->getProgramPipeline();
		pipeline->useProgramStages(GL_VERTEX_SHADER_BIT, *vertProg);
		pipeline->useProgramStages(GL_FRAGMENT_SHADER_BIT, *fragProg);

		vertProg->setUniformBlockBinding(GLProgramUniformBlockIndex::GlobalBlock, GLUniformBlockBinding::GlobalBlock);
		vertProg->setUniformBlockBinding(GLProgramUniformBlockIndex::ModelBlock, GLUniformBlockBinding::ModelBlock);
		GLint diffuseLocation = fragProg->getUniformLocation("diffuse");
		fragProg->setUniform(diffuseLocation, 0);
	}

	Camera cam;

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

	virtual void acceptMessage(Message& m) override final
	{
		if(m.getType() == MessageType::Window_Resized)
		{
			float aspectRatio = (float) window->getWidth() / (float) window->getHeight();
			cam.setAspectRatio(aspectRatio);
		}
		else if(m.getType() == MessageType::MouseInput)
		{
			MouseMessage& mm = static_cast<MouseMessage&>(m);
			glm::vec2 mouseChange = mm.getChange();
			movement.pitchMouseChange += mouseChange.y;
			movement.yawMouseChange += mouseChange.x;
			std::cout << movement.pitchMouseChange << ' ' << movement.yawMouseChange << std::endl;
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
			case '[':
				context->srgbWriteEnabled(true);
				break;
			case ']':
				context->srgbWriteEnabled(false);
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

public:
	TestApp(GLRenderWindow *window_in): window(window_in)
	{
		using namespace std;

		context = window->getContext();
		auto version = context->getGLVersion();
		cout << version.first << ',' << version.second << endl;
		context->setCurrent();

		initState();
		loadMesh();
		loadBuffers();
		loadTextures();
		loadShaders();

		squareDrawCommand.reset(new GLDrawIndexedCommand(squareMesh.getVao(), squareMesh.getIndexCount(), squareMesh.getPrimType(),
                                                         (const void*) squareMesh.getIndexOffset(), squareMesh.getIndexType()));

		window->registerListener(this);

		cam.setAspectRatio((float) window->getWidth() / (float) window->getHeight());
	}

	void run()
	{
		glm::mat4 modelWorldMat(1.0);

		GlobalBlock globalBlock;
		ModelBlock modelBlock;

		double prevFrameTime = 0.0;
		double curFrameTime = 0.0;
		double elapsedTime = 0.0;

		while(!window->shouldClose())
		{
			prevFrameTime = curFrameTime;
			curFrameTime = window->getTime();
			elapsedTime = curFrameTime - prevFrameTime;
			globalBlock.time = curFrameTime;
//			// Draw to framebuffer object
//			// On my system this is the only way I can get SRGB to work...
			context->useRenderTarget(window);
			context->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glm::vec3 linearV;
			glm::vec3 angularV;
			calcMovement(linearV, angularV);
			cam.offsetCamera(glm::vec3((float)elapsedTime * 3.0f) * linearV);
			if(glm::length(angularV) > 0.01f)
			{
				cam.orientCamera(glm::rotate(glm::quat(), (float)elapsedTime * length(angularV) * 2.5f, angularV));
			}

			modelBlock.modelCameraMat = cam.calcViewMatrix() * modelWorldMat;
			globalBlock.cameraClipMat = cam.calcPerspectiveMatrix();

			modelBlock.color = glm::mix(glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0), sin(window->getTime())*0.5+0.5);

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
			context->bindTexture(0, testTexture.get());
			context->bindSampler(0, testSampler.get());
			squareDrawCommand->draw(context);

//			// Blit to backbuffer
//			context->useRenderTarget(window);
//			fb->blitToCurrent(0, 0, win->getWidth(), win->getHeight(),
//							  0, 0, win->getWidth(), win->getHeight(),
//							  GL_COLOR_BUFFER_BIT, GL_NEAREST);

			window->present();
			window->handleEvents();
		}
	}
};

int main(int argc, char **argv)
{
try
{
	std::unique_ptr<GLRenderWindow> window = createWindow(argc, argv);
	TestApp testApp(window.get());
	testApp.run();
} // try
catch(std::exception& e)
{
	std::cout << e.what() << std::endl;
	return 1;
}
catch(...)
{
	std::cout << "Unknown exception" << std::endl;
	return 1;
}
}

std::unique_ptr<GLRenderWindow> createWindow(int argc, char **argv)
{
	unsigned int x = 800, y = 600;
	bool fullscreen = false;

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
	creator.hintGLVersion(4, 0);
	creator.hintDebug(true);
	creator.hintFullscreen(fullscreen);
	creator.hintSize(x, y);
	creator.hintTitle("Test Window");

	return creator.create();
}

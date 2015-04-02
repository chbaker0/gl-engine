#include <iostream>
#include <string>
#include <utility>

#include <boost/scope_exit.hpp>
#include <boost/pool/object_pool.hpp>

#include <glm/glm.hpp>

#include "GLFWRenderWindow.h"
#include "GLFWWindowContext.h"

#include "Messaging/KeyMessage.h"
#include "Messaging/MouseMessage.h"
#include "Messaging/MessageTypes.h"

class GLFWRenderWindowException : public GLRenderWindowException
{
private:
    std::string err;

public:
    GLFWRenderWindowException(std::string err_in): err(err_in) {};
    const char* what() const noexcept override
    {
        return err.c_str();
    }
};

static unsigned int winCount = 0;
static void initialize()
{
    if(winCount++ == 0)
    {
        if(!glfwInit())
            throw GLFWRenderWindowException("Could not initialize GLFW");
    }
}

static void terminate()
{
    if(--winCount == 0)
    {
        glfwTerminate();
    }
}

void GLFWRenderWindow::keyCallback(GLFWwindow *handle, int key, int scancode, int action, int mods)
{
	auto ptr = (GLFWRenderWindow*) glfwGetWindowUserPointer(handle);

	KeyMessage::Action translatedAction;
	switch(action)
	{
	case GLFW_PRESS:
		translatedAction = KeyMessage::Press;
		break;
	case GLFW_REPEAT:
		translatedAction = KeyMessage::Repeat;
		break;
	default:
		translatedAction = KeyMessage::Release;
		break;
	}

	ptr->messageQueue.push_back(new KeyMessage(ptr, key, translatedAction));

	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(handle, true);
	}
}

void GLFWRenderWindow::cursorPosCallback(GLFWwindow *handle, double xpos, double ypos)
{
	auto ptr = (GLFWRenderWindow*) glfwGetWindowUserPointer(handle);

	ptr->messageQueue.push_back(new MouseMessage(ptr, xpos, ypos, xpos - ptr->lastCursorX, ypos - ptr->lastCursorY));
	ptr->lastCursorX = xpos;
	ptr->lastCursorY = ypos;
}

void GLFWRenderWindow::cursorEnterCallback(GLFWwindow *handle, int entered)
{
	auto ptr = (GLFWRenderWindow*) glfwGetWindowUserPointer(handle);
}

void GLFWRenderWindow::mouseButtonCallback(GLFWwindow *handle, int button, int action, int mods)
{
	auto ptr = (GLFWRenderWindow*) glfwGetWindowUserPointer(handle);
}

void GLFWRenderWindow::scrollCallback(GLFWwindow *handle, double xoffset, double yoffset)
{
	auto ptr = (GLFWRenderWindow*) glfwGetWindowUserPointer(handle);
}

void GLFWRenderWindow::windowCloseCallback(GLFWwindow *handle)
{
	auto ptr = (GLFWRenderWindow*) glfwGetWindowUserPointer(handle);
}

void GLFWRenderWindow::framebufferResizeCallback(GLFWwindow *handle, int width, int height)
{
	auto ptr = (GLFWRenderWindow*) glfwGetWindowUserPointer(handle);
	ptr->width = width;
	ptr->height = height;
	if(GLContext::getCurrentContext()->getCurrentRenderTarget() == ptr)
		glViewport(0, 0, width, height);

	// Replace last message in queue if it is another resize message
	if(ptr->messageQueue.size() && ptr->messageQueue.back()->getType() == MessageType::Window_Resized)
	{
		delete ptr->messageQueue.back();
		ptr->messageQueue.back() = new Message(MessageType::Window_Resized, ptr);
	}
	else
		ptr->messageQueue.push_back(new Message(MessageType::Window_Resized, ptr));
}

static void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);

GLFWRenderWindow::GLFWRenderWindow(unsigned int xSize, unsigned int ySize, bool fullscreen,
                                   unsigned int glVersionMajor, unsigned int glVersionMinor,
                                   const char *title, bool debug, bool srgb):
                                		   lastCursorX(0.0f), lastCursorY(0.0f)
{
	// Initialize GLFW
    initialize();
    // Scope guard to terminate GLFW if a window creation error occurs
    bool shouldTerminate = true;
    BOOST_SCOPE_EXIT(&shouldTerminate)
    {
        if(shouldTerminate)
            terminate();
    } BOOST_SCOPE_EXIT_END

	// Set our window hints
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glVersionMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glVersionMinor);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    if(srgb)
    {
    	glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);
    	std::cout << "Asking for SRGB" << std::endl;
    }
    // Set a debug context if asked
    if(debug)
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    // Create window, throw exception if failed
    if(fullscreen)
    {
    	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    	if(monitor == nullptr)
    		throw GLFWRenderWindowException("Could not get handle to monitor");
    	if(xSize == 0 || ySize == 0)
    	{
        	const GLFWvidmode *vmode = glfwGetVideoMode(monitor);
        	if(vmode == nullptr)
        		throw GLFWRenderWindowException("Could not get video mode information");
    		xSize = vmode->width;
    		ySize = vmode->height;
    	}
    	handle = glfwCreateWindow(xSize, ySize, title, monitor, nullptr);
    }
    else
    {
        handle = glfwCreateWindow(xSize, ySize, title, nullptr, nullptr);
    }
    if(handle == nullptr)
        throw GLFWRenderWindowException("Could not create GLFW window");
    screenWidth = xSize, screenHeight = ySize;
    int tempw, temph;
    glfwGetFramebufferSize(handle, &tempw, &temph);
    width = tempw, height = temph;

    // Set pointer to this class for callbacks
    glfwSetWindowUserPointer(handle, this);

    // Set callback functions
    glfwSetKeyCallback(handle, keyCallback);
    glfwSetMouseButtonCallback(handle, mouseButtonCallback);
    glfwSetCursorPosCallback(handle, cursorPosCallback);
    glfwSetCursorEnterCallback(handle, cursorEnterCallback);
    glfwSetScrollCallback(handle, scrollCallback);
    glfwSetWindowCloseCallback(handle, windowCloseCallback);
    glfwSetFramebufferSizeCallback(handle, framebufferResizeCallback);

    glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Scope guard to reset context to previously current one after glew initialization
    GLFWwindow *temp = glfwGetCurrentContext();
    BOOST_SCOPE_EXIT(temp)
    {
        glfwMakeContextCurrent(temp);
    } BOOST_SCOPE_EXIT_END

	// Load glew
    glfwMakeContextCurrent(handle);
    glewExperimental = true;
    GLenum err = glewInit();
    if(err != GLEW_OK)
        throw GLFWRenderWindowException((const char*)glewGetErrorString(err));

    // Create a GLFWWindowContext
    context = new GLFWWindowContext44(handle);
    // Enable debug callback if asked
    if(debug)
    {
    	glEnable(GL_DEBUG_OUTPUT);
    	glDebugMessageCallback(debugCallback, nullptr);
    }

    // Disable action in our scope guard
    shouldTerminate = false;
}

GLFWRenderWindow::~GLFWRenderWindow()
{
    glfwDestroyWindow(handle);
    terminate();
    delete context;
}

GLContext* GLFWRenderWindow::getContext()
{
	return context;
}

bool GLFWRenderWindow::shouldClose() const
{
	return glfwWindowShouldClose(handle);
}

void GLFWRenderWindow::present()
{
	glfwSwapBuffers(handle);
}

void GLFWRenderWindow::drawTo()
{
	if(GLContext::getCurrentContext() != context)
		throw GLFWRenderWindowException("Can't set GLFWRenderWindow as current render target if it's context isn't current");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
}

void GLFWRenderWindow::handleEvents()
{
	glfwPollEvents();
	for(Message *m : messageQueue)
	{
		for(Listener *l : listeners)
		{
			l->acceptMessage(*m);
		}

		delete m;
	}
	messageQueue.clear();
}

double GLFWRenderWindow::getTime() const
{
	return glfwGetTime();
}

class GLPanicException : public std::runtime_error
{
public:
	GLPanicException(): runtime_error("Unrecoverable OpenGL error") {}
};

static void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
	thread_local std::string errorString;
	errorString.clear();
	errorString = "OpenGL Implementation message:\n";

	bool panic = false;

	switch(source)
	{
	case GL_DEBUG_SOURCE_API:
		errorString += "- Source: API\n";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		errorString += "- Source: Window system\n";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		errorString += "- Source: Shader compiler\n";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		errorString += "- Source: Third party\n";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		errorString += "- Source: Application\n";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		errorString += "- Source: Other\n";
		break;
	default:
		errorString += "- Source: Unknown\n";
		break;
	}
	switch(type)
	{
	case GL_DEBUG_TYPE_ERROR:
		errorString += "- Type: Error\n";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		errorString += "- Type: Deprecated behavior\n";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		errorString += "- Type: Undefined behavior\n";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		errorString += "- Type: Portability issue\n";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		errorString += "- Type: Performance issue\n";
		break;
	case GL_DEBUG_TYPE_MARKER:
		errorString += "- Type: Command stream annotation\n";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		errorString += "- Type: Group push\n";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		errorString += "- Type: Group pop\n";
		break;
	case GL_DEBUG_TYPE_OTHER:
		errorString += "- Type: Other\n";
		break;
	default:
		errorString += "- Type: Unknown\n";
		break;
	}
	switch(severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:
		errorString += "- Severity: High\n";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		errorString += "- Severity: Medium\n";
		break;
	case GL_DEBUG_SEVERITY_LOW:
		errorString += "- Severity: Low\n";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		errorString += "- Severity: Notification\n";
		break;
	}
	switch(id)
	{
	case GL_NO_ERROR:
		errorString += "- ID: No error\n";
		break;
	case GL_INVALID_ENUM:
		errorString += "- ID: Invalid enum\n";
		break;
	case GL_INVALID_VALUE:
		errorString += "- ID: Value out-of-range\n";
		break;
	case GL_INVALID_OPERATION:
		errorString += "- ID: Invalid operation\n";
		break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		errorString += "- ID: Operation on incomplete framebuffer object\n";
		break;
	case GL_OUT_OF_MEMORY:
		errorString += "- ID: Out of memory (unrecoverable)\n";
		panic = true;
		break;
	case GL_STACK_UNDERFLOW:
		errorString += "- ID: Operation would cause stack underflow\n";
		break;
	case GL_STACK_OVERFLOW:
		errorString += "- ID: Operation would cause stack overflow\n";
		break;
	default:
		errorString += "- ID: Unknown\n";
		break;
	}
	errorString += "- Message: ";
	errorString += message;

	std::cerr << errorString << std::endl;

	if(panic)
		throw GLPanicException();
}

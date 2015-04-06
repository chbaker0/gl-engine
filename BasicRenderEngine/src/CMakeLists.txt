list(APPEND SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/GLBuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GLContext.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GLDrawIndexedCommand.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GLFramebuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GLFWRenderWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GLFWWindowContext.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GLProgramPipeline.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GLRenderWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GLSampler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GLShader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GLShaderProgram.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GLTexture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GLVertexArrayObject.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Utilities.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Messaging/MessageAllocators.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Messaging/Subject.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BufferUtils/GLBufferView.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Model/Mesh.cpp
)

set(SOURCES "${SOURCES}" PARENT_SCOPE)
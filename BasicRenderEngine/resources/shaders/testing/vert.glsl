#version 430

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoord;

layout (std140, binding = 0) uniform GlobalBlock
{
    mat4 cameraClipMat;
};
layout (std140, binding = 1) uniform ModelBlock
{
    mat4 modelCameraMat;
    vec3 color;
};

out gl_PerVertex
{
    vec4 gl_Position;
};

out vec2 texCoordFrag;

void main()
{
    gl_Position = cameraClipMat * modelCameraMat * position;
    texCoordFrag = texCoord;
}
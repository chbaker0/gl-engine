#version 430

layout (location = 0) in vec4 position;

layout (std140, binding = 0) uniform GlobalMatrix
{
    mat4 cameraClipMat;
};
layout (std140, binding = 1) uniform ModelMatrix
{
    mat4 modelCameraMat;
};

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    gl_Position = cameraClipMat * modelCameraMat * position;
}
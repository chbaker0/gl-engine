#version 400

layout (location = 0) in vec4 position;
layout (location = 2) in vec2 texCoord;

layout (std140) uniform GlobalBlock
{
    mat4 cameraClipMat;
    float time;
};
layout (std140) uniform ModelBlock
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
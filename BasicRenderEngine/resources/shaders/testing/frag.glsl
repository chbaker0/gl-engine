#version 400

in vec2 texCoordFrag;
out vec4 outColor;

layout (std140) uniform GlobalBlock
{
    mat4 cameraClipMat;
    float time;
};

uniform sampler2D diffuse;

void main()
{
    outColor = (sin(time)*0.5 + 0.5) * texture(diffuse, texCoordFrag);
}
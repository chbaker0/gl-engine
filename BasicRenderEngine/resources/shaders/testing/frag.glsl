#version 430

in vec2 texCoordFrag;
out vec4 outColor;

layout (binding = 0) uniform sampler2D diffuse;

void main()
{
    outColor = texture(diffuse, texCoordFrag);
}
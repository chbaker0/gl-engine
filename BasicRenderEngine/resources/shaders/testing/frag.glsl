#version 430

in vec3 fragColor;
out vec4 outColor;

layout (binding = 0) uniform sampler2D diffuse;

void main()
{
//    outColor = vec4(fragColor, 1.0);
    outColor = texture(diffuse, vec2(0.0, 0.0));
}
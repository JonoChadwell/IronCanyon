#version 330 core
layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertNor;
layout (location = 2) in vec2 vertTex;

uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
out vec2 vTexCoord;

void main()
{
    vec4 pos = P * V * M *  vec4(vertPos, 1.0);
    gl_Position = pos;
    vTexCoord = vertTex;
}

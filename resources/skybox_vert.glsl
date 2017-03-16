#version 330 core
layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertNor;

uniform mat4 P;
uniform mat4 M;
uniform mat4 V;

void main()
{
    vec4 pos = P * M *  vec4(vertPos, 1.0);
    gl_Position = pos;
}

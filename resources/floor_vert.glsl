#version  330 core
layout(location = 0) in vec4 vertPos;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
out vec3 fragWorld;

void main()
{
   fragWorld = vec3(M * vertPos);
   gl_Position = P * V * M * vertPos;
}

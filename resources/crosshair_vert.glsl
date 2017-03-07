#version  330 core
layout(location = 0) in vec4 vertPos;
uniform mat4 P;

void main()
{
   gl_Position = P * vertPos;
}

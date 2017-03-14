#version  330 core
layout(location = 0) in vec4 vertPos;
uniform mat4 P;
uniform int height;

void main()
{
   gl_Position = P * vertPos;
   gl_Position.y += .336;
}

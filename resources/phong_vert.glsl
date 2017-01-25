#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
uniform vec3 eye;
out vec3 fragNor;
out vec3 fragWorld;
out vec3 viewDir;

void main()
{
   fragNor = normalize((M * vec4(vertNor, 0.0)).xyz);
   fragWorld = vec3(M * vertPos);
   viewDir = normalize(eye - fragWorld);
   gl_Position = P * V * M * vertPos;
}

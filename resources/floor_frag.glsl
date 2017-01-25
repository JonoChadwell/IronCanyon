#version 330 core
in vec3 fragWorld;
out vec4 color;

void main(){
   color = vec4(sin(fragWorld.x / 5) / 4 + .25, 1.0, sin(fragWorld.z / 5) / 4 + .25, 1.0);
}

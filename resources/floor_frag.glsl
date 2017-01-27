#version 330 core
in vec3 fragNor;
in vec3 fragWorld;
out vec4 color;

void main(){
   vec3 normal = normalize(fragNor);
   color = vec4(normal.x, normal.y, normal.z, 1.0);
}

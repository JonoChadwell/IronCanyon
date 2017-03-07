#version 330 core
uniform sampler2D PlayerTexture;
in vec2 vTexCoord;
out vec4 color;

void main(){
   vec4 texColor = texture(PlayerTexture, vTexCoord);
   color = vec4(texColor.r, texColor.g, texColor.b, 1.0);
}

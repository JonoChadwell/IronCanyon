#version 330 core
uniform sampler2D SkyboxTexture;
uniform float rpos;
uniform float type;
in vec2 vTexCoord;
out vec4 color;


void main()
{    
   color = texture(SkyboxTexture, vTexCoord);
   color += vec4(0, 0, 0, type * rpos/1000);
}

#version 330 core
uniform sampler2D alphaTexture;
in vec4 partCol;
out vec4 color;

void main()
{
	float alpha = texture(alphaTexture, gl_PointCoord).r;
	color = vec4(partCol.rgb, partCol.a*alpha);
}

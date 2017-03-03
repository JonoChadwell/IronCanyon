#version 330 core
uniform sampler2D PlayerTexture;
uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float shine;
uniform vec3 sunDir;
in vec3 fragNor;
in vec3 fragWorld;
in vec3 viewDir;
in vec2 vTexCoord;
out vec4 color;

void main(){
   vec4 texColor = texture(PlayerTexture, vTexCoord);
   vec3 lightColor = vec3(1, 1, 1);
   vec3 normLight = normalize(-sunDir);
   float ambient = .2;
   vec3 diffShade = MatDif * max(0, dot(normLight, fragNor));
   vec3 ambShade = ambient * MatAmb;
   vec3 reflect = -1 * normLight + 2 * (dot( normLight, fragNor) ) * fragNor;
   vec3 specShade = pow(max(0, dot(normalize(viewDir), normalize(reflect))), shine)
                    * MatSpec * lightColor;
   vec4 tempColor = vec4(ambShade + diffShade + specShade, 1.0);
   color = vec4(texColor.r * tempColor.r, texColor.g * tempColor.g, texColor.b * tempColor.b, 1.0);
}

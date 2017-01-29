#version 330 core
uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float shine;
uniform vec3 lightPos;
in vec3 fragNor;
in vec3 fragWorld;
in vec3 viewDir;
out vec4 color;

void main(){
   vec3 lightColor = vec3(1, 1, 1);
   vec3 normLight = normalize(lightPos - fragWorld);
   float ambient = .2;
   vec3 diffShade = MatDif * max(0, dot(normLight, fragNor));
   vec3 ambShade = ambient * MatAmb;
   vec3 reflect = -1 * normLight + 2 * (dot( normLight, fragNor) ) * fragNor;
   vec3 specShade = pow(max(0, dot(normalize(viewDir), normalize(reflect))), shine)
                    * MatSpec * lightColor;
   vec3 positionShade = vec3(
         max(0,sin(fragWorld.x)), 
		 max(0,sin(fragWorld.y)), 
		 max(0,sin(fragWorld.z))) * 0.1;
   color = vec4(ambShade + diffShade + specShade + positionShade, 1.0);
}

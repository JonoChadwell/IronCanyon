#version 330 core
uniform vec3 uniColor;
out vec4 color;

void main(){
   float distanceFromCenter = distance( vec2( .5, .5 ),
                         vec2( gl_PointCoord.x, gl_PointCoord.y ) );
   if( distanceFromCenter > .2 ){
      discard;
   }
   if ((gl_PointCoord.x < .45 || gl_PointCoord.x > .55) && (gl_PointCoord.y > .55 || gl_PointCoord.y < .45)) {
      discard;
   }
   float distanceFromCenterWindow = distance( vec2( .5, .5 ), vec2( gl_FragCoord.x, gl_FragCoord.y ) );
   if( distanceFromCenterWindow < 1 ){
      discard;
   }
   color = vec4(uniColor.r, uniColor.g, uniColor.b, 1.0);
}

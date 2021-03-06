#version 330 core
uniform vec3 uniColor;
uniform int hair;
out vec4 color;

void main(){
   float distanceFromCenter = distance( vec2( .5, .5 ),
                         vec2( gl_PointCoord.x, gl_PointCoord.y ) );
   if( distanceFromCenter > .3 ){
      discard;
   }
   if (hair == 3) {
      if ((gl_PointCoord.x < .36 || gl_PointCoord.x > .66) && (gl_PointCoord.y > .66 || gl_PointCoord.y < .36))
         discard;
      if ((gl_PointCoord.x > .42 && gl_PointCoord.x < .58) || (gl_PointCoord.y <= .58 && gl_PointCoord.y > .42))
         discard;
   }
   if (hair == 1 || hair == 2) {
      if ((gl_PointCoord.x < .45 || gl_PointCoord.x > .55) && (gl_PointCoord.y > .55 || gl_PointCoord.y < .45)) {
         discard;
      }
      if (hair == 2) {
         if (gl_PointCoord.x > .45 && gl_PointCoord.x < .55 && gl_PointCoord.y < .55 && gl_PointCoord.y > .45) {
            discard;
         }
      }
   }
   else if (hair == 4) {
      if( distanceFromCenter > .08 ){
         discard;
      }
   }
   float distanceFromCenterWindow = distance( vec2( .5, .5 ), vec2( gl_FragCoord.x, gl_FragCoord.y ) );
   if( distanceFromCenterWindow < 1 ){
      discard;
   }
   color = vec4(uniColor.r, uniColor.g, uniColor.b, 1.0);
}

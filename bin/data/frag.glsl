
// Adapted from https://github.com/antimatter15/splat
#version 150
//precision mediump float;

in vec4 vColor;
in vec2 vPosition;
//layout(location = 0)
out vec4 outColor;

void main () {    
  float A = 1.-length(vPosition.xy);
  if (A < -4.0) discard;
  float B = exp(A) * vColor.a;
    outColor = vec4( B*vColor.rgb, B);
}


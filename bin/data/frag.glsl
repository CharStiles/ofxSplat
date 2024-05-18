
// Adapted from https://github.com/antimatter15/splat
#version 150

in vec4 vColor;
in vec2 vPosition;
out vec4 outColor;

void main () {
    float A = -dot(vPosition, vPosition);
    if (A < -4.0) discard;
    float B = exp(A) * vColor.a;
    outColor = vec4(B * vColor.rgb, B);
}

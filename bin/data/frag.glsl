#version 150
in vec4 color;
in vec2 texCoordVarying;
out vec4 outColor;
uniform sampler2D tex;

void main(){
	outColor = color;
}
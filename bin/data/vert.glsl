#version 150

uniform mat4 modelViewProjectionMatrix;
in vec4 position;
in vec4 instanceColor;
in vec2 texcoord;


uniform sampler2D tex;
out vec4 color;
out vec2 texCoordVarying;

void main(){
    texCoordVarying = texcoord;
    color = instanceColor;
    gl_Position = modelViewProjectionMatrix * position;

}

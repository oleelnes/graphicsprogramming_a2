#version 430 core

in vec2 vsTexcoord;

out vec4 outColor;

uniform sampler2D uTexture;

void main(){
	outColor = vec4(vsTexcoord, uTexture);
}
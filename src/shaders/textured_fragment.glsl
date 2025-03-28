#version 460 core

in vec2 texCoords;
out vec4 outColor;
uniform sampler2D tex;

void main() {
	outColor = texture(tex, texCoords);
}
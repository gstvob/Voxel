#version 460 core
layout(location=0) in vec3 inPos;
layout(location=1) in float inDirection;
layout(location=2) in float inColorIndex;

out vec3 fragColor;
out vec3 fragPosition;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

vec3 colors[4] = {
	vec3(0.52, 0.54, 0.52),
	vec3(0.5, 0.41, 0.21),
	vec3(0.3, 0.63, 0.24),
	vec3(0.8, 0.8, 0.8)
};

vec3 directions[6] = {
	vec3(0.0, 0.0, -1.0),
	vec3(0.0, 1.0, 0.0),
	vec3(-1.0, 0.0, 0.0),
	vec3(1.0, 0.0, 0.0),
	vec3(0.0, 0.0, 1.0),
	vec3(0.0, -1.0, 0.0)
};


void main() {
	fragColor = colors[int(inColorIndex)];
	normal = directions[int(inDirection)];
	gl_Position = projection * view * model * vec4(inPos.xyz, 1.0);
	fragPosition = vec3(gl_Position);
}
#version 460 core

in vec3 fragColor;
in vec3 fragPosition;
in vec3 normal; 

out vec4 outColor;

uniform vec3 sunDirection;
uniform vec3 lightColor;


void main() {
	float ambientStrength = 0.1;
	
	vec3 normalizedNormal = normalize(normal);
	vec3 lightDirection = normalize(sunDirection + vec3(0.0, 10.0, 0.0));
	float diffuseFactor = max(dot(normalizedNormal, lightDirection), 0.0);

	vec3 ambientColor = ambientStrength * lightColor;
	vec3 diffuseColor = lightColor * diffuseFactor;
	outColor = vec4((ambientColor + diffuseColor) * fragColor, 1.0);
}
#version 300 es
precision mediump float;

uniform samplerCube backgroundCubeMap;

in vec3 fragPos;
out vec4 FragColor;

void main(){
	vec3 viewDirection = normalize(fragPos);
	FragColor = texture(backgroundCubeMap, viewDirection);
}
#version 300 es
precision mediump float;

uniform mat4 viewMat, projMat;

layout (location = 0) in vec3 position;

out vec3 fragPos;

void main(){
	mat4 viewRotation = mat4(mat3(viewMat));
	fragPos = position;
	gl_Position = projMat * viewRotation * vec4(position, 1.0);
}
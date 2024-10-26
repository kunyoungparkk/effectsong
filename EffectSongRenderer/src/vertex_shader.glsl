#version 300 es
precision mediump float;

uniform mat4 worldMat, viewMat, projMat;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;

out vec3 fragNormal;
out vec2 fragTexcoord;

void main(){
	fragNormal = normal;
	fragTexcoord = texcoord;
	gl_Position = projMat * viewMat * worldMat * vec4(position, 1.0);
}
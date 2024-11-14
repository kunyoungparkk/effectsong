#version 300 es
precision mediump float;

uniform mat4 worldMat, viewMat, projMat;
uniform mat3 normalMat;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;
layout (location = 3) in vec4 tangent;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragTexcoord;
out mat3 TBN;

void main(){
	fragPos = vec3(worldMat * vec4(position, 1.0));
	fragNormal = normalize(normalMat * normal);
	fragTexcoord = texcoord;

	vec3 T = normalize(mat3(worldMat) * tangent.xyz);
	vec3 N = fragNormal;
	vec3 B = cross(N, T) * tangent.w; // ∫Ò∆Æ≈ƒ¡®∆Æ πÊ«‚ ¡∂¡§
	TBN = mat3(T, B, N);
	
	gl_Position = projMat * viewMat * worldMat * vec4(position, 1.0);
}
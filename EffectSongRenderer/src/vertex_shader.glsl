#version 300 es
precision highp float;

uniform mat4 worldMat, viewMat, projMat;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;
layout (location = 3) in vec4 tangent;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragTexcoord;
out mat3 tangentWorldMat;

void main(){
	mat3 normalMat = transpose(inverse(mat3(worldMat)));

	fragPos = vec3(worldMat * vec4(position, 1.0));
	fragNormal = normalize(normalMat * normal);
	fragTexcoord = texcoord;

	vec3 T = normalize(normalMat * tangent);
	vec3 N = fragNormal;
	vec3 B = cross(N, T);
	mat3 TBN = mat3(T, B, N);
	
	tangentWorldMat = normalMat * inverse(TBN);

	gl_Position = projMat * viewMat * worldMat * vec4(position, 1.0);
}
#pragma once
#include "common.h"
class Node;

class Light {
public:
	enum class LightType
	{
		NONE,
		DIRECTIONAL_LIGHT,
		POINT_LIGHT,
		SPOT_LIGHT
	};
	Light(Node* node, cgltf_light* cgltf_light);

	glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);
	float intensity = 0.0f;
	float range = 0.0f;
	float innerConeAngle = 0.0f;
	float outerConeAngle = 0.0f;
	LightType lightType = LightType::NONE;
	std::string name = "";

	Node* getNode();
private:
	Node* m_node;
	cgltf_light* m_cgltf_light;
};
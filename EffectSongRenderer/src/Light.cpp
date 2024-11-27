#include "Light.h"
#include "Node.h"

Light::Light(Node* node, cgltf_light* cgltf_light)
{
	m_node = node;
	m_cgltf_light = cgltf_light;
	
	if (!m_cgltf_light) {
		return;
	}
	switch (cgltf_light->type)
	{
	case cgltf_light_type::cgltf_light_type_directional:
		lightType = LightType::DIRECTIONAL_LIGHT;
		break;
	case cgltf_light_type::cgltf_light_type_point:
		lightType = LightType::POINT_LIGHT;
		break;
	case cgltf_light_type::cgltf_light_type_spot:
		lightType = LightType::SPOT_LIGHT;
		break;
	default:
		//TODO: ERROR!
		exit(0);
		break;
	}

	color[0] = cgltf_light->color[0];
	color[1] = cgltf_light->color[1];
	color[2] = cgltf_light->color[2];

	//TODO: how to recognize light intensity exactly? intensity value of gltf is too big
	intensity = cgltf_light->intensity * 0.01f;
	//TODO: gltf doesn't contain range of blender works
	range = cgltf_light->range;
	if (range < 0.1f) {
		range = 5.0;// cgltf_light->range;
	}
	innerConeAngle = cgltf_light->spot_inner_cone_angle;
	outerConeAngle = cgltf_light->spot_outer_cone_angle;
	if (cgltf_light->name)
	{
		name = cgltf_light->name;
	}
}

Light::Light(Node* node) : m_node(node)
{
}

Node* Light::getNode()
{
	return m_node;
}

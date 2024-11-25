#include "Light.h"
#include "Node.h"

Light::Light(Node* node, cgltf_light* cgltf_light)
{
	m_node = node;
	m_cgltf_light = cgltf_light;

	m_node->setLight(this);
	
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

	intensity = cgltf_light->intensity;
	//TODO: blender range �ȵ����� ����
	if (cgltf_light->range < 0.1f) {
		range = 5.0;// cgltf_light->range;
	}
	else {
		range = cgltf_light->range;
	}
	innerConeAngle = cgltf_light->spot_inner_cone_angle;
	outerConeAngle = cgltf_light->spot_outer_cone_angle;
	if (cgltf_light->name)
	{
		name = cgltf_light->name;
	}
}

Node* Light::getNode()
{
	return m_node;
}

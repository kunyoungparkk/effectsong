#include "Scene.h"
#include "Renderer.h"
#include <string>

Scene::Scene(cgltf_scene& cgltfScene):Node(this, nullptr){
	if (cgltfScene.name) {
		m_name = cgltfScene.name;
	}
	for (int i = 0; i < cgltfScene.nodes_count; i++) {
		Node* node = new Node(cgltfScene.nodes[i], this, this);
		m_children.push_back(node);
	}
}

Scene::Scene() :Node(this, nullptr) {}

Scene::~Scene()
{
}

//void Scene::update() {
//	for (auto iter = m_children.begin(); iter != m_children.end(); iter++) {
//		(*iter)->update();
//	}
//}

void Scene::render(GLuint shaderProgram) {
	//light
	int dirLightCnt = 0;
	int pointLightCnt = 0;
	int spotLightCnt = 0;
	std::string uniformName;

	if (shaderProgram != m_shaderProgram) {
		m_shaderProgram = shaderProgram;

		m_numDirectionalLightsLoc = glGetUniformLocation(m_shaderProgram, "numDirectionalLights");
		m_numPointLightsLoc = glGetUniformLocation(m_shaderProgram, "numPointLights");
		m_numSpotLightsLoc = glGetUniformLocation(m_shaderProgram, "numSpotLights");
		for (int i = 0; i < 10; i++) {
			uniformName = "directionalLights[" + std::to_string(i) + "].position";
			m_directionalLightsLoc[i].position = glGetUniformLocation(m_shaderProgram, uniformName.c_str());
			uniformName = "directionalLights[" + std::to_string(i) + "].intensity";
			m_directionalLightsLoc[i].intensity = glGetUniformLocation(m_shaderProgram, uniformName.c_str());
			uniformName = "directionalLights[" + std::to_string(i) + "].color";
			m_directionalLightsLoc[i].color = glGetUniformLocation(m_shaderProgram, uniformName.c_str());
			uniformName = "directionalLights[" + std::to_string(i) + "].direction";
			m_directionalLightsLoc[i].direction = glGetUniformLocation(m_shaderProgram, uniformName.c_str());

			uniformName = "pointLights[" + std::to_string(i) + "].position";
			m_pointLightsLoc[i].position = glGetUniformLocation(m_shaderProgram, uniformName.c_str());
			uniformName = "pointLights[" + std::to_string(i) + "].intensity";
			m_pointLightsLoc[i].intensity = glGetUniformLocation(m_shaderProgram, uniformName.c_str());
			uniformName = "pointLights[" + std::to_string(i) + "].color";
			m_pointLightsLoc[i].color = glGetUniformLocation(m_shaderProgram, uniformName.c_str());
			uniformName = "pointLights[" + std::to_string(i) + "].range";
			m_pointLightsLoc[i].range = glGetUniformLocation(m_shaderProgram, uniformName.c_str());

			uniformName = "spotLights[" + std::to_string(i) + "].position";
			m_spotLightsLoc[i].position = glGetUniformLocation(m_shaderProgram, uniformName.c_str());
			uniformName = "spotLights[" + std::to_string(i) + "].intensity";
			m_spotLightsLoc[i].intensity = glGetUniformLocation(m_shaderProgram, uniformName.c_str());
			uniformName = "spotLights[" + std::to_string(i) + "].color";
			m_spotLightsLoc[i].color = glGetUniformLocation(m_shaderProgram, uniformName.c_str());
			uniformName = "spotLights[" + std::to_string(i) + "].range";
			m_spotLightsLoc[i].range = glGetUniformLocation(m_shaderProgram, uniformName.c_str());
			uniformName = "spotLights[" + std::to_string(i) + "].direction";
			m_spotLightsLoc[i].direction = glGetUniformLocation(m_shaderProgram, uniformName.c_str());
			uniformName = "spotLights[" + std::to_string(i) + "].innerConeAngle";
			m_spotLightsLoc[i].innerConeAngle = glGetUniformLocation(m_shaderProgram, uniformName.c_str());
			uniformName = "spotLights[" + std::to_string(i) + "].outerConeAngle";
			m_spotLightsLoc[i].outerConeAngle = glGetUniformLocation(m_shaderProgram, uniformName.c_str());
		}
	}

	for (auto iter = m_lights.begin(); iter != m_lights.end(); iter++) {
		Light* light = (*iter);
		switch (light->lightType) {
		case Light::LightType::DIRECTIONAL_LIGHT:
		{
			if (dirLightCnt >= 10) {
				break;
			}
			glUniform3fv(m_directionalLightsLoc[dirLightCnt].position, 1, glm::value_ptr(light->getNode()->getGlobalPosition()));
			glUniform1f(m_directionalLightsLoc[dirLightCnt].intensity, light->intensity);
			glUniform3fv(m_directionalLightsLoc[dirLightCnt].color, 1, glm::value_ptr(light->color));
			glUniform3fv(m_directionalLightsLoc[dirLightCnt].direction, 1, glm::value_ptr(glm::normalize(-1.0f * light->getNode()->getGlobalFront())));

			dirLightCnt++;
			break;
		}
		case Light::LightType::POINT_LIGHT:
		{
			if (pointLightCnt >= 10) {
				break;
			}
			glUniform3fv(m_pointLightsLoc[pointLightCnt].position, 1, glm::value_ptr(light->getNode()->getGlobalPosition()));
			glUniform1f(m_pointLightsLoc[pointLightCnt].intensity, light->intensity);
			glUniform3fv(m_pointLightsLoc[pointLightCnt].color, 1, glm::value_ptr(light->color));
			glUniform1f(m_pointLightsLoc[pointLightCnt].range, light->range);

			pointLightCnt++;
			break;
		}
		case Light::LightType::SPOT_LIGHT:
		{
			if (spotLightCnt >= 10) {
				break;
			}
			glUniform3fv(m_spotLightsLoc[spotLightCnt].position, 1, glm::value_ptr(light->getNode()->getGlobalPosition()));
			glUniform1f(m_spotLightsLoc[spotLightCnt].intensity, light->intensity);
			glUniform3fv(m_spotLightsLoc[spotLightCnt].color, 1, glm::value_ptr(light->color));
			glUniform1f(m_spotLightsLoc[spotLightCnt].range, light->range);
			glUniform3fv(m_spotLightsLoc[spotLightCnt].direction, 1, glm::value_ptr(glm::normalize(-1.0f * light->getNode()->getGlobalFront())));
			glUniform1f(m_spotLightsLoc[spotLightCnt].innerConeAngle, light->innerConeAngle);
			glUniform1f(m_spotLightsLoc[spotLightCnt].outerConeAngle, light->outerConeAngle);

			spotLightCnt++;
			break;
		}
		}
	}
	glUniform1i(m_numDirectionalLightsLoc, dirLightCnt);
	glUniform1i(m_numPointLightsLoc, pointLightCnt);
	glUniform1i(m_numSpotLightsLoc, spotLightCnt);

	//nodes
	for (auto iter = m_children.begin(); iter != m_children.end(); iter++) {
		(*iter)->render(shaderProgram);
	}
}

void Scene::addLight(Light* light)
{
	m_lights.push_back(light);
}

void Scene::removeLight(Light* light)
{
	m_lights.remove(light);
}

void Scene::addCamera(Camera* camera)
{
	m_cameras.push_back(camera);
}

void Scene::removeCamera(Camera* camera)
{
	m_cameras.remove(camera);
}

Camera* Scene::getCameraAt(int index)
{
	if (index >= m_cameras.size()) {
		return nullptr;
	}
	auto iter = m_cameras.begin();
	std::advance(iter, index);
	return *iter;
}
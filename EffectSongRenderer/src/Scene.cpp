#include "Scene.h"
#include "Renderer.h"
#include <string>

Scene::Scene(cgltf_scene& cgltfScene) : m_cgltf_scene(cgltfScene) {
	if (m_cgltf_scene.name) {
		m_name = m_cgltf_scene.name;
	}
	for (int i = 0; i < m_cgltf_scene.nodes_count; i++) {
		Node* node = new Node(m_cgltf_scene.nodes[i], nullptr, this);
		m_nodes.push_back(node);
	}
}

Scene::~Scene()
{
	for (auto iter = m_nodes.begin(); iter != m_nodes.end(); iter++) {
		delete *iter;
	}
}

void Scene::update() {
	for (auto iter = m_nodes.begin(); iter != m_nodes.end(); iter++) {
		(*iter)->update();
	}
}

void Scene::render(GLuint shaderProgram) {
	//light
	int dirLightCnt = 0;
	int pointLightCnt = 0;
	int spotLightCnt = 0;
	std::string uniformName;
	//TODO: Light의 Position은 global position을 넣어줘야 함.
	for (auto iter = lights.begin(); iter != lights.end(); iter++) {
		Light* light = (*iter);
		switch (light->lightType) {
		case Light::LightType::DIRECTIONAL_LIGHT:
		{
			if (dirLightCnt >= 10) {
				break;
			}
			uniformName = "directionalLights[" + std::to_string(dirLightCnt) + "].position";
			GLint lightPosLoc = glGetUniformLocation(shaderProgram, uniformName.c_str());
			glUniform3fv(lightPosLoc, 1, glm::value_ptr(light->getNode()->getGlobalPosition()));

			uniformName = "directionalLights[" + std::to_string(dirLightCnt) + "].intensity";
			GLint lightIntensityLoc = glGetUniformLocation(shaderProgram, uniformName.c_str());
			glUniform1f(lightIntensityLoc, light->intensity);

			uniformName = "directionalLights[" + std::to_string(dirLightCnt) + "].color";
			GLint lightColorLoc = glGetUniformLocation(shaderProgram, uniformName.c_str());
			glUniform3fv(lightColorLoc, 1, glm::value_ptr(light->color));

			uniformName = "directionalLights[" + std::to_string(dirLightCnt) + "].direction";
			GLint lightDirLoc = glGetUniformLocation(shaderProgram, uniformName.c_str());
			glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::normalize(-1.0f * light->getNode()->getGlobalFront())));

			dirLightCnt++;
			break;
		}
		case Light::LightType::POINT_LIGHT:
		{
			if (pointLightCnt >= 10) {
				break;
			}
			uniformName = "pointLights[" + std::to_string(pointLightCnt) + "].position";
			GLint lightPosLoc = glGetUniformLocation(shaderProgram, uniformName.c_str());
			glUniform3fv(lightPosLoc, 1, glm::value_ptr(light->getNode()->getGlobalPosition()));

			uniformName = "pointLights[" + std::to_string(pointLightCnt) + "].intensity";
			GLint lightIntensityLoc = glGetUniformLocation(shaderProgram, uniformName.c_str());
			glUniform1f(lightIntensityLoc, light->intensity);

			uniformName = "pointLights[" + std::to_string(pointLightCnt) + "].color";
			GLint lightColorLoc = glGetUniformLocation(shaderProgram, uniformName.c_str());
			glUniform3fv(lightColorLoc, 1, glm::value_ptr(light->color));

			uniformName = "pointLights[" + std::to_string(pointLightCnt) + "].range";
			GLint lightRangeLoc = glGetUniformLocation(shaderProgram, uniformName.c_str());
			glUniform1f(lightRangeLoc, light->range);

			pointLightCnt++;
			break;
		}
		case Light::LightType::SPOT_LIGHT:
		{
			if (spotLightCnt >= 10) {
				break;
			}
			uniformName = "spotLights[" + std::to_string(spotLightCnt) + "].position";
			GLint lightPosLoc = glGetUniformLocation(shaderProgram, uniformName.c_str());
			glUniform3fv(lightPosLoc, 1, glm::value_ptr(light->getNode()->getGlobalPosition()));

			uniformName = "spotLights[" + std::to_string(spotLightCnt) + "].intensity";
			GLint lightIntensityLoc = glGetUniformLocation(shaderProgram, uniformName.c_str());
			glUniform1f(lightIntensityLoc, light->intensity);

			uniformName = "spotLights[" + std::to_string(spotLightCnt) + "].color";
			GLint lightColorLoc = glGetUniformLocation(shaderProgram, uniformName.c_str());
			glUniform3fv(lightColorLoc, 1, glm::value_ptr(light->color));

			uniformName = "spotLights[" + std::to_string(spotLightCnt) + "].range";
			GLint lightRangeLoc = glGetUniformLocation(shaderProgram, uniformName.c_str());
			glUniform1f(lightRangeLoc, light->range);

			uniformName = "spotLights[" + std::to_string(spotLightCnt) + "].direction";
			GLint lightDirLoc = glGetUniformLocation(shaderProgram, uniformName.c_str());
			glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::normalize(-1.0f * light->getNode()->getGlobalFront())));

			uniformName = "spotLights[" + std::to_string(spotLightCnt) + "].innerConeAngle";
			GLint lightInnerConeAngleLoc = glGetUniformLocation(shaderProgram, uniformName.c_str());
			glUniform1f(lightInnerConeAngleLoc, light->innerConeAngle);

			uniformName = "spotLights[" + std::to_string(spotLightCnt) + "].outerConeAngle";
			GLint lightOuterConeAngleLoc = glGetUniformLocation(shaderProgram, uniformName.c_str());
			glUniform1f(lightOuterConeAngleLoc, light->outerConeAngle);

			spotLightCnt++;
			break;
		}
		}
	}
	GLint numDirectionalLightsLoc = glGetUniformLocation(shaderProgram, "numDirectionalLights");
	glUniform1i(numDirectionalLightsLoc, dirLightCnt);
	GLint numPointLightsLoc = glGetUniformLocation(shaderProgram, "numPointLights");
	glUniform1i(numPointLightsLoc, pointLightCnt);
	GLint numSpotLightsLoc = glGetUniformLocation(shaderProgram, "numSpotLights");
	glUniform1i(numSpotLightsLoc, spotLightCnt);

	//nodes
	for (auto iter = m_nodes.begin(); iter != m_nodes.end(); iter++) {
		(*iter)->render(shaderProgram);
	}
}

void Scene::addLight(Light* light)
{
	lights.push_back(light);
}

void Scene::removeLight(Light* light)
{
	lights.remove(light);
}

Node* Scene::getChildAt(int index)
{
	if (index >= m_nodes.size()) {
		return nullptr;
	}
	auto iter = m_nodes.begin();
	std::advance(iter, index);
	return *iter;
}

Node* Scene::getChildByName(std::string name)
{
	for (auto iter = m_nodes.begin(); iter != m_nodes.end(); iter++) {
		if (name == (*iter)->getName()) {
			return *iter;
		}
	}
	return nullptr;
}

void Scene::addNode(Node* node)
{
	m_nodes.push_back(node);
}

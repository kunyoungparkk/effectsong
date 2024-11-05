#include "Scene.h"
#include <string>

Scene::Scene(cgltf_scene& cgltfScene) : m_cgltf_scene(cgltfScene) {
	for (int i = 0; i < m_cgltf_scene.nodes_count; i++) {
		Node* node = new Node(m_cgltf_scene.nodes[i], nullptr, this);
		nodes.push_back(node);
	}
	//TODO: modify required about temp camera
	if (!m_active_camera) {
		Node* emptyNode = new Node(this, nullptr);
		Camera* camera = new Camera(emptyNode);
		setActiveCamera(camera);
		emptyNode->setPosition(glm::vec3(0, 0, -5));
		emptyNode->setRotation(
			glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
		nodes.push_back(emptyNode);
	}
}

void Scene::update() {
	for (int i = 0; i < nodes.size(); i++) {
		nodes[i]->update();
	}
}

void Scene::render(GLuint shaderProgram) {
	//camera
	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;
	if (m_active_camera) {
		projectionMatrix = glm::perspective(
			glm::radians(m_active_camera->fov), 640.0f / 480.0f,
			m_active_camera->zNear, m_active_camera->zFar);
		viewMatrix = glm::inverse(m_active_camera->getNode()->getModelMatrix());

		GLint camPosLoc = glGetUniformLocation(shaderProgram, "cameraWorldPos");
		//TODO: Global position으로 변경 필요
		glUniform3fv(camPosLoc, 1, glm::value_ptr(m_active_camera->getNode()->getPosition()));
	}
	else {
		//TODO: ERROR!
		projectionMatrix = glm::mat4(1.0f);
		viewMatrix = glm::mat4(1.0f);
		exit(0);
	}
	GLint viewMatLoc = glGetUniformLocation(shaderProgram, "viewMat");
	GLint projectionMatLoc = glGetUniformLocation(shaderProgram, "projMat");
	glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(projectionMatLoc, 1, GL_FALSE,
		glm::value_ptr(projectionMatrix));

	//light
	int dirLightCnt = 0;
	int pointLightCnt = 0;
	int spotLightCnt = 0;
	std::string uniformName;
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
			glUniform3fv(lightPosLoc, 1, glm::value_ptr(light->getNode()->getPosition()));

			uniformName = "directionalLights[" + std::to_string(dirLightCnt) + "].intensity";
			GLint lightIntensityLoc = glGetUniformLocation(shaderProgram, uniformName.c_str());
			glUniform1f(lightIntensityLoc, light->intensity);

			uniformName = "directionalLights[" + std::to_string(dirLightCnt) + "].color";
			GLint lightColorLoc = glGetUniformLocation(shaderProgram, uniformName.c_str());
			glUniform3fv(lightColorLoc, 1, glm::value_ptr(light->color));

			uniformName = "directionalLights[" + std::to_string(dirLightCnt) + "].direction";
			GLint lightDirLoc = glGetUniformLocation(shaderProgram, uniformName.c_str());
			glm::vec3 rotatedFront(glm::rotate(light->getNode()->getRotation(), glm::vec3(0.0f, 0.0f, -1.0f)));
			glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::normalize(rotatedFront)));
			
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
			glUniform3fv(lightPosLoc, 1, glm::value_ptr(light->getNode()->getPosition()));

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
			glUniform3fv(lightPosLoc, 1, glm::value_ptr(light->getNode()->getPosition()));

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
			glm::vec3 rotatedFront(glm::rotate(light->getNode()->getRotation(), glm::vec3(0.0f, 0.0f, -1.0f)));
			glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::normalize(rotatedFront)));

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
	for (int i = 0; i < nodes.size(); i++) {
		nodes[i]->render(shaderProgram);
	}
}

void Scene::setActiveCamera(Camera* camera) { m_active_camera = camera; }

Camera* Scene::getActiveCamera() { return m_active_camera; }

void Scene::addLight(Light* light)
{
	lights.push_back(light);
}

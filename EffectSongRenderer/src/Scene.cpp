#include "Scene.h"

Scene::Scene(cgltf_scene& cgltfScene) : m_cgltf_scene(cgltfScene) {
  for (int i = 0; i < m_cgltf_scene.nodes_count; i++) {
    if (m_cgltf_scene.nodes[i]->camera) {
      Camera* camera = new Camera(m_cgltf_scene.nodes[i], nullptr, this);
      setActiveCamera(camera);
      nodes.push_back(camera);
    } else {
      Node* node = new Node(m_cgltf_scene.nodes[i], nullptr, this);
      nodes.push_back(node);
    }
  }
  if (!m_active_camera) {
    Camera* camera = new Camera(nullptr, nullptr, this);
    setActiveCamera(camera);
    camera->setPosition(glm::vec3(0, 0, -5));
    camera->setRotation(
        glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
    nodes.push_back(camera);
  }
}

void Scene::render() {
  glm::mat4 projectionMatrix;
  glm::mat4 viewMatrix;
  if (m_active_camera) {
    projectionMatrix = glm::perspective(
        glm::radians(m_active_camera->getFov()), 640.0f / 480.0f,
        m_active_camera->getNear(), m_active_camera->getFar());
    viewMatrix = m_active_camera->getViewMatrix();
  } else {
    //ERROR!
    projectionMatrix = glm::mat4(1.0f);
    viewMatrix = glm::mat4(1.0f);
  }

  for (int i = 0; i < nodes.size(); i++) {
    nodes[i]->render(projectionMatrix, viewMatrix);
  }
}

void Scene::setActiveCamera(Camera* camera) { m_active_camera = camera; }

Camera* Scene::getActiveCamera() { return m_active_camera; }

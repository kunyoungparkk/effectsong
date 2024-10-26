#pragma once
#include "common.h"
#include "Primitive.h"

class Scene;
class Node {
 public:
  Node(cgltf_node* cgltfNode, Node* parent, Scene* scene);

  void update();
  void render(glm::mat4 projectionMatrix, glm::mat4 viewMatrix);

  glm::mat4 getModelMatrix();

  glm::vec3 getPosition() const;
  glm::quat getRotation() const;
  glm::vec3 getScale() const;

  void setPosition(glm::vec3 position);
  void setRotation(glm::quat rotation);
  void setScale(glm::vec3 scale);

  glm::vec3 getFront();
  glm::vec3 getLeft();

 private:
  Node* m_parent;
  Scene* m_scene;
  std::vector<Node*> m_children;
  std::vector<Primitive*> m_primitives;
  glm::vec3 m_position = glm::vec3(0, 0, 0);
  glm::quat m_rotation = glm::quat(1, 0, 0, 0);
  glm::vec3 m_scale = glm::vec3(1, 1, 1);
  cgltf_node* m_cgltf_node;
};
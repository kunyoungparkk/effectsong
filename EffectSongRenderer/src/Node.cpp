#include "Node.h"

Node::Node(cgltf_node* cgltfNode, Node* parent, Scene* scene)
    : m_parent(parent), m_scene(scene), m_cgltf_node(cgltfNode) {
  if (m_cgltf_node) {
    m_position = *(glm::vec3*)cgltfNode->translation;
    m_rotation = *(glm::quat*)cgltfNode->rotation;
    m_scale = *(glm::vec3*)cgltfNode->scale;

    for (int primIdx = 0; primIdx < cgltfNode->mesh->primitives_count;
         primIdx++) {
      cgltf_primitive* cgltfPrim = &cgltfNode->mesh->primitives[primIdx];

      Primitive* prim = new Primitive(cgltfPrim);
      m_primitives.push_back(prim);
    }

    for (int childIdx = 0; childIdx < cgltfNode->children_count; childIdx++) {
      cgltf_node* cgltfChildNode = cgltfNode->children[childIdx];
      Node* childNode = new Node(cgltfChildNode, this, m_scene);
      m_children.push_back(childNode);
    }
  }
}

void Node::update() { m_rotation.x += 1.0f; }

void Node::render(glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
  for (int primIdx = 0; primIdx < m_primitives.size(); primIdx++) {
    m_primitives[primIdx]->render(getModelMatrix(), projectionMatrix, viewMatrix);
  }
}

glm::mat4 Node::getModelMatrix() {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, m_position);
  model = model * glm::toMat4(m_rotation);
  model = glm::scale(model, m_scale);

  if (m_parent) {
    model = m_parent->getModelMatrix() * model;
  }
  return model;
}

glm::vec3 Node::getPosition() const { return m_position; }

glm::quat Node::getRotation() const { return m_rotation; }

glm::vec3 Node::getScale() const { return m_scale; }

void Node::setPosition(glm::vec3 position) { m_position = position; }

void Node::setRotation(glm::quat rotation) { m_rotation = rotation; }

void Node::setScale(glm::vec3 scale) { m_scale = scale; }

glm::vec3 Node::getFront() {
  return glm::rotate(m_rotation, glm::vec3(0, 0, 1));
}

glm::vec3 Node::getLeft() { return glm::normalize(glm::cross(glm::vec3(0, 1, 0), getFront())); }

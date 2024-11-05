#include "Node.h"
#include "Scene.h"

Node::Node(cgltf_node* cgltfNode, Node* parent, Scene* scene)
    : m_parent(parent), m_scene(scene), m_cgltf_node(cgltfNode) {
  if (m_cgltf_node) {
    m_position = *(glm::vec3*)cgltfNode->translation;
    m_rotation = *(glm::quat*)cgltfNode->rotation;
    m_scale = *(glm::vec3*)cgltfNode->scale;
    //skin

    //mesh
    if (cgltfNode->mesh)
    {
        for (int primIdx = 0; primIdx < cgltfNode->mesh->primitives_count;
            primIdx++) {
            cgltf_primitive* cgltfPrim = &cgltfNode->mesh->primitives[primIdx];

            Primitive* prim = new Primitive(cgltfPrim);
            m_primitives.push_back(prim);
        }
    }
    //camera
    if (m_cgltf_node->camera) {
       m_camera = new Camera(this, m_cgltf_node->camera);
       //TODO: 여기서 임시로 결정하는 것이 맞는지 생각해보기
       m_scene->setActiveCamera(m_camera);
    }
    //light
    if (m_cgltf_node->light) {
        m_light = new Light(this, m_cgltf_node->light);
        //TODO: 여기서 Scene에 등록하는 것이 맞는지 생각해보기
        m_scene->addLight(m_light);
    }

    for (int childIdx = 0; childIdx < cgltfNode->children_count; childIdx++) {
      cgltf_node* cgltfChildNode = cgltfNode->children[childIdx];
      Node* childNode = new Node(cgltfChildNode, this, m_scene);
      m_children.push_back(childNode);
    }
  }
}

Node::Node(Scene* scene, Node* parent)
    : m_parent(parent), m_scene(scene), m_cgltf_node(nullptr)
{

}

void Node::update() { 
}

void Node::render(GLuint shaderProgram) {
  if (m_light) {

  }
  //vertex shader
  GLint worldMatLoc = glGetUniformLocation(shaderProgram, "worldMat");
  glUniformMatrix4fv(worldMatLoc, 1, GL_FALSE, glm::value_ptr(getModelMatrix()));

  for (int primIdx = 0; primIdx < m_primitives.size(); primIdx++) {
    m_primitives[primIdx]->render(shaderProgram);
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
//TODO: global transform Get, Set 필요

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

Light* Node::getLight()
{
    return m_light;
}

void Node::setLight(Light* light)
{
    if (m_light) {
        delete m_light;
    }
    m_light = light;
}

Camera* Node::getCamera()
{
    return m_camera;
}

void Node::setCamera(Camera* camera)
{
    if (m_camera) {
        delete m_camera;
    }
    m_camera = camera;
}

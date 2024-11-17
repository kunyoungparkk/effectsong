#pragma once
#include "common.h"
#include "Node.h"
#include "Camera.h"
#include "Light.h"

class Scene {
 public:
  Scene(cgltf_scene& cgltfScene);
  ~Scene();
  void update();
  void render(GLuint shaderProgram);

  void addLight(Light* light);
  void removeLight(Light* light);

  std::string getName() { return m_name; }
  void setName(std::string name) { m_name = name; }

  Node* getNodeAt(int index);
  Node* getNodeByName(std::string name);
  void addNode(Node* node);
 private:
  std::list<Node*> m_nodes;
  std::list<Light*> lights;
  std::string m_name = "";
  cgltf_scene& m_cgltf_scene;
};
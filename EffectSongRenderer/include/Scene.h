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

  void addCamera(Camera* camera);
  void removeCamera(Camera* camera);
  Camera* getCameraAt(int index);
  int getCameraCount() { return m_cameras.size(); }

  std::string getName() { return m_name; }
  void setName(std::string name) { m_name = name; }

  Node* getChildAt(int index);
  Node* getChildByName(std::string name);
  void addNode(Node* node);

  int getChildrenCount() { return m_nodes.size(); }

private:
  std::list<Node*> m_nodes;
  std::list<Light*> m_lights;
  std::list<Camera*> m_cameras;
  std::string m_name = "";
  cgltf_scene& m_cgltf_scene;
};
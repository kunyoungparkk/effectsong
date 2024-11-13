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
  void setActiveCamera(Camera* camera);
  Camera* getActiveCamera();

  void addLight(Light* light);
  void removeLight(Light* light);

 private:
  Camera* m_active_camera = nullptr;
  std::vector<Node*> nodes;
  std::list<Light*> lights;
  cgltf_scene& m_cgltf_scene;
};
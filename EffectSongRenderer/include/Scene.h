#pragma once
#include "common.h"
#include "Node.h"
#include "Camera.h"

class Scene {
 public:
  Scene(cgltf_scene& cgltfScene);
  void render();
  void setActiveCamera(Camera* camera);
  Camera* getActiveCamera();

 private:
  Camera* m_active_camera = nullptr;
  std::vector<Node*> nodes;
  cgltf_scene& m_cgltf_scene;
};
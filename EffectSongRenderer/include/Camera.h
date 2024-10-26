#pragma once
#include "Node.h"

class Camera : public Node {
 public:
  Camera(cgltf_node* cgltfNode, Node* parent, Scene* scene);     
  glm::mat4 getViewMatrix();

  float getAspectRatio();
  float getFov();
  float getWidth();
  float getHeight();
  float getFar();
  float getNear();

  void setAspectRatio(float aspectRatio);
  void setFov(float fov);
  void setWidth(float width);
  void setHeight(float height);
  void setFar(float far);
  void setNear(float near);

 private:
  bool m_isPerspective = true;
  
  //perspective property
  float m_aspectRatio = 0.0f;
  float m_yFov = 45.0f;

  //orthographic property
  float m_xMag = 0.0f;
  float m_yMag = 0.0f;

  //common
  float m_zFar = 1000.0f;
  float m_zNear = 0.1f;
 
  cgltf_camera* m_cgltf_camera = nullptr;
};
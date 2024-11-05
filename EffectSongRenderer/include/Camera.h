#pragma once
#include "common.h"
class Node;

enum class ProjectionType {
	PERSPECTIVE,
	ORTHOGRAPHIC
};
class Camera {
 public:
  Camera(Node* node, cgltf_camera* cgltf_camera);
  Camera(Node* node);

  ProjectionType projectionType = ProjectionType::PERSPECTIVE;
  
  //perspective property
  float aspectRatio = 0.0f;
  float fov = 45.0f;

  //orthographic property
  float xMag = 0.0f;
  float yMag = 0.0f;

  //common
  float zFar = 1000.0f;
  float zNear = 0.1f;
 
  std::string name;

  Node* getNode();
private:
  cgltf_camera* m_cgltf_camera = nullptr;
  Node* m_node;
};
#include "Camera.h"

Camera::Camera(cgltf_node* cgltfNode, Node* parent, Scene* scene)
    : Node(cgltfNode, parent, scene) {
  if (cgltfNode) {
    m_cgltf_camera = cgltfNode->camera;
    if (!cgltfNode->camera) {
      // ERROR!
      return;
    }
    if (cgltfNode->camera->type != cgltf_camera_type_perspective &&
        cgltfNode->camera->type != cgltf_camera_type_orthographic) {
      // ERROR!
      return;
    }

    m_isPerspective = cgltfNode->camera->type == cgltf_camera_type_perspective;

    if (cgltfNode->camera->type == cgltf_camera_type_perspective) {
      m_zFar = m_cgltf_camera->data.perspective.zfar;
      m_zNear = m_cgltf_camera->data.perspective.znear;
      if (m_cgltf_camera->data.perspective.has_aspect_ratio) {
        m_aspectRatio = m_cgltf_camera->data.perspective.aspect_ratio;
      }
      m_yFov = m_cgltf_camera->data.perspective.yfov;
    } else if (cgltfNode->camera->type == cgltf_camera_type_perspective) {
      m_zFar = m_cgltf_camera->data.orthographic.zfar;
      m_zNear = m_cgltf_camera->data.orthographic.znear;
      m_xMag = m_cgltf_camera->data.orthographic.xmag;
      m_yMag = m_cgltf_camera->data.orthographic.ymag;
    }
  } else {
    
  }
}

glm::mat4 Camera::getViewMatrix() {
  return glm::inverse(getModelMatrix()); }

float Camera::getAspectRatio() { return m_aspectRatio; }

float Camera::getFov() { return m_yFov; }

float Camera::getWidth() { return m_xMag; }

float Camera::getHeight() { return m_yMag; }

float Camera::getFar() { return m_zFar; }

float Camera::getNear() { return m_zNear; }

void Camera::setAspectRatio(float aspectRatio) { m_aspectRatio = aspectRatio; }

void Camera::setFov(float fov) { m_yFov = fov; }

void Camera::setWidth(float width) { m_xMag = width; }

void Camera::setHeight(float height) { m_yMag = height; }

void Camera::setFar(float far) { m_zFar = far; }

void Camera::setNear(float near) { m_zNear = near; }

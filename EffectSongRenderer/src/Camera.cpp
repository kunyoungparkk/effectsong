#include "Camera.h"
#include "Node.h"

Camera::Camera(Node* node, cgltf_camera* cgltf_camera)
{
	m_cgltf_camera = cgltf_camera;
	m_node = node;

	m_node->setCamera(this);

	if (!m_cgltf_camera) {
		// TODO: ERROR!
		exit(0);
	}
	if (m_cgltf_camera->type != cgltf_camera_type_perspective &&
		m_cgltf_camera->type != cgltf_camera_type_orthographic) {
		// TODO: ERROR!
		exit(0);
	}

	if (m_cgltf_camera->name) {
		name = m_cgltf_camera->name;
	}

	switch (m_cgltf_camera->type)
	{
	case cgltf_camera_type_perspective:
		projectionType = ProjectionType::PERSPECTIVE;
		zFar = m_cgltf_camera->data.perspective.zfar;
		zNear = m_cgltf_camera->data.perspective.znear;
		if (m_cgltf_camera->data.perspective.has_aspect_ratio) {
			aspectRatio = m_cgltf_camera->data.perspective.aspect_ratio;
		}
		fov = m_cgltf_camera->data.perspective.yfov;
		break;
	case cgltf_camera_type_orthographic:
		projectionType = ProjectionType::ORTHOGRAPHIC;
		zFar = m_cgltf_camera->data.orthographic.zfar;
		zNear = m_cgltf_camera->data.orthographic.znear;
		xMag = m_cgltf_camera->data.orthographic.xmag;
		yMag = m_cgltf_camera->data.orthographic.ymag;
		break;
	default:
		//TODO: ERROR
		exit(0);
		break;
	}
}

Camera::Camera(Node* node)
{
	m_cgltf_camera = nullptr;
	m_node = node;
	
	m_node->setCamera(this);
}

Node* Camera::getNode()
{
	return m_node;
}

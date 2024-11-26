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

glm::mat4 Camera::getViewMatrix()
{
	if (!m_node) {
		return glm::mat4(0.0);
	}
	glm::mat4 modelMatrix = m_node->getModelMatrix();

	glm::vec3 translation = glm::vec3(modelMatrix[3]);
	glm::vec3 scale = glm::vec3(
		glm::length(glm::vec3(modelMatrix[0])),
		glm::length(glm::vec3(modelMatrix[1])),
		glm::length(glm::vec3(modelMatrix[2]))
	);
	glm::mat3 rotationMatrix = glm::mat3(
		glm::vec3(modelMatrix[0]) / scale.x,
		glm::vec3(modelMatrix[1]) / scale.y,
		glm::vec3(modelMatrix[2]) / scale.z
	);

	glm::mat4 modelMatrixNoScale = glm::mat4(1.0f);
	modelMatrixNoScale = glm::translate(modelMatrixNoScale, translation);
	modelMatrixNoScale = modelMatrixNoScale * glm::mat4(rotationMatrix);

	return glm::inverse(modelMatrixNoScale);
}

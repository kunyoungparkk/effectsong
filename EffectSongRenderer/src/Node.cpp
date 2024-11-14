#include "Node.h"
#include "Scene.h"

bool decomposeMatrix(const glm::mat4& matrix, glm::vec3& position, glm::quat& rotation, glm::vec3& scale) {
	// 위치
	position = glm::vec3(matrix[3]);

	// 스케일
	glm::vec3 col0 = glm::vec3(matrix[0]);
	glm::vec3 col1 = glm::vec3(matrix[1]);
	glm::vec3 col2 = glm::vec3(matrix[2]);
	scale.x = glm::length(col0);
	scale.y = glm::length(col1);
	scale.z = glm::length(col2);

	// 스케일을 제거한 회전 행렬
	glm::mat3 rotationMatrix = glm::mat3(matrix);
	if (scale.x != 0) rotationMatrix[0] /= scale.x;
	if (scale.y != 0) rotationMatrix[1] /= scale.y;
	if (scale.z != 0) rotationMatrix[2] /= scale.z;

	// 회전 행렬을 쿼터니언으로 변환
	rotation = glm::quat_cast(rotationMatrix);

	return true;
}

Node::Node(cgltf_node* cgltfNode, Node* parent, Scene* scene)
	: m_parent(parent), m_scene(scene), m_cgltf_node(cgltfNode) {
	if (m_cgltf_node) {
		if (cgltfNode->name) {
			m_name = cgltfNode->name;
		}
		else if (cgltfNode->mesh && cgltfNode->mesh->name) {
			m_name = cgltfNode->mesh->name;
		}

		if (cgltfNode->has_translation) {
			m_position = *(glm::vec3*)cgltfNode->translation;
		}
		if (cgltfNode->has_rotation) {
			m_rotation = *(glm::quat*)cgltfNode->rotation;
		}
		if (cgltfNode->has_scale) {
			m_scale = *(glm::vec3*)cgltfNode->scale;
		}
		if (!m_cgltf_node->has_translation && !m_cgltf_node->has_rotation && !m_cgltf_node->has_scale && m_cgltf_node->matrix) {
			decomposeMatrix(glm::make_mat4(m_cgltf_node->matrix), m_position, m_rotation, m_scale); // `matrix` 속성 사용
		}
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

Node::~Node()
{
	for (int i = 0; i < m_children.size(); i++) {
		delete m_children[i];
	}
	for (int i = 0; i < m_primitives.size(); i++)
	{
		delete m_primitives[i];
	}
	if (m_camera) {
		delete m_camera;
	}
	if (m_light) {
		if (m_scene) {
			m_scene->removeLight(m_light);
		}
		delete m_light;
	}
}

void Node::update() {
}

void Node::render(GLuint shaderProgram) {
	for (int i = 0; i < m_children.size(); i++) {
		m_children[i]->render(shaderProgram);
	}

	//vertex shader
	glm::mat4 modelMatrix = getModelMatrix();
	GLint worldMatLoc = glGetUniformLocation(shaderProgram, "worldMat");
	glUniformMatrix4fv(worldMatLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	GLint normalMatLoc = glGetUniformLocation(shaderProgram, "normalMat");
	glUniformMatrix3fv(normalMatLoc, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(modelMatrix)))));

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

glm::vec3 Node::getGlobalPosition() const
{
	glm::vec3 position = m_position;
	if (m_parent) {
		position = m_parent->getModelMatrix() * glm::vec4(m_position, 1.0);
	}
	return position;
}

glm::quat Node::getGlobalRotation() const
{
	glm::quat rotation = m_rotation;
	if (m_parent) {
		rotation = m_parent->getGlobalRotation() * rotation;
	}
	return rotation;
}

glm::vec3 Node::getGlobalScale() const
{
	glm::vec3 scale = m_scale;
	if (m_parent) {
		scale = m_parent->getGlobalScale() * scale;
	}
	return scale;
}

glm::vec3 Node::getFront() {
	return glm::rotate(m_rotation, glm::vec3(0, 0, 1));
}

glm::vec3 Node::getLeft() { return glm::normalize(glm::cross(glm::vec3(0, 1, 0), getFront())); }

glm::vec3 Node::getGlobalFront()
{
	return glm::rotate(getGlobalRotation(), glm::vec3(0, 0, 1));
}

glm::vec3 Node::getGlobalLeft()
{
	return glm::normalize(glm::cross(glm::vec3(0, 1, 0), getGlobalFront()));
}

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

#include "Node.h"
#include "Scene.h"
#include "Renderer.h"

bool decomposeMatrix(const glm::mat4& matrix, glm::vec3& position, glm::quat& rotation, glm::vec3& scale) {
	// ��ġ
	position = glm::vec3(matrix[3]);

	// ������
	glm::vec3 col0 = glm::vec3(matrix[0]);
	glm::vec3 col1 = glm::vec3(matrix[1]);
	glm::vec3 col2 = glm::vec3(matrix[2]);
	scale.x = glm::length(col0);
	scale.y = glm::length(col1);
	scale.z = glm::length(col2);

	// �������� ������ ȸ�� ���
	glm::mat3 rotationMatrix = glm::mat3(matrix);
	if (scale.x != 0) rotationMatrix[0] /= scale.x;
	if (scale.y != 0) rotationMatrix[1] /= scale.y;
	if (scale.z != 0) rotationMatrix[2] /= scale.z;

	// ȸ�� ����� ���ʹϾ����� ��ȯ
	rotation = glm::quat_cast(rotationMatrix);

	return true;
}

Node::Node(cgltf_node* cgltfNode, Node* parent, Scene* scene)
	: m_parent(parent), m_scene(scene){
	if (cgltfNode) {
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
		if (!cgltfNode->has_translation && !cgltfNode->has_rotation && !cgltfNode->has_scale && cgltfNode->matrix) {
			decomposeMatrix(glm::make_mat4(cgltfNode->matrix), m_position, m_rotation, m_scale); // `matrix` �Ӽ� ���
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
		if (cgltfNode->camera) {
			setCamera(new Camera(this, cgltfNode->camera));
		}
		//light
		if (cgltfNode->light) {
			setLight(new Light(this, cgltfNode->light));
		}

		for (int childIdx = 0; childIdx < cgltfNode->children_count; childIdx++) {
			cgltf_node* cgltfChildNode = cgltfNode->children[childIdx];
			Node* childNode = new Node(cgltfChildNode, this, m_scene);
			m_children.push_back(childNode);
		}
	}
}

Node::Node(Scene* scene, Node* parent)
	: m_parent(parent), m_scene(scene)
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
	for (int i = 0; i < m_children.size(); i++) {
		m_children[i]->update();
	}
	//TODO: temporary reactive scale
	if (m_bAudioReactiveScale) {
		float scaleFactor = m_reactiveOriginScale + m_reactiveChangingScale * Renderer::getInstance()->getCurrentEnergy();
		this->setScale(glm::vec3(scaleFactor));
	}
}

void Node::render(GLuint shaderProgram) {
	for (int i = 0; i < m_children.size(); i++) {
		m_children[i]->render(shaderProgram);
	}

	//vertex shader
	if (shaderProgram != m_shaderProgram) {
		m_shaderProgram = shaderProgram;
		m_worldMatLoc = glGetUniformLocation(shaderProgram, "worldMat");
	}

	glm::mat4 modelMatrix = getModelMatrix();
	glUniformMatrix4fv(m_worldMatLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

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
//TODO: global transform Get, Set �ʿ�

glm::vec3 Node::getPosition() const { return m_position; }

glm::quat Node::getRotation() const { return m_rotation; }

glm::vec3 Node::getEulerRotation() const { return glm::eulerAngles(m_rotation) * 180.0f / glm::pi<float>(); }

glm::vec3 Node::getScale() const { return m_scale; }

void Node::setPosition(glm::vec3 position) { m_position = position; }

void Node::setRotation(glm::quat rotation) { m_rotation = rotation; }

void Node::setRotationByEuler(glm::vec3 euler) { m_rotation = glm::quat(euler / 180.0f * glm::pi<float>()); }

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
		m_scene->removeLight(m_light);
		delete m_light;
	}
	m_scene->addLight(light);
	m_light = light;
}

Camera* Node::getCamera()
{
	return m_camera;
}

void Node::setCamera(Camera* camera)
{
	if (m_camera) {
		m_scene->removeCamera(m_camera);
		delete m_camera;
	}
	m_scene->addCamera(camera);
	m_camera = camera;
}

Node* Node::getChildAt(int index)
{
	if (m_children.size() <= index)
		return nullptr;
	return m_children[index];
}

Node* Node::getChildByName(std::string name)
{
	for (int i = 0; i < m_children.size(); i++) {
		if (m_children[i]->getName() == name) {
			return m_children[i];
		}
	}
	return nullptr;
}

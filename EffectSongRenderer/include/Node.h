#pragma once
#include "common.h"
#include "Primitive.h"
#include "Camera.h"
#include "Light.h"

class Scene;
class Node {
public:
	Node(cgltf_node* cgltfNode, Node* parent, Scene* scene);
	Node(Scene* scene, Node* parent);
	~Node();

	void update();
	void render(GLuint shaderProgram);

	glm::mat4 getModelMatrix();

	glm::vec3 getPosition() const;
	glm::quat getRotation() const;
	glm::vec3 getScale() const;

	void setPosition(glm::vec3 position);
	void setRotation(glm::quat rotation);
	void setScale(glm::vec3 scale);

	glm::vec3 getGlobalPosition() const;
	glm::quat getGlobalRotation() const;
	glm::vec3 getGlobalScale() const;

	glm::vec3 getFront();
	glm::vec3 getLeft();

	glm::vec3 getGlobalFront();
	glm::vec3 getGlobalLeft();

	//Component
	Light* getLight();
	void setLight(Light* light);

	Camera* getCamera();
	void setCamera(Camera* camera);

private:
	Node* m_parent;
	Scene* m_scene;

	Light* m_light = nullptr;
	Camera* m_camera = nullptr;
	//TODO: Mesh, Skin Ãß°¡

	std::vector<Node*> m_children;
	std::vector<Primitive*> m_primitives;
	glm::vec3 m_position = glm::vec3(0, 0, 0);
	glm::quat m_rotation = glm::quat(1, 0, 0, 0);
	glm::vec3 m_scale = glm::vec3(1, 1, 1);
	std::string m_name = "";

	cgltf_node* m_cgltf_node;
};
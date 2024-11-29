#pragma once
#include "common.h"
#include "Primitive.h"

class Camera;
class Light;
class Scene;
class Node {
public:
	Node(cgltf_node* cgltfNode, Node* parent, Scene* scene);
	Node(Scene* scene, Node* parent);
	virtual ~Node();

	virtual void update();
	virtual void render(GLuint shaderProgram);

	/*TRS*/
	glm::vec3 getPosition() const;
	glm::quat getRotation() const;
	glm::vec3 getEulerRotation() const;
	glm::vec3 getScale() const;
	glm::vec3 getFront();
	glm::vec3 getLeft();
	void setPosition(glm::vec3 position);
	void setRotation(glm::quat rotation);
	void setRotationByEuler(glm::vec3 euler);
	void setScale(glm::vec3 scale);
	glm::vec3 getGlobalPosition() const;
	glm::quat getGlobalRotation() const;
	glm::vec3 getGlobalScale() const;
	glm::vec3 getGlobalFront();
	glm::vec3 getGlobalLeft();
	glm::mat4 getModelMatrix();

	//Component
	Light* getLight();
	void setLight(Light* light);

	Camera* getCamera();
	void setCamera(Camera* camera);
	
	std::string getName() { return m_name; }
	void setName(std::string name) { m_name = name; }

	void addChild(Node* node);
	void removeChild(Node* node);
	void setParent(Node* node) { m_parent = node; }
	Node* getParent() { return m_parent; }

	Node* getChildAt(int index);
	Node* getChildByName(std::string name);
	int getChildrenCount() { return m_children.size(); }

	//TODO: temporary code, refactor required
	bool m_bAudioReactiveScale = false;
	float m_reactiveOriginScale = 1.0f;
	float m_reactiveChangingScale = 0.0f;

protected:
	std::list<Node*> m_children;
	std::string m_name = "";
	GLuint m_shaderProgram = -1;
private:
	Node* m_parent;
	Scene* m_scene;

	Light* m_light = nullptr;
	Camera* m_camera = nullptr;
	//TODO: add Mesh(refactor), Skin

	std::vector<Primitive*> m_primitives;

	glm::vec3 m_position = glm::vec3(0, 0, 0);
	glm::quat m_rotation = glm::quat(1, 0, 0, 0);
	glm::vec3 m_scale = glm::vec3(1, 1, 1);

	GLuint m_worldMatLoc = -1;
};
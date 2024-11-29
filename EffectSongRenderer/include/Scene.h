#pragma once
#include "common.h"
#include "Node.h"
#include "Camera.h"
#include "Light.h"

struct DirectionalLightUniform {
	GLuint position = -1;
	GLuint intensity = -1;
	GLuint color = -1;
	GLuint direction = -1;
};
struct PointLightUniform {
	GLuint position = -1;
	GLuint intensity = -1;
	GLuint color = -1;
	GLuint range = -1;
};
struct SpotLightUniform {
	GLuint position = -1;
	GLuint intensity = -1;
	GLuint color = -1;
	GLuint range = -1;
	GLuint direction = -1;
	GLuint innerConeAngle = -1;
	GLuint outerConeAngle = -1;
};
class Scene : public Node {
 public:
  Scene(cgltf_scene& cgltfScene);
  Scene();
  virtual ~Scene();

  //override
  //virtual void update();
  virtual void render(GLuint shaderProgram);

  void addLight(Light* light);
  void removeLight(Light* light);
  void addCamera(Camera* camera);
  void removeCamera(Camera* camera);
  Camera* getCameraAt(int index);
  int getCameraCount() { return m_cameras.size(); }
private:
  std::list<Light*> m_lights;
  std::list<Camera*> m_cameras;
  DirectionalLightUniform m_directionalLightsLoc[10];
  PointLightUniform m_pointLightsLoc[10];
  SpotLightUniform m_spotLightsLoc[10];
  GLuint m_numDirectionalLightsLoc = -1;
  GLuint m_numPointLightsLoc = -1;
  GLuint m_numSpotLightsLoc = -1;
};
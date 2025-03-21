#pragma once
#include <list>
#include <unordered_map>
#include "common.h"
#define IBL_DIFFUSE_LENGTH 64
class Scene;
class Material;
class Texture;
class SpecularIBLTexture;
class DiffuseIBLTexture;
class Primitive;
class IBLPrimitive;
class SoundTexture;
class Camera;

class Renderer {
 public:
  ~Renderer();

  //SingleTon
  static Renderer* getInstance() {
    if (instance == nullptr) {
      instance = new Renderer();
    }
    return instance;
  }
  static void deleteInstance() {
	  delete instance;
      instance = nullptr;
  }

  void update(float currentTime, bool isPlay);
  void render();

  void addScene(Scene* scene);
  void removeScene(Scene* scene);
  Scene* getSceneAt(int index);
  Scene* getSceneByName(std::string name);
  int getSceneCount() { return m_scenes.size(); }

  void setActiveCamera(Camera* camera);
  Camera* getActiveCamera();

  Material* getMaterial(cgltf_material* cgltfMaterial);
  void addMaterial(cgltf_material* cgltfMaterial, Material* material);

  Texture* getTexture(cgltf_texture* cgltf_texture, std::string gltfPath, bool isSRGB = false);

  int getWidth() const;
  int getHeight() const;
  void resize(int width, int height);

  bool setAudioFile(std::string filePath);

  glm::vec4 getBackgroundColor() const { return m_backgroundColor; }
  void setBackgroundColor(glm::vec4 backgroundColor) { m_backgroundColor = backgroundColor; }

  float getDiffuseIBLIntensity() const { return m_diffuseIBLIntensity; }
  void setDiffuseIBLIntensity(float diffuseIBLIntensity) { m_diffuseIBLIntensity = diffuseIBLIntensity; }

  float getCurrentEnergy() const;
 private:
  Renderer();
  static Renderer* instance;
  GLuint m_shaderProgram = 0;
  GLuint m_iblShaderProgram = 0;
  

  Camera* m_activeCamera = nullptr;

  std::list<Scene*> m_scenes;
  std::unordered_map<std::string, Material*> m_materials;
  std::unordered_map<std::string, Texture*> m_textures;

  IBLPrimitive* m_skybox = nullptr;
  GLint m_skyboxViewProjMatLoc = -1;

  SpecularIBLTexture* m_specularIBLTexture = nullptr;
  DiffuseIBLTexture* m_diffuseIBLTexture = nullptr;
  unsigned char m_diffusePixels[IBL_DIFFUSE_LENGTH * IBL_DIFFUSE_LENGTH * 4] = { 0, };
  
  Texture* m_lut_ggx = nullptr;

  int m_width = 0;
  int m_height = 0;

  //sound art
  GLuint m_specularIBLFrameBuffer = 0;
  GLuint m_diffuseIBLFrameBuffer = 0;
  SoundTexture* m_soundTexture = nullptr;
  float m_currentTime = 0.0f;
  float m_diffuseIBLIntensity = 1.0f;
  glm::vec4 m_backgroundColor = glm::vec4(0.0f);

  GLint m_artProgram = -1;
  GLint m_volumeUniformLoc = -1;
  GLint m_resolutionUniformLoc = -1;
  // GLint m_backgroundUniformLoc = -1;
  GLint m_timeUniformLoc = -1;
  GLint m_vertexCountUniformLoc = -1;
  GLint m_soundUniformLoc = -1;
  GLint m_soundUniformLoc2 = -1;
  GLint m_isStereoUniformLoc = -1;

  GLint m_specularIBLMapLoc = -1;
  GLint m_diffuseIBLColorLoc = -1;
  GLint m_lutGGXLoc = -1;
  GLint m_iblIntensityLoc = -1;
  GLint m_cameraWorldPosLoc = -1;
  GLint m_viewMatLoc = -1;
  GLint m_projMatLoc = -1;
};
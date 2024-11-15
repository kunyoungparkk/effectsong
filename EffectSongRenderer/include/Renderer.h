#pragma once
#include <list>
#include <unordered_map>
#include "common.h"

class Scene;
class Material;
class Texture;
class IBLTexture;
class Primitive;
class IBLPrimitive;
class SoundTexture;

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

  void update(float currentTime);
  void render();
  void addScene(Scene* scene);
  void removeScene(Scene* scene);
  
  Scene* getActiveScene();
  void setActiveScene(Scene* activeScene);

  Material* getMaterial(std::string name);
  void addMaterial(std::string name, Material* material);

  Texture* getTexture(std::string uri);
  void addTexture(std::string uri, Texture* texture);

  int getWidth();
  int getHeight();

  void setWidth(int width);
  void setHeight(int height);

  int getVertexCount();
  void setVertexCount(int vertexCount);
 private:
  Renderer();
  static Renderer* instance;
  GLuint m_shaderProgram = 0;
  GLuint m_iblShaderProgram = 0;
  GLuint m_artShaderProgram = 0;

  std::list<Scene*> m_scenes;
  std::unordered_map<std::string, Material*> m_materials;
  std::unordered_map<std::string, Texture*> m_textures;

  Scene* m_active_scene = nullptr;
  IBLPrimitive* m_skybox = nullptr;

  IBLTexture* m_specularIBLTexture = nullptr;
  IBLTexture* m_diffuseIBLTexture = nullptr;
  Texture* m_lut_ggx = nullptr;
  SoundTexture* m_soundTexture = nullptr;

  int m_width = 0;
  int m_height = 0;

  //sound art
  float m_currentTime = 0.0f;
  float m_backgroundColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
  int m_vertexCount = 10000;
};
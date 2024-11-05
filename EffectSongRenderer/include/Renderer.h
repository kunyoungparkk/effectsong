#pragma once
#include <list>
#include <unordered_map>
#include "common.h"
#include "Scene.h"
#include "Material.h"
#include "Texture.h"

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

  void update();
  void render();
  void addScene(Scene* scene);
  void removeScene(Scene* scene);
  
  Scene* getActiveScene();
  void setActiveScene(Scene* activeScene);

  Material* getMaterial(std::string name);
  void addMaterial(std::string name, Material* material);

  Texture* getTexture(std::string uri);
  void addTexture(std::string uri, Texture* texture);

 private:
  Renderer();
  static Renderer* instance;
  GLuint m_shaderProgram;

  std::list<Scene*> m_scenes;
  std::unordered_map<std::string, Material*> m_materials;
  std::unordered_map<std::string, Texture*> m_textures;

  Scene* m_active_scene;
};
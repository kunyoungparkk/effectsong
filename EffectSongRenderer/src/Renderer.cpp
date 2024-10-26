#include "renderer.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include "Texture.h"

Renderer* Renderer::instance = nullptr;

Renderer::Renderer()
    :  m_active_scene(nullptr) {
  // glew 초기화
  glewExperimental = true;
  if (glewInit() != GLEW_OK) {
    std::cout << "GLEW Init Failed" << std::endl;
    return;
  }

  // OpenGL 설정
  glViewport(0, 0, 640, 480);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
}

Renderer::~Renderer() {  }

void Renderer::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (auto iter = m_scenes.begin(); iter != m_scenes.end(); iter++) {
    (*iter)->render();
  }
}

void Renderer::addScene(Scene* scene) { m_scenes.push_back(scene); }

void Renderer::removeScene(Scene* scene) { m_scenes.remove(scene); }

Scene* Renderer::getActiveScene() { return m_active_scene; }

void Renderer::setActiveScene(Scene* activeScene) {
  m_active_scene = activeScene;
}

Material* Renderer::getMaterial(std::string name) { return m_materials[name]; }

void Renderer::addMaterial(std::string name, Material* material) {
  m_materials[name] = material;
}

Texture* Renderer::getTexture(std::string uri) { return m_textures[uri]; }

void Renderer::addTexture(std::string uri, Texture* texture) {
  m_textures[uri] = texture;
}

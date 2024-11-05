#include "renderer.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "Texture.h"

Renderer* Renderer::instance = nullptr;

std::string readFile(const char* filePath) {
    std::ifstream file(filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
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

  //shader progrma 설정

  std::string vertexShaderSource =
      readFile("../../src/vertex_shader.glsl");
  std::string fragmentShaderSource =
      readFile("../../src/fragment_shader.glsl");

  const char* cVertexShaderSource = vertexShaderSource.c_str();
  const char* cFragmentShaderSource = fragmentShaderSource.c_str();

  // vertex shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &cVertexShaderSource, NULL);
  glCompileShader(vertexShader);
  // Fragment Shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &cFragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  m_shaderProgram = glCreateProgram();
  glAttachShader(m_shaderProgram, vertexShader);
  glAttachShader(m_shaderProgram, fragmentShader);
  glLinkProgram(m_shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

Renderer::~Renderer() {  }

void Renderer::update() {
    for (auto iter = m_scenes.begin(); iter != m_scenes.end(); iter++) {
        (*iter)->update();
    }
}

void Renderer::render() {
  glUseProgram(m_shaderProgram);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (auto iter = m_scenes.begin(); iter != m_scenes.end(); iter++) {
    (*iter)->render(m_shaderProgram);
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

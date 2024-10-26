#pragma once
#include "common.h"

class Renderable {
 public:
  Renderable(GLTFData& data, GLenum primitiveMode);
  void bind(GLuint shaderProgram);
  void update();
  void render();
  ~Renderable();

  glm::mat4 getModelMatrix();

  glm::vec3 getPosition();
  glm::vec3 getRotation();
  glm::vec3 getScale();

  void setPosition(glm::vec3 position);
  void setRotation(glm::vec3 rotation);
  void setScale(glm::vec3 scale);

 private:
  GLTFData m_gltfData;
  GLuint VAO, VBO, EBO, m_texture;
  GLenum m_primitiveMode;
  glm::vec3 m_position;
  glm::vec3 m_rotation;
  glm::vec3 m_scale;
};
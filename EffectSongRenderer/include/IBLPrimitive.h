#pragma once
#include "common.h"
class IBLPrimitive {
 public:
  IBLPrimitive(std::vector<glm::vec3>& vertices, std::vector<GLuint>& indices);
  void render(GLuint shaderProgram);

 private:
  std::vector<glm::vec3> m_vertices;
  std::vector<GLuint> m_indices;

  GLuint VAO, VBO, EBO;
};
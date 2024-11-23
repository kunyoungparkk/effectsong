#pragma once
#include "common.h"
class IBLPrimitive {
 public:
  IBLPrimitive(std::vector<glm::vec3>& vertices, std::vector<GLuint>& indices);
  void render();
  void bind(GLuint shaderProgram);

 private:
  std::vector<glm::vec3> m_vertices;
  std::vector<GLuint> m_indices;
  GLint m_specularIBLTexLoc = -1;
  GLuint VAO, VBO, EBO;
};
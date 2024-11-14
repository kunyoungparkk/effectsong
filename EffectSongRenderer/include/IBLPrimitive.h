#pragma once
#include "common.h"
class IBLTexture;
class IBLPrimitive {
 public:
  IBLPrimitive(std::vector<glm::vec3>& vertices, std::vector<GLuint>& indices);
  void render(GLuint shaderProgram);

 private:
  std::vector<glm::vec3> m_vertices;
  std::vector<GLuint> m_indices;

  //IBLTexture* m_iblTexture;
  GLuint VAO, VBO, EBO;
};
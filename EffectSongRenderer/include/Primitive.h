#pragma once
#include "common.h"
#include "Material.h"

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texcoord;
  glm::vec3 tangent;
};

class Primitive {
 public:
  Primitive(cgltf_primitive* cgltfPrimitive);
  void render(GLuint shaderProgram);

 private:
  std::vector<Vertex> m_vertices;
  std::vector<glm::vec3> m_bitangents;
  std::vector<GLuint> m_indices;

  Material* m_material;
  GLuint VAO, VBO, EBO;
  GLenum m_primitiveMode;
};
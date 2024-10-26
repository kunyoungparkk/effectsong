#pragma once
#include "common.h"
#include "Material.h"

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texcoord;
};

class Primitive {
 public:
  Primitive(cgltf_primitive* cgltfPrimitive);

  void render(glm::mat4 modelMatrix, glm::mat4 projectionMatrix,
              glm::mat4 viewMatrix);

 private:
  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  Material* m_material;
  GLuint VAO, VBO, EBO;
  GLenum m_primitiveMode;
};
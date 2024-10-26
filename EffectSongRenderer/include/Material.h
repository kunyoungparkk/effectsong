#pragma once
#include "common.h"
#include "Texture.h"

class Material {
 public:
  Material(cgltf_material* material);
  void bind(glm::mat4 modelMatrix, glm::mat4 projectionMatrix,
            glm::mat4 viewMatrix);
 private:
  GLuint m_shaderProgram;
  cgltf_material* m_material;
  Texture* m_baseColorTexture = nullptr;
  Texture* m_metallicRoughnessTexture = nullptr;
  Texture* m_normalTexture = nullptr;
  Texture* m_aoTexture = nullptr;
};
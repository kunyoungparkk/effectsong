#pragma once
#include "common.h"
#include "Texture.h"

class Material {
 public:
  Material(cgltf_material* material);
  void bind(GLuint shaderProgram);
 private:
  cgltf_material* m_material;
  Texture* m_baseColorTexture = nullptr;
  Texture* m_emissiveTexture = nullptr;
  Texture* m_metallicRoughnessTexture = nullptr;
  Texture* m_normalTexture = nullptr;
  Texture* m_occlusionTexture = nullptr;
};
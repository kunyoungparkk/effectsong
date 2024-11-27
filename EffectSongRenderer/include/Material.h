#pragma once
#include "common.h"
#include "Texture.h"

class Material {
 public:
  Material(cgltf_material* material, std::string& gltfPath);
  void bind(GLuint shaderProgram);
 private:
  cgltf_material* m_material;
  Texture* m_baseColorTexture = nullptr;
  Texture* m_emissiveTexture = nullptr;
  Texture* m_metallicRoughnessTexture = nullptr;
  Texture* m_normalTexture = nullptr;
  Texture* m_occlusionTexture = nullptr;

  GLuint m_shaderProgram = -1;

  GLint m_baseColorTexLoc = -1;
  GLint m_useBaseColorTexLoc = -1;
  GLint m_metallicRoughnessTexLoc = -1;
  GLint m_useMetallicRoughnessTexLoc = -1;
  GLint m_emissiveTexLoc = -1;
  GLint m_useEmissiveTexLoc = -1;
  GLint m_normalTexLoc = -1;
  GLint m_useNormalTexLoc = -1;
  GLint m_occlusionTexLoc = -1;
  GLint m_useOcclusionTexLoc = -1;
  GLint m_baseColorLoc = -1;
  GLint m_specularColorLoc = -1;
  GLint m_emissionColorLoc = -1;
  GLint m_metallicLoc = -1;
  GLint m_roughnessLoc = -1;
};
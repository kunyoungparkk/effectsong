#pragma once
#include "Texture.h"
#include "common.h"

class Material {
 public:
  Material(cgltf_material* material, std::string& gltfPath);
  Material();
  void bind(GLuint shaderProgram);

 private:
  Texture* m_baseColorTexture = nullptr;
  Texture* m_metallicRoughnessTexture = nullptr;
  glm::vec4 m_baseColorFactor = glm::vec4(1.0f);
  float m_metallicFactor = 0.0f;
  float m_roughnessFactor = 1.0f;

  //TODO: clearcoat

  //TODO: ior

  //TODO: specular
  glm::vec3 m_specularColorFactor = glm::vec3(0.0f);

  //TODO: sheen

  //TODO: transmission

  //TODO: volume

  //TODO: emissive strength

  //TODO: iridescence

  //TODO: anisotropy

  //TODO: dispersion

  //normal
  Texture* m_normalTexture = nullptr;
  //occlusion
  Texture* m_occlusionTexture = nullptr;
  //emissive
  Texture* m_emissiveTexture = nullptr;
  glm::vec3 m_emissiveFactor = glm::vec3(0.0f);

  //TODO:
  //alpha mode
  //alpha cutoff
  //doublesided
  //unlit
  //extras, extension count, extensions ...

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
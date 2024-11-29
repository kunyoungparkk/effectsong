#include "Material.h"
#include "Renderer.h"
#include "Texture.h"

Material::Material(cgltf_material* material, std::string& gltfPath){
  if (material->pbr_metallic_roughness.base_color_texture.texture) {
      m_baseColorTexture = Renderer::getInstance()->getTexture(
          material->pbr_metallic_roughness.base_color_texture.texture, gltfPath, true);
  }
  if (material->pbr_metallic_roughness.metallic_roughness_texture.texture){
      m_metallicRoughnessTexture = Renderer::getInstance()->getTexture(
          material->pbr_metallic_roughness.metallic_roughness_texture.texture, gltfPath);
  }
  if (material->emissive_texture.texture) {
      m_emissiveTexture = Renderer::getInstance()->getTexture(
          material->emissive_texture.texture, gltfPath, true);
  }
  if (material->normal_texture.texture) {
      m_normalTexture = Renderer::getInstance()->getTexture(
          material->normal_texture.texture, gltfPath);
  }
  if (material->occlusion_texture.texture) {
      m_occlusionTexture = Renderer::getInstance()->getTexture(
          material->occlusion_texture.texture, gltfPath);
  }
  m_baseColorFactor = *(glm::vec4*)material->pbr_metallic_roughness.base_color_factor;
  m_specularColorFactor = *(glm::vec3*)material->specular.specular_color_factor;
  m_emissiveFactor = *(glm::vec3*)material->emissive_factor;
  m_metallicFactor = material->pbr_metallic_roughness.metallic_factor;
  m_roughnessFactor = material->pbr_metallic_roughness.roughness_factor;
}

Material::Material()
{
}

void Material::bind(GLuint shaderProgram) {
  //초기화시 텍스처(1) + IBL 텍스처(spec, diffuse, ggx) + sound texture(2)를 이미 추가한 뒤
  int textureIndex = 6;
  if (m_shaderProgram != shaderProgram) {
      m_shaderProgram = shaderProgram;
      m_baseColorTexLoc = glGetUniformLocation(shaderProgram, "baseColorTexture");
      m_useBaseColorTexLoc = glGetUniformLocation(shaderProgram, "useBaseColorTexture");
      m_metallicRoughnessTexLoc = glGetUniformLocation(shaderProgram, "metallicRoughnessTexture");
      m_useMetallicRoughnessTexLoc = glGetUniformLocation(shaderProgram, "useMetallicRoughnessTexture");
      m_emissiveTexLoc = glGetUniformLocation(shaderProgram, "emissiveTexture");
      m_useEmissiveTexLoc = glGetUniformLocation(shaderProgram, "useEmissiveTexture");
      m_normalTexLoc = glGetUniformLocation(shaderProgram, "normalTexture");
      m_useNormalTexLoc = glGetUniformLocation(shaderProgram, "useNormalTexture");
      m_occlusionTexLoc = glGetUniformLocation(shaderProgram, "occlusionTexture");
      m_useOcclusionTexLoc = glGetUniformLocation(shaderProgram, "useOcclusionTexture");
      m_baseColorLoc = glGetUniformLocation(shaderProgram, "material.baseColor");
      m_specularColorLoc = glGetUniformLocation(shaderProgram, "material.specularColor");
      m_emissionColorLoc = glGetUniformLocation(shaderProgram, "material.emissionColor");
      m_metallicLoc = glGetUniformLocation(shaderProgram, "material.metallic");
      m_roughnessLoc = glGetUniformLocation(shaderProgram, "material.roughness");
  }

  if (m_baseColorTexture) {
      m_baseColorTexture->bind(textureIndex);
      glUniform1i(m_baseColorTexLoc, textureIndex);
      textureIndex++;
      glUniform1i(m_useBaseColorTexLoc, GL_TRUE);
  }
  else {
      glUniform1i(m_useBaseColorTexLoc, GL_FALSE);
  }
  if (m_metallicRoughnessTexture) {
      m_metallicRoughnessTexture->bind(textureIndex);
      glUniform1i(m_metallicRoughnessTexLoc, textureIndex);
      textureIndex++;
      glUniform1i(m_useMetallicRoughnessTexLoc, GL_TRUE);
  }
  else {
      glUniform1i(m_useMetallicRoughnessTexLoc, GL_FALSE);
  }
  if (m_emissiveTexture) {
      m_emissiveTexture->bind(textureIndex);
      glUniform1i(m_emissiveTexLoc, textureIndex);
      textureIndex++;
      glUniform1i(m_useEmissiveTexLoc, GL_TRUE);
  }
  else {
      glUniform1i(m_useEmissiveTexLoc, GL_FALSE);
  }
  if (m_normalTexture) {
      m_normalTexture->bind(textureIndex);
      glUniform1i(m_normalTexLoc, textureIndex);
      textureIndex++;
      glUniform1i(m_useNormalTexLoc, GL_TRUE);
  }
  else {
      glUniform1i(m_useNormalTexLoc, GL_FALSE);
  }
  if (m_occlusionTexture) {
      m_occlusionTexture->bind(textureIndex);
      glUniform1i(m_occlusionTexLoc, textureIndex);
      textureIndex++;
      glUniform1i(m_useOcclusionTexLoc, GL_TRUE);
  }
  else {
      glUniform1i(m_useOcclusionTexLoc, GL_FALSE);
  }

  glUniform4fv(m_baseColorLoc, 1, glm::value_ptr(m_baseColorFactor));
  glUniform3fv(m_specularColorLoc, 1, glm::value_ptr(m_specularColorFactor));
  glUniform3fv(m_emissionColorLoc, 1, glm::value_ptr(m_emissiveFactor));
  glUniform1f(m_metallicLoc, m_metallicFactor);
  glUniform1f(m_roughnessLoc, m_roughnessFactor);
}

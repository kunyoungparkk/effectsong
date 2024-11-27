#include "Material.h"
#include "Renderer.h"
#include "Texture.h"

Material::Material(cgltf_material* material, std::string& gltfPath) : m_material(material) {
  if (m_material->pbr_metallic_roughness.base_color_texture.texture) {
      m_baseColorTexture = Renderer::getInstance()->getTexture(
          m_material->pbr_metallic_roughness.base_color_texture.texture, gltfPath);
  }
  if (m_material->pbr_metallic_roughness.metallic_roughness_texture.texture){
      m_metallicRoughnessTexture = Renderer::getInstance()->getTexture(
          m_material->pbr_metallic_roughness.metallic_roughness_texture.texture, gltfPath);
  }
  if (m_material->emissive_texture.texture) {
      m_emissiveTexture = Renderer::getInstance()->getTexture(
          m_material->emissive_texture.texture, gltfPath);
  }
  if (m_material->normal_texture.texture) {
      m_normalTexture = Renderer::getInstance()->getTexture(
          m_material->normal_texture.texture, gltfPath);
  }
  if (m_material->occlusion_texture.texture) {
      m_occlusionTexture = Renderer::getInstance()->getTexture(
          m_material->occlusion_texture.texture, gltfPath);
  }
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

  glUniform4fv(m_baseColorLoc, 1, m_material->pbr_metallic_roughness.base_color_factor);
  glUniform3fv(m_specularColorLoc, 1, m_material->specular.specular_color_factor);
  glUniform3fv(m_emissionColorLoc, 1, m_material->emissive_factor);
  glUniform1f(m_metallicLoc, m_material->pbr_metallic_roughness.metallic_factor);
  glUniform1f(m_roughnessLoc, m_material->pbr_metallic_roughness.roughness_factor);
}

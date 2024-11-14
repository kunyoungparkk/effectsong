#include "Material.h"
#include "Renderer.h"
#include "Texture.h"

Material::Material(cgltf_material* material) : m_material(material) {
  if (m_material->pbr_metallic_roughness.base_color_texture.texture) {
      m_baseColorTexture = Renderer::getInstance()->getTexture(
        m_material->pbr_metallic_roughness.base_color_texture.texture->image
            ->uri);
  }
  if (m_material->pbr_metallic_roughness.metallic_roughness_texture.texture){
      m_metallicRoughnessTexture = Renderer::getInstance()->getTexture(
          m_material->pbr_metallic_roughness.metallic_roughness_texture.texture->image->uri);
  }
  if (m_material->emissive_texture.texture) {
      m_emissiveTexture = Renderer::getInstance()->getTexture(
          m_material->emissive_texture.texture->image->uri);
  }
  if (m_material->normal_texture.texture) {
      m_normalTexture = Renderer::getInstance()->getTexture(
          m_material->normal_texture.texture->image->uri);
  }
  if (m_material->occlusion_texture.texture) {
      m_occlusionTexture = Renderer::getInstance()->getTexture(
          m_material->occlusion_texture.texture->image->uri);
  }
}

void Material::bind(GLuint shaderProgram) {
  //초기화시 텍스처(2) + IBL 텍스처를 이미 추가한 뒤
  int textureIndex = 5;
  
  if (m_baseColorTexture) {
      m_baseColorTexture->bind(textureIndex);
      GLint baseColorTexLoc =
          glGetUniformLocation(shaderProgram, "baseColorTexture");
      glUniform1i(baseColorTexLoc, textureIndex);
      textureIndex++;

      glUniform1i(glGetUniformLocation(shaderProgram, "useBaseColorTexture"), GL_TRUE);
  }
  else {
      glUniform1i(glGetUniformLocation(shaderProgram, "useBaseColorTexture"), GL_FALSE);
  }
  if (m_metallicRoughnessTexture) {
      m_metallicRoughnessTexture->bind(textureIndex);
      GLint metallicRoughnessTexLoc = glGetUniformLocation(shaderProgram, "metallicRoughnessTexture");
      glUniform1i(metallicRoughnessTexLoc, textureIndex);
      textureIndex++;
      glUniform1i(glGetUniformLocation(shaderProgram, "useMetallicRoughnessTexture"), GL_TRUE);
  }
  else {
      glUniform1i(glGetUniformLocation(shaderProgram, "useMetallicRoughnessTexture"), GL_FALSE);
  }
  if (m_emissiveTexture) {
      m_emissiveTexture->bind(textureIndex);
      GLint emissiveTexLoc = glGetUniformLocation(shaderProgram, "emissiveTexture");
      glUniform1i(emissiveTexLoc, textureIndex);
      textureIndex++;
      glUniform1i(glGetUniformLocation(shaderProgram, "useEmissiveTexture"), GL_TRUE);
  }
  else {
      glUniform1i(glGetUniformLocation(shaderProgram, "useEmissiveTexture"), GL_FALSE);
  }
  if (m_normalTexture) {
      m_normalTexture->bind(textureIndex);
      GLint normalTexLoc = glGetUniformLocation(shaderProgram, "normalTexture");
      glUniform1i(normalTexLoc, textureIndex);
      textureIndex++;
      glUniform1i(glGetUniformLocation(shaderProgram, "useNormalTexture"), GL_TRUE);
  }
  else {
      glUniform1i(glGetUniformLocation(shaderProgram, "useNormalTexture"), GL_FALSE);
  }
  if (m_occlusionTexture) {
      m_occlusionTexture->bind(textureIndex);
      GLint occlusionTexLoc = glGetUniformLocation(shaderProgram, "occlusionTexture");
      glUniform1i(occlusionTexLoc, textureIndex);
      textureIndex++;
      glUniform1i(glGetUniformLocation(shaderProgram, "useOcclusionTexture"), GL_TRUE);
  }
  else {
      glUniform1i(glGetUniformLocation(shaderProgram, "useOcclusionTexture"), GL_FALSE);
  }

  GLint baseColorLoc = glGetUniformLocation(shaderProgram, "material.baseColor");
  glUniform4fv(baseColorLoc, 1, m_material->pbr_metallic_roughness.base_color_factor);

  GLint specularColorLoc = glGetUniformLocation(shaderProgram, "material.specularColor");
  glUniform3fv(specularColorLoc, 1, m_material->specular.specular_color_factor);

  GLint emissionColorLoc = glGetUniformLocation(shaderProgram, "material.emissionColor");
  glUniform3fv(emissionColorLoc, 1, m_material->emissive_factor);
  
  GLint metallicLoc = glGetUniformLocation(shaderProgram, "material.metallic");
  glUniform1f(metallicLoc, m_material->pbr_metallic_roughness.metallic_factor);
  
  GLint roughnessLoc = glGetUniformLocation(shaderProgram, "material.roughness");
  glUniform1f(roughnessLoc, m_material->pbr_metallic_roughness.roughness_factor);

}

#include "Material.h"
#include "Renderer.h"
#include "Texture.h"

Material::Material(cgltf_material* material) : m_material(material) {
  // 임시로 base color texture만
  if (m_material->has_pbr_metallic_roughness) {
      m_baseColorTexture = Renderer::getInstance()->getTexture(
        m_material->pbr_metallic_roughness.base_color_texture.texture->image
            ->uri);
      m_material->pbr_metallic_roughness.metallic_roughness_texture;

  }
}

void Material::bind(GLuint shaderProgram) {
  //fragment shader
  if (m_baseColorTexture) {
      m_baseColorTexture->bind();
  }

  GLint baseColorLoc = glGetUniformLocation(shaderProgram, "material.baseColor");
  glUniform4fv(baseColorLoc, 1, m_material->pbr_metallic_roughness.base_color_factor);

  GLint specularColorLoc = glGetUniformLocation(shaderProgram, "material.specularColor");
  glUniform3fv(specularColorLoc, 1, m_material->specular.specular_color_factor);

  GLint emissionColorLoc = glGetUniformLocation(shaderProgram, "material.emissionColor");
  glUniform3fv(emissionColorLoc, 1, m_material->emissive_factor);

  GLint baseColorTexLoc =
      glGetUniformLocation(shaderProgram, "baseColorTexture");
  glUniform1i(baseColorTexLoc, 0);
}

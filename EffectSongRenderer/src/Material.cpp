#include "Material.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "Renderer.h"
#include "Texture.h"

std::string readFile(const char* filePath) {
  std::ifstream file(filePath);
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

Material::Material(cgltf_material* material) : m_material(material) {
  std::string vertexShaderSource =
      readFile("../../src/vertex_shader.glsl");
  std::string fragmentShaderSource =
      readFile("../../src/fragment_shader.glsl");

  const char* cVertexShaderSource = vertexShaderSource.c_str();
  const char* cFragmentShaderSource = fragmentShaderSource.c_str();

  // vertex shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &cVertexShaderSource, NULL);
  glCompileShader(vertexShader);
  // Fragment Shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &cFragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  m_shaderProgram = glCreateProgram();
  glAttachShader(m_shaderProgram, vertexShader);
  glAttachShader(m_shaderProgram, fragmentShader);
  glLinkProgram(m_shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // 임시로 base color texture만
  if (m_material->has_pbr_metallic_roughness) {
      m_baseColorTexture = Renderer::getInstance()->getTexture(
        m_material->pbr_metallic_roughness.base_color_texture.texture->image
            ->uri);
      m_material->pbr_metallic_roughness.metallic_roughness_texture;

  }
}

void Material::bind(glm::mat4 modelMatrix, glm::mat4 projectionMatrix,
                    glm::mat4 viewMatrix) {
  glUseProgram(m_shaderProgram);

  GLint worldMatLoc = glGetUniformLocation(m_shaderProgram, "worldMat");
  GLint viewMatLoc = glGetUniformLocation(m_shaderProgram, "viewMat");
  GLint projectionMatLoc = glGetUniformLocation(m_shaderProgram, "projMat");
  glUniformMatrix4fv(worldMatLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
  glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
  glUniformMatrix4fv(projectionMatLoc, 1, GL_FALSE,
                     glm::value_ptr(projectionMatrix));

  if (m_baseColorTexture) {
      m_baseColorTexture->bind();
  }
  GLint diffuseLoc = glGetUniformLocation(m_shaderProgram, "material.diffuse");

  GLint diffuseTexLoc =
      glGetUniformLocation(m_shaderProgram, "material.diffuseTexture");

  glUniform4fv(diffuseLoc, 1,
               m_material->pbr_metallic_roughness.base_color_factor);
  glUniform1i(diffuseTexLoc, 0);
}

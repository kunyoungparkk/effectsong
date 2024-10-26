#include "Renderable.h"

#include <glm/glm.hpp>
#include <vector>

#include "Texture.h"

Renderable::Renderable(GLTFData& data, GLenum primitiveMode)
    : m_gltfData(data),
      m_position(glm::vec3(0.0f)),
      m_rotation(glm::vec3(0.0f)),
      m_scale(glm::vec3(1.0f)),
      m_primitiveMode(primitiveMode){
  std::vector<Vertex> vertices;
  for (int i = 0; i < m_gltfData.vertices.size() / 3; i++) {
    Vertex vertex;
    vertex.position =
        glm::vec3(m_gltfData.vertices[i * 3], m_gltfData.vertices[i * 3 + 1],
                  m_gltfData.vertices[i * 3 + 2]);

    if (!m_gltfData.texcoords.empty()) {
      vertex.texcoord = glm::vec2(m_gltfData.texcoords[i * 2],
                                  m_gltfData.texcoords[i * 2 + 1]);
    }
    if (!m_gltfData.normals.empty()) {
      vertex.normal =
          glm::vec3(m_gltfData.normals[i * 3], m_gltfData.normals[i * 3 + 1],
                    m_gltfData.normals[i * 3 + 2]);
    }
    vertices.push_back(vertex);
  }

  m_texture = loadTexture(m_gltfData.materials[0]->pbr_metallic_roughness.base_color_texture.texture);

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // VBO, EBO : VAO에 종속적
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
               vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               m_gltfData.indices.size() * sizeof(GLuint),
               m_gltfData.indices.data(), GL_STATIC_DRAW);

  // attribute 설정 : VAO에 종속적 - > VAO에 정점 속성 정보 저장
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (const void*)offsetof(Vertex, position));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (const void*)offsetof(Vertex, normal));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (const void*)offsetof(Vertex, texcoord));
  glEnableVertexAttribArray(2);
}

void Renderable::bind(GLuint shaderProgram) {
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(VAO);
  glBindTexture(GL_TEXTURE_2D, m_texture);

  GLint diffuseLoc = glGetUniformLocation(shaderProgram, "material.diffuse");

  GLint diffuseTexLoc =
      glGetUniformLocation(shaderProgram, "material.diffuseTexture");
  
  glUniform4fv(diffuseLoc, 1, m_gltfData.materials[0]->pbr_metallic_roughness.base_color_factor);
  glUniform1i(diffuseTexLoc, 0);
}

void Renderable::update() {
  m_rotation.y += 1.0f; 
}

void Renderable::render() {
  // Draw
  glDrawElements(m_primitiveMode, m_gltfData.indices.size(), GL_UNSIGNED_INT,
                 0);
}

Renderable::~Renderable() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}

glm::mat4 Renderable::getModelMatrix() {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, m_position);
  model = glm::rotate(model, glm::radians(m_rotation.x), glm::vec3(1, 0, 0));
  model = glm::rotate(model, glm::radians(m_rotation.y), glm::vec3(0, 1, 0));
  model = glm::rotate(model, glm::radians(m_rotation.z), glm::vec3(0, 0, 1));
  model = glm::scale(model, m_scale);
  return model;
}

glm::vec3 Renderable::getPosition() { return m_position; }

glm::vec3 Renderable::getRotation() { return m_rotation; }

glm::vec3 Renderable::getScale() { return m_scale; }

void Renderable::setPosition(glm::vec3 position) { m_position = position; }

void Renderable::setRotation(glm::vec3 rotation) { m_rotation = rotation; }

void Renderable::setScale(glm::vec3 scale) { m_scale = scale; }

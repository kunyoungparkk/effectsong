#pragma once
#include "common.h"
class Texture {
 public:
  Texture(std::string& gltfPath, cgltf_texture* cgltf_texture);
  Texture(std::string& path);
  Texture(int width, int height, int nrChannels, GLint wrapS, GLint wrapT, GLint minFilter, GLint maxFilter);
  ~Texture();
  void bind(int texIdx);
  GLuint getId() { return m_textureID; }
 private:
  cgltf_texture* m_cgltfTexture;
  GLuint m_textureID;
  unsigned char* m_data;
};

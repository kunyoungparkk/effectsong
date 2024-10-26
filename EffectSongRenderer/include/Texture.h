#pragma once
#include "common.h"
class Texture {
 public:
  Texture(std::string& gltfPath, cgltf_texture* cgltf_texture);
  ~Texture();
  void bind();
 private:
  cgltf_texture* m_cgltfTexture;
  GLuint m_textureID;
  unsigned char* m_data;
};

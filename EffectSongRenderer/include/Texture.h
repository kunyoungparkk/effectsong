#pragma once
#include "common.h"
class Texture {
 public:
  Texture(std::string& gltfPath, cgltf_texture* cgltf_texture, bool bSRGB = false);
  Texture(std::string& path);
  ~Texture();
  void bind(int texIdx);
 private:
  cgltf_texture* m_cgltfTexture;
  GLuint m_textureID;
  unsigned char* m_data;
};

#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>

#include "stb_image.h"

Texture::Texture(std::string& gltfPath, cgltf_texture* cgltf_texture, bool bSRGB)
    : m_cgltfTexture(cgltf_texture) {
  int width, height, nrChannels;
  std::string combinedPath = gltfPath + cgltf_texture->image->uri;
  m_data = stbi_load(combinedPath.c_str(), &width, &height, &nrChannels, 0);

  if (!m_data) {
    std::cerr << "Failed to load texture: " << combinedPath << std::endl;
    exit(1);
  }

  glGenTextures(1, &m_textureID);
  glBindTexture(GL_TEXTURE_2D, m_textureID);
  
  // 텍스처 설정
  if (cgltf_texture->sampler)
  {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
          cgltf_texture->sampler->wrap_s);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
          cgltf_texture->sampler->wrap_t);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
          cgltf_texture->sampler->min_filter);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
          cgltf_texture->sampler->mag_filter);
  }
  
  // 텍스처 데이터 생성
  if (nrChannels == 3) {
    glTexImage2D(GL_TEXTURE_2D, 0, bSRGB ? GL_SRGB:GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, m_data);
  } else if (nrChannels == 4) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, m_data);
  }
  glGenerateMipmap(GL_TEXTURE_2D);
}

Texture::~Texture() { stbi_image_free(m_data); }

void Texture::bind(int texIdx) {
  glActiveTexture(GL_TEXTURE0 + texIdx);
  glBindTexture(GL_TEXTURE_2D, m_textureID);
}

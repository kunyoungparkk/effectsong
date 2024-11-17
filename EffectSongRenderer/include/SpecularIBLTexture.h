#pragma once
#include "common.h"
class SpecularIBLTexture {
 public:
  SpecularIBLTexture();
  ~SpecularIBLTexture();
  void bind(int texIdx);
  GLuint getId() { return m_textureID; }
  GLuint getDepthTextureId() { return m_depthTextureID; }
 private:
  GLuint m_textureID;
  GLuint m_depthTextureID;
};

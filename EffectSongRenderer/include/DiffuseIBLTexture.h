#pragma once
#include "common.h"
class DiffuseIBLTexture {
 public:
  DiffuseIBLTexture(int width, int height, int nrChannels, GLint wrapS, GLint wrapT, GLint minFilter, GLint maxFilter);
  ~DiffuseIBLTexture();
  void bind(int texIdx);
  GLuint getId() { return m_textureID; }
  GLuint getDepthTextureId() { return m_depthTextureID; }
 private:
  GLuint m_textureID;
  GLuint m_depthTextureID;
};

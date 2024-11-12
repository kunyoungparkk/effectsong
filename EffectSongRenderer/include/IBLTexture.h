#pragma once
#include "common.h"
class IBLTexture {
 public:
  IBLTexture(std::string& path);
  ~IBLTexture();
  void bind(int texIdx);
 private:
  GLuint m_textureID;
  unsigned char* m_data;
};

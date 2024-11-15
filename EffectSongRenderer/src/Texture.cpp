#include "Texture.h"
#include "IBLTexture.h"
#include "SoundTexture.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
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
	glActiveTexture(GL_TEXTURE0);
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
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	// 텍스처 데이터 생성
	if (nrChannels == 3) {
		glTexImage2D(GL_TEXTURE_2D, 0, bSRGB ? GL_SRGB : GL_RGB, width, height, 0, GL_RGB,
			GL_UNSIGNED_BYTE, m_data);
	}
	else if (nrChannels == 4) {
		glTexImage2D(GL_TEXTURE_2D, 0, bSRGB ? GL_SRGB_ALPHA : GL_RGBA, width, height, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, m_data);
	}
	glGenerateMipmap(GL_TEXTURE_2D);
}

Texture::Texture(std::string& path)
	: m_cgltfTexture(nullptr) {
	int width, height, nrChannels;
	m_data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

	if (!m_data) {
		std::cerr << "Failed to load texture: " << path << std::endl;
		exit(1);
	}

	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	// 텍스처 설정
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
		GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
		GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		GL_LINEAR);

	// 텍스처 데이터 생성
	if (nrChannels == 3) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
			GL_UNSIGNED_BYTE, m_data);
	}
	else if (nrChannels == 4) {
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

IBLTexture::IBLTexture()
{
	glGenTextures(1, &m_textureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);

	glGenRenderbuffers(1, &m_depthTextureID);
	glBindRenderbuffer(GL_RENDERBUFFER, m_depthTextureID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, SOUND_TEXTURE_WIDTH, SOUND_TEXTURE_HEIGHT);
	for (int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, SOUND_TEXTURE_WIDTH, SOUND_TEXTURE_HEIGHT, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, nullptr);
	}
	// 텍스처 설정
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
		GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
		GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
		GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,
		GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

IBLTexture::~IBLTexture() {
	glDeleteTextures(1, &m_textureID);
}

void IBLTexture::bind(int texIdx) {
	glActiveTexture(GL_TEXTURE0 + texIdx);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
}

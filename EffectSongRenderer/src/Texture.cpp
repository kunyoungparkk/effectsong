#include "Texture.h"
#include "SpecularIBLTexture.h"
#include "DiffuseIBLTexture.h"
#include "SoundTexture.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static const std::string base64Chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

std::vector<unsigned char> decodeBase64(const std::string& encodedString) {
	// find position ';base64,'
	size_t base64Start = encodedString.find(";base64,");
	if (base64Start == std::string::npos) {
		return {};
	}

	std::string base64Data = encodedString.substr(base64Start + 8);

	std::vector<unsigned char> decodedData;
	int val = 0, valb = -8;
	for (unsigned char c : base64Data) {
		if (base64Chars.find(c) == std::string::npos) break;
		val = (val << 6) + base64Chars.find(c);
		valb += 6;
		if (valb >= 0) {
			decodedData.push_back((val >> valb) & 0xFF);
			valb -= 8;
		}
	}
	return decodedData;
}

Texture::Texture(std::string& gltfPath, cgltf_texture* cgltf_texture, bool isSRGB)
	: m_cgltfTexture(cgltf_texture) {

	int width, height, nrChannels;

	cgltf_image* image = cgltf_texture->image;
	//embedded gltf
	if (image->uri && strncmp(image->uri, "data:", 5) == 0) {
		std::vector<unsigned char> decodedData = decodeBase64(image->uri + 5);
		m_data = stbi_load_from_memory(
			decodedData.data(), decodedData.size(), &width, &height, &nrChannels, 0);
	}
	//general gltf
	else if (image->uri) {
		std::string combinedPath = gltfPath + cgltf_texture->image->uri;
		m_data = stbi_load(combinedPath.c_str(), &width, &height, &nrChannels, 0);
	}
	//glb
	else if (image->buffer_view) {
		// GLB 버퍼 텍스처
		const unsigned char* bufferData = static_cast<const unsigned char*>(
			image->buffer_view->buffer->data) + image->buffer_view->offset;
		m_data = stbi_load_from_memory(
			bufferData, image->buffer_view->size, &width, &height, &nrChannels, 0);
	}
	else {
		// ERROR!
		std::cerr << "Failed to load texture: " << std::endl;
		exit(1);
	}

	if (!m_data) {
		std::cerr << "Failed to load texture: " << std::endl;
		exit(1);
	}

	glGenTextures(1, &m_textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	GLuint defaultWrapS = GL_REPEAT;
	GLuint defaultWrapT = GL_REPEAT;
	GLuint defaultMin = GL_LINEAR_MIPMAP_LINEAR;
	GLuint defaultMag = GL_LINEAR;

	if (cgltf_texture->sampler)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
			cgltf_texture->sampler->wrap_s ? cgltf_texture->sampler->wrap_s : defaultWrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
			cgltf_texture->sampler->wrap_t ? cgltf_texture->sampler->wrap_t : defaultWrapT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			cgltf_texture->sampler->min_filter ? cgltf_texture->sampler->min_filter : defaultMin);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
			cgltf_texture->sampler->mag_filter ? cgltf_texture->sampler->mag_filter : defaultMag);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, defaultWrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, defaultWrapT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, defaultMin);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, defaultMag);
	}
	//TODO: emscripten SRGB is not working at mipmap
	if (nrChannels == 4) {
		glTexImage2D(GL_TEXTURE_2D, 0, /*isSRGB ? GL_SRGB8_ALPHA8 : */GL_RGBA, width, height, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, m_data);
	}
	else {
		glTexImage2D(GL_TEXTURE_2D, 0, /*isSRGB ? GL_SRGB8 :*/ GL_RGB, width, height, 0, GL_RGB,
			GL_UNSIGNED_BYTE, m_data);
	}
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
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
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	// �ؽ�ó ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
		GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
		GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		GL_LINEAR);

	// �ؽ�ó ������ ����
	if (nrChannels == 3) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
			GL_UNSIGNED_BYTE, m_data);
	}
	else if (nrChannels == 4) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, m_data);
	}
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(int width, int height, int nrChannels, GLint wrapS, GLint wrapT, GLint minFilter, GLint maxFilter)
	: m_cgltfTexture(nullptr), m_data(nullptr) {
	glGenTextures(1, &m_textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	// �ؽ�ó ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
		wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
		wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		maxFilter);

	// �ؽ�ó ������ ����
	if (nrChannels == 3) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
			GL_UNSIGNED_BYTE, m_data);
	}
	else if (nrChannels == 4) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, m_data);
	}
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() { if (m_data) { stbi_image_free(m_data); } }

void Texture::bind(int texIdx) {
	glActiveTexture(GL_TEXTURE0 + texIdx);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
}

SpecularIBLTexture::SpecularIBLTexture()
{
	glGenTextures(1, &m_textureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);

	glGenRenderbuffers(1, &m_depthTextureID);
	glBindRenderbuffer(GL_RENDERBUFFER, m_depthTextureID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 2048, 2048);
	for (int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, 2048, 2048, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, nullptr);
	}
	// �ؽ�ó ����
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
		GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
		GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
		GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,
		GL_LINEAR);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

SpecularIBLTexture::~SpecularIBLTexture() {
	glDeleteTextures(1, &m_textureID);
}

void SpecularIBLTexture::bind(int texIdx) {
	glActiveTexture(GL_TEXTURE0 + texIdx);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

DiffuseIBLTexture::DiffuseIBLTexture()
{
	glGenTextures(1, &m_textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	glGenRenderbuffers(1, &m_depthTextureID);
	glBindRenderbuffer(GL_RENDERBUFFER, m_depthTextureID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 64, 64);

	// �ؽ�ó ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
		GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
		GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, nullptr);

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

DiffuseIBLTexture::~DiffuseIBLTexture() {
	glDeleteTextures(1, &m_textureID);
}

void DiffuseIBLTexture::bind(int texIdx) {
	glActiveTexture(GL_TEXTURE0 + texIdx);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
}
#include "renderer.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "Scene.h"
#include "Material.h"
#include "Texture.h"
#include "Primitive.h"
#include "IBLTexture.h"
#include "IBLPrimitive.h"
#include "SoundTexture.h"
#include "ArtShader.h"

Renderer* Renderer::instance = nullptr;

std::string readFile(const char* filePath) {
	std::ifstream file(filePath);
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}
Renderer::Renderer()
	: m_active_scene(nullptr) {
	// glew 초기화
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW Init Failed" << std::endl;
		return;
	}

	// OpenGL 설정
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//general shader program
	std::string vertexShaderSource =
		readFile("../../src/vertex_shader.glsl");
	std::string fragmentShaderSource =
		readFile("../../src/fragment_shader.glsl");
	const char* cVertexShaderSource = vertexShaderSource.c_str();
	const char* cFragmentShaderSource = fragmentShaderSource.c_str();
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &cVertexShaderSource, NULL);
	glCompileShader(vertexShader);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &cFragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	m_shaderProgram = glCreateProgram();
	glAttachShader(m_shaderProgram, vertexShader);
	glAttachShader(m_shaderProgram, fragmentShader);
	glLinkProgram(m_shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//ibl shader program
	std::string iblVertexShaderSource =
		readFile("../../src/ibl_vertex_shader.glsl");
	std::string iblFragmentShaderSource =
		readFile("../../src/ibl_fragment_shader.glsl");
	const char* cIBLVertexShaderSource = iblVertexShaderSource.c_str();
	const char* cIBLFragmentShaderSource = iblFragmentShaderSource.c_str();
	GLuint iblVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(iblVertexShader, 1, &cIBLVertexShaderSource, NULL);
	glCompileShader(iblVertexShader);
	GLuint iblFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(iblFragmentShader, 1, &cIBLFragmentShaderSource, NULL);
	glCompileShader(iblFragmentShader);
	m_iblShaderProgram = glCreateProgram();
	glAttachShader(m_iblShaderProgram, iblVertexShader);
	glAttachShader(m_iblShaderProgram, iblFragmentShader);
	glLinkProgram(m_iblShaderProgram);
	glDeleteShader(iblVertexShader);
	glDeleteShader(iblFragmentShader);

	//shader art
	glGenTextures(1, &m_artTextureBuffer);
	glBindTexture(GL_TEXTURE_2D, m_artTextureBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenFramebuffers(1, &m_artFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_artFrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_artTextureBuffer, 0);

	//GLuint depthRenderBuffer;
	//glGenRenderbuffers(1, &depthRenderBuffer);
	//glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_width, m_height);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer is not complete!" << std::endl;
	}
	glActiveTexture(GL_TEXTURE0 + 12);
	glBindTexture(GL_TEXTURE_2D, m_artTextureBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//IBL
	std::string ggxPath = "../../res/IBL/lut_ggx.png";
	m_lut_ggx = new Texture(ggxPath);

	std::string specularPath = "../../res/IBL/IBL_specular_image.png";
	std::string diffusePath = "../../res/IBL/IBL_diffuse_image.png";
	m_specularIBLTexture = new IBLTexture(specularPath);
	m_diffuseIBLTexture = new IBLTexture(diffusePath);
	//skybox
	std::vector<glm::vec3> skyboxVerts = { glm::vec3(-1, -1, -1), glm::vec3(1, -1, -1), glm::vec3(1, 1, -1), glm::vec3(-1, 1, -1),
		glm::vec3(-1, -1, 1), glm::vec3(1, -1, 1), glm::vec3(1, 1, 1), glm::vec3(-1, 1, 1)
	};
	std::vector<GLuint> skyboxIndices = {
		0, 1, 2,  0, 2, 3,
		4, 6, 5,  4, 7, 6,
		0, 4, 5,  0, 5, 1,
		2, 6, 7,  2, 7, 3,
		0, 3, 7,  0, 7, 4,
		1, 5, 6,  1, 6, 2
	};
	m_skybox = new IBLPrimitive(skyboxVerts, skyboxIndices);

	m_specularIBLTexture->bind(2);
	m_diffuseIBLTexture->bind(3);
	m_lut_ggx->bind(4);

	//sound texture
	m_soundTexture = new SoundTexture();
	m_soundTexture->loadWavFile("../../res/music/agi.wav");
	m_soundTexture->bind(5, 6);
}

Renderer::~Renderer() {
	//일단 GLTF 관련도 여기서 해제
	for (auto iter = m_scenes.begin(); iter != m_scenes.end(); iter++) {
		delete *iter;
	}
	for (auto iter = m_materials.begin(); iter != m_materials.end(); iter++) {
		delete iter->second;
	}
	for (auto iter = m_textures.begin(); iter != m_textures.end(); iter++) {
		delete iter->second;
	}

	if (m_skybox){
		delete m_skybox;
	}
	if (m_lut_ggx) {
		delete m_lut_ggx;
	}
	if (m_specularIBLTexture) {
		delete m_specularIBLTexture;
	}
	if (m_diffuseIBLTexture) {
		delete m_diffuseIBLTexture;
	}
	if (m_soundTexture) {
		delete m_soundTexture;
	}
}

void Renderer::update(float currentTime) {
	m_currentTime = currentTime;
	m_soundTexture->update(currentTime);
	for (auto iter = m_scenes.begin(); iter != m_scenes.end(); iter++) {
		(*iter)->update();
	}
}

void Renderer::render() {
	glViewport(0, 0, m_width, m_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//vertex shader art
	glBindFramebuffer(GL_FRAMEBUFFER, m_artFrameBuffer);
	ArtShader::getInstance()->bind();
	GLuint artProgram = ArtShader::getInstance()->getProgram();
	glDepthMask(GL_TRUE);
	//TODO:touch coord input(-1 ~ 1, -1 ~ 1)
	GLint touchUniformLoc = glGetUniformLocation(artProgram, "touch");

	GLint resolutionUniformLoc = glGetUniformLocation(artProgram, "resolution");
	GLint resolution[2] = { m_width, m_height };
	glUniform2iv(resolutionUniformLoc, 1, resolution);

	GLint backgroundUniformLoc = glGetUniformLocation(artProgram, "background");
	glUniform4fv(backgroundUniformLoc, 1, m_backgroundColor);

	GLint timeUniformLoc = glGetUniformLocation(artProgram, "time");
	glUniform1f(timeUniformLoc, m_currentTime);

	GLint vertexCountUniformLoc = glGetUniformLocation(artProgram, "vertexCount");
	glUniform1i(vertexCountUniformLoc, m_vertexCount);

	GLint soundUniformLoc = glGetUniformLocation(artProgram, "sound");
	glUniform1i(soundUniformLoc, 5);
	GLint soundUniformLoc2 = glGetUniformLocation(artProgram, "sound2");
	glUniform1i(soundUniformLoc2, 6);
	GLint isStereoUniformLoc = glGetUniformLocation(artProgram, "isStereo");
	if (m_soundTexture->getChannelCount() == 2) {
		glUniform1i(isStereoUniformLoc, true);
	}
	else {
		glUniform1i(isStereoUniformLoc, false);
	}
	ArtShader::getInstance()->render();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	//TODO: multi scene 처리
	for (auto iter = m_scenes.begin(); iter != m_scenes.end(); iter++) {
		//skybox
		glDepthMask(GL_FALSE);
		glUseProgram(m_iblShaderProgram);
		Camera* activeCamera = (*iter)->getActiveCamera();
		if (activeCamera)
		{
			glm::mat4 projectionMatrix = glm::perspective(
				glm::radians(activeCamera->fov),(float)m_width / (float)m_height,
				activeCamera->zNear, activeCamera->zFar);
			glm::mat4 viewMatrix = glm::inverse(activeCamera->getNode()->getModelMatrix());

			GLint viewMatLoc = glGetUniformLocation(m_iblShaderProgram, "viewMat");
			GLint projectionMatLoc = glGetUniformLocation(m_iblShaderProgram, "projMat");
			glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(projectionMatLoc, 1, GL_FALSE,
				glm::value_ptr(projectionMatrix));

			m_skybox->render(m_iblShaderProgram);
		}

		glDepthMask(GL_TRUE);
		glUseProgram(m_shaderProgram);
		GLint specularIBLTexLoc = glGetUniformLocation(m_shaderProgram, "specularIBLMap");
		glUniform1i(specularIBLTexLoc, 2);
		GLint diffuseIBLTexLoc = glGetUniformLocation(m_shaderProgram, "diffuseIBLMap");
		glUniform1i(diffuseIBLTexLoc, 3);
		GLint lutGGXTexLoc = glGetUniformLocation(m_shaderProgram, "lutGGX");
		glUniform1i(lutGGXTexLoc, 4);

		//GLint leftSoundTexLoc = glGetUniformLocation(m_shaderProgram, "leftSoundTexture");
		//glUniform1i(leftSoundTexLoc, 5);
		//GLint rightSoundTexLoc = glGetUniformLocation(m_shaderProgram, "rightSoundTexture");
		//glUniform1i(rightSoundTexLoc, 6);

		GLint iblItensityLoc = glGetUniformLocation(m_shaderProgram, "iblIntensity");
		glUniform1f(iblItensityLoc, 1.0f);
		//scene
		(*iter)->render(m_shaderProgram);

	}
}

void Renderer::addScene(Scene* scene) { m_scenes.push_back(scene); }

void Renderer::removeScene(Scene* scene) { m_scenes.remove(scene); }

Scene* Renderer::getActiveScene() { return m_active_scene; }

void Renderer::setActiveScene(Scene* activeScene) {
	m_active_scene = activeScene;
}

Material* Renderer::getMaterial(std::string name) { return m_materials[name]; }

void Renderer::addMaterial(std::string name, Material* material) {
	m_materials[name] = material;
}

Texture* Renderer::getTexture(std::string uri) { return m_textures[uri]; }

void Renderer::addTexture(std::string uri, Texture* texture) {
	m_textures[uri] = texture;
}

int Renderer::getWidth()
{
	return m_width;
}

int Renderer::getHeight()
{
	return m_height;
}

void Renderer::setWidth(int width)
{
	m_width = width;
}

void Renderer::setHeight(int height)
{
	m_height = height;
}

int Renderer::getVertexCount()
{
	return m_vertexCount;
}

void Renderer::setVertexCount(int vertexCount)
{
	m_vertexCount = vertexCount;
}

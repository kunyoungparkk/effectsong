#include "Renderer.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "Scene.h"
#include "Material.h"
#include "Texture.h"
#include "Primitive.h"
#include "SpecularIBLTexture.h"
#include "DiffuseIBLTexture.h"
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
Renderer::Renderer(){
#if _WIN64
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW Init Failed" << std::endl;
		return;
	}
#endif
	// OpenGL
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	//glEnable(GL_PROGRAM_POINT_SIZE);

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

	//IBL
	std::string ggxPath = "../../res/IBL/lut_ggx.png";
	m_lut_ggx = new Texture(ggxPath);

	//shader art
	glGenFramebuffers(1, &m_specularIBLFrameBuffer);
	glGenFramebuffers(1, &m_diffuseIBLFrameBuffer);

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

	m_specularIBLTexture = new SpecularIBLTexture();
	m_specularIBLTexture->bind(2);
	m_diffuseIBLTexture = new DiffuseIBLTexture(64, 64, 4, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR);
	m_diffuseIBLTexture->bind(3);
	m_lut_ggx->bind(4);

	m_soundTexture = new SoundTexture();
}

Renderer::~Renderer() {
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

void Renderer::update(float currentTime, bool isPlay) {
	m_currentTime = currentTime;
	m_soundTexture->update(currentTime, isPlay);

	float soundSize = m_soundTexture->getCurrentEnergy();
	Node* node = getSceneAt(0)->getNodeAt(0);
	node->setScale(glm::vec3(soundSize * 0.1f + 0.9f));
	for (auto iter = m_scenes.begin(); iter != m_scenes.end(); iter++) {
		(*iter)->update();
	}
}

void Renderer::render() {
	/*vertex shader art*/
	glBindFramebuffer(GL_FRAMEBUFFER, m_specularIBLFrameBuffer);
	glDisable(GL_CULL_FACE);
	glViewport(0, 0, 2048, 2048);
	glClearColor(m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, m_backgroundColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLuint artProgram = ArtShader::getInstance()->getProgram();
	glUseProgram(artProgram);
	glDepthMask(GL_TRUE);
	//TODO:touch coord input(-1 ~ 1, -1 ~ 1)
	GLint touchUniformLoc = glGetUniformLocation(artProgram, "touch");

	GLint volumeUniformLoc = glGetUniformLocation(artProgram, "volume");
	glUniform1f(volumeUniformLoc, m_soundTexture->getCurrentEnergy());

	GLint resolutionUniformLoc = glGetUniformLocation(artProgram, "resolution");
	float resolution[2] = { 2048.0f, 2048.0f };
	glUniform2fv(resolutionUniformLoc, 1, resolution);

	GLint backgroundUniformLoc = glGetUniformLocation(artProgram, "background");
	glUniform4fv(backgroundUniformLoc, 1, glm::value_ptr(m_backgroundColor));

	GLint timeUniformLoc = glGetUniformLocation(artProgram, "time");
	glUniform1f(timeUniformLoc, m_currentTime);

	GLint vertexCountUniformLoc = glGetUniformLocation(artProgram, "vertexCount");
	glUniform1f(vertexCountUniformLoc, (float)ArtShader::getInstance()->getVertexCount());

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

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, m_specularIBLTexture->getId(), 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_specularIBLTexture->getDepthTextureId());
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer is not complete!" << std::endl;
	}
	ArtShader::getInstance()->render();
	m_specularIBLTexture->bind(2);
	for (int i = 1; i < 6; i++) {
		glCopyTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, 0, 0, 2048, 2048);
	}

	/*diffuse texture->IBL_DIFFUSE_LENGTH* IBL_DIFFUSE_LENGTH*/
	glBindFramebuffer(GL_FRAMEBUFFER, m_diffuseIBLFrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_diffuseIBLTexture->getId(), 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_diffuseIBLTexture->getDepthTextureId());
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer is not complete!" << std::endl;
	}
	glViewport(0, 0, IBL_DIFFUSE_LENGTH, IBL_DIFFUSE_LENGTH);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ArtShader::getInstance()->render();

	m_diffuseIBLTexture->bind(3);
	glReadPixels(0, 0, IBL_DIFFUSE_LENGTH, IBL_DIFFUSE_LENGTH, GL_RGBA, GL_UNSIGNED_BYTE, m_diffusePixels);
	//glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_diffusePixels);
	glm::vec4 diffuseIBLColor = glm::vec4(0.0f);
	for (int i = 0; i < IBL_DIFFUSE_LENGTH; i++) {
		for (int j = 0; j < IBL_DIFFUSE_LENGTH; j++) {
			int index = 4 * (IBL_DIFFUSE_LENGTH * i + j);
			diffuseIBLColor.r += m_diffusePixels[index] / 255.0f;
			diffuseIBLColor.g += m_diffusePixels[index + 1] / 255.0f;
			diffuseIBLColor.b += m_diffusePixels[index + 2] / 255.0f;
			diffuseIBLColor.a += m_diffusePixels[index + 3] / 255.0f;
		}
	}
	diffuseIBLColor *= m_diffuseIBLIntensity / IBL_DIFFUSE_LENGTH / IBL_DIFFUSE_LENGTH;

	/*scene rendering*/
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, m_width, m_height);
	glEnable(GL_CULL_FACE);

	//skybox
	glDepthMask(GL_FALSE);
	glUseProgram(m_iblShaderProgram);
	if (m_activeCamera)
	{
		glm::mat4 projectionMatrix = glm::perspective(
			glm::radians(m_activeCamera->fov), (float)m_width / m_height,
			m_activeCamera->zNear, m_activeCamera->zFar);
		glm::mat4 viewMatrix = glm::inverse(m_activeCamera->getNode()->getModelMatrix());

		GLint viewMatLoc = glGetUniformLocation(m_iblShaderProgram, "viewMat");
		GLint projectionMatLoc = glGetUniformLocation(m_iblShaderProgram, "projMat");
		glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projectionMatLoc, 1, GL_FALSE,
			glm::value_ptr(projectionMatrix));

		m_skybox->render(m_iblShaderProgram);
	}

	glDepthMask(GL_TRUE);
	for (auto iter = m_scenes.begin(); iter != m_scenes.end(); iter++) {
		glUseProgram(m_shaderProgram);
		GLint specularIBLTexLoc = glGetUniformLocation(m_shaderProgram, "specularIBLMap");
		glUniform1i(specularIBLTexLoc, 2);
		GLint diffuseIBLTexLoc = glGetUniformLocation(m_shaderProgram, "diffuseIBLColor");
		glUniform4fv(diffuseIBLTexLoc, 1, glm::value_ptr(diffuseIBLColor));
		GLint lutGGXTexLoc = glGetUniformLocation(m_shaderProgram, "lutGGX");
		glUniform1i(lutGGXTexLoc, 4);
		//sound tex (5,6) �̹� ���ε� (update)
		GLint iblItensityLoc = glGetUniformLocation(m_shaderProgram, "iblIntensity");
		glUniform1f(iblItensityLoc, 1.0f);

		//camera
		glm::mat4 projectionMatrix = glm::mat4(1.0f);
		glm::mat4 viewMatrix = glm::mat4(1.0f);
		if (m_activeCamera) {
			projectionMatrix = glm::perspective(
				glm::radians(m_activeCamera->fov),
				(float)Renderer::getInstance()->getWidth() / Renderer::getInstance()->getHeight(),
				m_activeCamera->zNear, m_activeCamera->zFar);
			viewMatrix = glm::inverse(m_activeCamera->getNode()->getModelMatrix());

			GLint camPosLoc = glGetUniformLocation(m_shaderProgram, "cameraWorldPos");
			glUniform3fv(camPosLoc, 1, glm::value_ptr(m_activeCamera->getNode()->getGlobalPosition()));
		}
		GLint viewMatLoc = glGetUniformLocation(m_shaderProgram, "viewMat");
		GLint projectionMatLoc = glGetUniformLocation(m_shaderProgram, "projMat");
		glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projectionMatLoc, 1, GL_FALSE,
			glm::value_ptr(projectionMatrix));

		//scene
		(*iter)->render(m_shaderProgram);
	}
}

void Renderer::addScene(Scene* scene) { m_scenes.push_back(scene); }

void Renderer::removeScene(Scene* scene) { m_scenes.remove(scene); }

Scene* Renderer::getSceneAt(int index)
{
	if (m_scenes.size() <= index) {
		return nullptr;
	}
	auto iter = m_scenes.begin();
	std::advance(iter, index);

	return *iter;
}

Scene* Renderer::getSceneByName(std::string name)
{
	for (auto iter = m_scenes.begin(); iter != m_scenes.end(); iter++) {
		if ((*iter)->getName() == name) {
			return *iter;
		}
	}
	return nullptr;
}

void Renderer::setActiveCamera(Camera* camera)
{
	m_activeCamera = camera;
}

Camera* Renderer::getActiveCamera()
{
	return m_activeCamera;
}

Material* Renderer::getMaterial(std::string name) { return m_materials[name]; }

void Renderer::addMaterial(std::string name, Material* material) {
	m_materials[name] = material;
}

Texture* Renderer::getTexture(std::string uri) { return m_textures[uri]; }

void Renderer::addTexture(std::string uri, Texture* texture) {
	m_textures[uri] = texture;
}

int Renderer::getWidth() const
{
	return m_width;
}

int Renderer::getHeight() const
{
	return m_height;
}

void Renderer::resize(int width, int height)
{
	m_width = width;
	m_height = height;
}

bool Renderer::setAudioFile(std::string filePath)
{
	//TODO: extend mp3, flac..
	return m_soundTexture->loadAudioFile(filePath);
}

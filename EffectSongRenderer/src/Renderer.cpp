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
	glViewport(0, 0, 640, 480);
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
	m_soundTexture->loadWavFile("C:/Users/pgyag/Desktop/EffectSong/EffectSongRenderer/res/music/agi.wav");
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
float curTime = 0.0f;
void Renderer::update() {
	curTime += 0.02f;
	m_soundTexture->update(curTime);
	for (auto iter = m_scenes.begin(); iter != m_scenes.end(); iter++) {
		(*iter)->update();
	}
}

void Renderer::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (auto iter = m_scenes.begin(); iter != m_scenes.end(); iter++) {
		//skybox
		glDepthMask(GL_FALSE);
		glUseProgram(m_iblShaderProgram);
		Camera* activeCamera = (*iter)->getActiveCamera();
		if (activeCamera)
		{
			glm::mat4 projectionMatrix = glm::perspective(
				glm::radians(activeCamera->fov), 640.0f / 480.0f,
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

		GLint leftSoundTexLoc = glGetUniformLocation(m_shaderProgram, "leftSoundTexture");
		glUniform1i(leftSoundTexLoc, 5);
		GLint rightSoundTexLoc = glGetUniformLocation(m_shaderProgram, "rightSoundTexture");
		glUniform1i(rightSoundTexLoc, 6);

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

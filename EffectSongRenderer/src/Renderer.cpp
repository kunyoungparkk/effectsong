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

float Renderer::getCurrentEnergy() const
{
	if (m_soundTexture) {
		m_soundTexture->getCurrentEnergy(); 
	}
}

Renderer::Renderer() {
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
	const char* cVertexShaderSource = R"(#version 300 es
precision highp float;

uniform mat4 worldMat, viewMat, projMat;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;
layout (location = 3) in vec4 tangent;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragTexcoord;
out mat3 TBN;

void main(){
	mat3 normalMat = transpose(inverse(mat3(worldMat)));

	fragPos = vec3(worldMat * vec4(position, 1.0));
	fragNormal = normalize(normalMat * normal);
	fragTexcoord = texcoord;

	vec3 T = normalize(normalMat * tangent.xyz);
	vec3 N = fragNormal;
	vec3 B = cross(N, T) * tangent.w;
	TBN = mat3(T, B, N);

	gl_Position = projMat * viewMat * worldMat * vec4(position, 1.0);
}
	)";
	const char* cFragmentShaderSource = R"(#version 300 es
precision mediump float;

struct Material {
    vec4 baseColor;
    vec3 specularColor;
    vec3 emissionColor;
    float metallic;
    float roughness;
};

struct DirectionalLight {
    vec3 position;
    float intensity;
    vec3 color;
    vec3 direction;
};
struct PointLight {
    vec3 position;
    float intensity;
    vec3 color;
    float range;
};
struct SpotLight {
    vec3 position;
    float intensity;
    vec3 color;
    float range;
    vec3 direction;
    float innerConeAngle;
    float outerConeAngle;
};

const int MAX_DIRECTIONAL_LIGHTS = 10;
const int MAX_POINT_LIGHTS = 10;
const int MAX_SPOT_LIGHTS = 10;

uniform Material material;

uniform bool useBaseColorTexture;
uniform sampler2D baseColorTexture;

uniform bool useMetallicRoughnessTexture;
uniform sampler2D metallicRoughnessTexture;

uniform bool useEmissiveTexture;
uniform sampler2D emissiveTexture;

uniform bool useNormalTexture;
uniform sampler2D normalTexture;

uniform bool useOcclusionTexture;
uniform sampler2D occlusionTexture;

//Lights
uniform sampler2D lutGGX;
//uniform sampler2D lutCharlie;
//uniform sampler2D lutSheen;

uniform samplerCube specularIBLMap;
uniform vec4 diffuseIBLColor;
uniform float iblIntensity;

uniform DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform int numDirectionalLights;
uniform int numPointLights;
uniform int numSpotLights;

uniform vec3 cameraWorldPos;

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexcoord;
in mat3 TBN;

out vec4 FragColor;

/*IBL*/
vec3 calculateIBLSpecular(vec3 N, vec3 V, float roughness, vec3 F0) {
    vec3 R = reflect(-V, N);
    
    vec3 prefilteredColor = texture(specularIBLMap, R).rgb;

    vec2 envBRDF = texture(lutGGX, vec2(max(dot(N, V), 0.0), roughness)).rg;
    return prefilteredColor * (F0 * envBRDF.x + envBRDF.y);
}
/*BRDF 계산*/
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = 3.141592 * denom * denom;

    return num / denom;
}
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
vec3 calculateBRDF(vec3 N, vec3 V, vec3 L, vec3 albedo, vec3 F0, float metallic, float roughness, float ao) {
    vec3 H = normalize(V + L);
    
    // 각 구성 요소 계산
    float NDF = DistributionGGX(N, H, roughness); // Normal Distribution Function
    float G = GeometrySmith(N, V, L, roughness);  // Geometry function
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0); // Fresnel-Schlick approximation

    // Cook-Torrance BRDF 계산
    vec3 specular = (NDF * G * F) / (4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001);

    // Lambertian Diffuse BRDF
    vec3 kD = vec3(1.0) - F; // Diffuse reflection coefficient
    kD *= 1.0 - metallic;    // 금속성 재질에는 디퓨즈가 없음
    vec3 diffuse = albedo / 3.141592; // Lambertian diffuse
    
    // 디퓨즈와 스펙큘러 합산 후 AO 적용
    return (kD * diffuse + specular) * ao;
}

void main() {
    vec3 result = vec3(0.0, 0.0, 0.0);
    vec3 view = normalize(cameraWorldPos - fragPos);

    vec3 normal;
    if(useNormalTexture)
    {
        normal = normalize(TBN * (2.0 * texture(normalTexture, fragTexcoord).rgb - 1.0));
    }
    else
    {
        normal = fragNormal;
    }

    /*BaseColor*/
    vec3 baseColorTextureColor = vec3(material.baseColor);
    if(useBaseColorTexture){
        baseColorTextureColor *= vec3(texture(baseColorTexture, fragTexcoord));
    }

    /*MetallicRoughness*/
    float metallic = material.metallic;
    float roughness = material.roughness;
    if(useMetallicRoughnessTexture){
        vec2 metallicRoughness = texture(metallicRoughnessTexture, fragTexcoord).gb;
        metallic *= metallicRoughness.g;
        roughness *= metallicRoughness.r;
    }

    /*Ambient Occlusion*/
    float ao = 1.0;
    if(useOcclusionTexture)
    {
        ao = texture(occlusionTexture, fragTexcoord).r;
    }

    /*Emission*/
    vec3 emissiveTextureColor = material.emissionColor;
    if(useEmissiveTexture){
        emissiveTextureColor *= texture(emissiveTexture, fragTexcoord).rgb;
    }

    vec3 F0 = mix(vec3(0.04), baseColorTextureColor, metallic);

    //TODO: diffuseIBL.a 적용?
    vec3 diffuseIBL = diffuseIBLColor.rgb * (baseColorTextureColor / 3.141592);
    vec3 specularIBL = calculateIBLSpecular(normal, view, roughness, F0);
    result = iblIntensity * (diffuseIBL + specularIBL) * ao + emissiveTextureColor;

    //Compute Directional Lights
    for(int i=0; i<numDirectionalLights; i++)
    {
        vec3 lightDirection = normalize(-directionalLights[i].direction);
        
        result += directionalLights[i].color * directionalLights[i].intensity * calculateBRDF(normal, view, lightDirection, baseColorTextureColor, F0, metallic, roughness, ao);
    }

    //Compute Point Lights
    for(int i=0; i<numPointLights; i++)
    {
        vec3 lightDirection = normalize(pointLights[i].position - fragPos);

        float distance = length(pointLights[i].position - fragPos);
        float range = pointLights[i].range;
        float attenuation = clamp((range * range - distance * distance) / (range * range), 0.0, 1.0);
        
        result += pointLights[i].color * pointLights[i].intensity * calculateBRDF(normal, view, lightDirection, baseColorTextureColor, F0, metallic, roughness, ao) * attenuation;
    }

    //Compute Spot Lights
    for(int i=0; i<numSpotLights; i++)
    {
        vec3 lightDirection = normalize(spotLights[i].position - fragPos);
        
        float distance = length(spotLights[i].position - fragPos);
        float range = spotLights[i].range;
        float attenuation = clamp((range * range - distance * distance) / (range * range), 0.0, 1.0);
        float spotEffect = dot(-lightDirection, normalize(spotLights[i].direction));
        float innerCutOff = cos(spotLights[i].innerConeAngle);
        float outerCutOff = cos(spotLights[i].outerConeAngle);
        float spotAttenuation = clamp((spotEffect - outerCutOff) / (innerCutOff - outerCutOff), 0.0, 1.0);
        attenuation *= spotAttenuation;
        
        result += spotLights[i].color * spotLights[i].intensity * calculateBRDF(normal, view, lightDirection, baseColorTextureColor, F0, metallic, roughness, ao) * attenuation;
    }

	FragColor = vec4(result, 1.0);
}
)";
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
	const char* cIBLVertexShaderSource = R"(#version 300 es
precision mediump float;

uniform mat4 viewMat, projMat;

layout (location = 0) in vec3 position;

out vec3 fragPos;

void main(){
	mat4 viewRotation = mat4(mat3(viewMat));
	fragPos = position;
	gl_Position = projMat * viewRotation * vec4(position, 1.0);
}
)";
	const char* cIBLFragmentShaderSource = R"(#version 300 es
precision mediump float;

uniform samplerCube backgroundCubeMap;

in vec3 fragPos;
out vec4 FragColor;

void main(){
	vec3 viewDirection = normalize(fragPos);
	FragColor = texture(backgroundCubeMap, viewDirection);
}
)";
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
	std::string ggxPath = EFFECTSONG_ROOT + std::string("res/IBL/lut_ggx.png");
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
		delete* iter;
	}
	for (auto iter = m_materials.begin(); iter != m_materials.end(); iter++) {
		delete iter->second;
	}
	for (auto iter = m_textures.begin(); iter != m_textures.end(); iter++) {
		delete iter->second;
	}

	if (m_skybox) {
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

	for (auto iter = m_scenes.begin(); iter != m_scenes.end(); iter++) {
		(*iter)->update();
	}
}

void Renderer::render() {
	/*vertex shader art*/
	glBindFramebuffer(GL_FRAMEBUFFER, m_specularIBLFrameBuffer);
	for (int i = 0; i < 6; i++) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_specularIBLTexture->getId(), 0);
	}
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
						GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 };
	glDrawBuffers(6, drawBuffers);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_specularIBLTexture->getDepthTextureId());
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer is not complete!" << std::endl;
	}
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
	ArtShader::getInstance()->render();

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

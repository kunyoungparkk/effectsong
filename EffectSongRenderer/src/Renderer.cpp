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

std::string generateTextureKey(const cgltf_texture* texture, std::string& gltfPath) {
	const cgltf_image* image = texture->image;

	//base64 embedded gltf
	if (image->uri && strncmp(image->uri, "data:", 5) == 0) {
		std::string base64Data = image->uri + 5; // after "data:"
		size_t commaPos = base64Data.find(',');
		if (commaPos != std::string::npos) {
			std::string encodedData = base64Data.substr(commaPos + 1);
			// generate hash
			std::hash<std::string> hasher;
			size_t hashValue = hasher(encodedData);

			return "embedded_" + std::to_string(hashValue);
		}
		return "embedded_invalid";
	}
	//general gltf
	else if (image->uri) {
		return gltfPath + std::string(image->uri);
	}
	//glb
	else if (image->buffer_view) {
		const cgltf_buffer_view* view = image->buffer_view;
		return "buffer_" + std::to_string(view->buffer - &image->buffer_view->buffer[0]) +
			"_" + std::to_string(view->offset) +
			"_" + std::to_string(view->size);
	}

	return "unknown_texture";
}

//TODO: extend it
std::string generateMaterialKey(const cgltf_material* material) {
	size_t hash = 0;
	std::hash<float> floatHasher;
	std::hash<void*> pointerHasher;
	std::hash<std::string> stringHasher;

	if (material->name) {
		hash ^= stringHasher(material->name);
	}
	for (int i = 0; i < 4; ++i) {
		hash ^= floatHasher(material->pbr_metallic_roughness.base_color_factor[i]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	}
	hash ^= floatHasher(material->pbr_metallic_roughness.metallic_factor) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	hash ^= floatHasher(material->pbr_metallic_roughness.roughness_factor) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	if (material->pbr_metallic_roughness.base_color_texture.texture) {
		hash ^= pointerHasher(material->pbr_metallic_roughness.base_color_texture.texture) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	}
	if (material->pbr_metallic_roughness.metallic_roughness_texture.texture) {
		hash ^= pointerHasher(material->pbr_metallic_roughness.metallic_roughness_texture.texture) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	}
	if (material->emissive_texture.texture) {
		hash ^= pointerHasher(material->emissive_texture.texture) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	}
	if (material->normal_texture.texture) {
		hash ^= pointerHasher(material->normal_texture.texture) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	}
	if (material->occlusion_texture.texture) {
		hash ^= pointerHasher(material->occlusion_texture.texture) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	}
	hash ^= std::hash<int>()(material->alpha_mode) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	hash ^= std::hash<bool>()(material->double_sided) + 0x9e3779b9 + (hash << 6) + (hash >> 2);

	hash ^= floatHasher(material->emissive_factor[0]) + floatHasher(material->emissive_factor[1]) + floatHasher(material->emissive_factor[2]);
	hash ^= floatHasher(material->alpha_cutoff);

	return std::to_string(hash);
}

float Renderer::getCurrentEnergy() const
{
	if (m_soundTexture) {
		return m_soundTexture->getCurrentEnergy();
	}
	return 0.0f;
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
	//glCullFace(GL_BACK);
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
    R.x *= -1.;
	R.y *= -1.;
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

	glUseProgram(m_shaderProgram);
	m_specularIBLMapLoc = glGetUniformLocation(m_shaderProgram, "specularIBLMap");
	m_diffuseIBLColorLoc = glGetUniformLocation(m_shaderProgram, "diffuseIBLColor");
	m_lutGGXLoc = glGetUniformLocation(m_shaderProgram, "lutGGX");
	m_iblIntensityLoc = glGetUniformLocation(m_shaderProgram, "iblIntensity");
	m_cameraWorldPosLoc = glGetUniformLocation(m_shaderProgram, "cameraWorldPos");
	m_viewMatLoc = glGetUniformLocation(m_shaderProgram, "viewMat");
	m_projMatLoc = glGetUniformLocation(m_shaderProgram, "projMat");

	glGenFramebuffers(1, &m_specularIBLFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_specularIBLFrameBuffer);
	m_specularIBLTexture = new SpecularIBLTexture();
	m_specularIBLTexture->bind(1);
	for (int i = 0; i < 6; i++) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_specularIBLTexture->getId(), 0);
	}
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_specularIBLTexture->getDepthTextureId());

	glGenFramebuffers(1, &m_diffuseIBLFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_diffuseIBLFrameBuffer);
	m_diffuseIBLTexture = new DiffuseIBLTexture();
	m_diffuseIBLTexture->bind(2);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_diffuseIBLTexture->getId(), 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_diffuseIBLTexture->getDepthTextureId());

	std::string ggxPath = EFFECTSONG_ROOT + std::string("res/IBL/lut_ggx.png");
	m_lut_ggx = new Texture(ggxPath);
	m_lut_ggx->bind(3);

	glUniform1i(m_specularIBLMapLoc, 1);
	glUniform1i(m_lutGGXLoc, 3);

	m_soundTexture = new SoundTexture();

	//ibl shader program
	const char* cIBLVertexShaderSource = R"(#version 300 es
precision mediump float;

uniform mat4 viewProjMat;

layout (location = 0) in vec3 position;

out vec3 fragPos;

void main(){
	fragPos = position;
	gl_Position = viewProjMat * vec4(position, 1.0);
}
)";
	const char* cIBLFragmentShaderSource = R"(#version 300 es
precision mediump float;

uniform samplerCube backgroundCubeMap;

in vec3 fragPos;
out vec4 FragColor;

void main(){
	vec3 viewDirection = normalize(fragPos);
	viewDirection.x *= -1.0;
	viewDirection.y *= -1.0;
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


	//skybox
	glUseProgram(m_iblShaderProgram);
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
	m_skybox->bind(m_iblShaderProgram);
	m_skyboxViewProjMatLoc = glGetUniformLocation(m_iblShaderProgram, "viewProjMat");

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
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
						GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 };
	glDrawBuffers(6, drawBuffers);

	//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
	//	std::cerr << "Framebuffer is not complete!" << std::endl;
	//}
	glDisable(GL_CULL_FACE);
	glViewport(0, 0, 2048, 2048);
	glClearColor(m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, m_backgroundColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLuint artProgram = ArtShader::getInstance()->getProgram();
	glUseProgram(artProgram);
	glDepthMask(GL_TRUE);
	if (m_artProgram != artProgram) {
		m_artProgram = artProgram;
		m_volumeUniformLoc = glGetUniformLocation(artProgram, "volume");
		m_resolutionUniformLoc = glGetUniformLocation(artProgram, "resolution");
		m_backgroundUniformLoc = glGetUniformLocation(artProgram, "background");
		m_timeUniformLoc = glGetUniformLocation(artProgram, "time");
		m_vertexCountUniformLoc = glGetUniformLocation(artProgram, "vertexCount");
		m_soundUniformLoc = glGetUniformLocation(artProgram, "sound");
		m_soundUniformLoc2 = glGetUniformLocation(artProgram, "sound2");
		m_isStereoUniformLoc = glGetUniformLocation(artProgram, "isStereo");
	}
	float resolution[2] = { 2048.0f, 2048.0f };
	glUniform1f(m_volumeUniformLoc, m_soundTexture->getCurrentEnergy());
	glUniform2fv(m_resolutionUniformLoc, 1, resolution);
	glUniform4fv(m_backgroundUniformLoc, 1, glm::value_ptr(m_backgroundColor));
	glUniform1f(m_timeUniformLoc, m_currentTime);
	glUniform1f(m_vertexCountUniformLoc, (float)ArtShader::getInstance()->getVertexCount());
	glUniform1i(m_soundUniformLoc, 4);
	glUniform1i(m_soundUniformLoc2, 5);
	if (m_soundTexture->getChannelCount() == 2) {
		glUniform1i(m_isStereoUniformLoc, true);
	}
	else {
		glUniform1i(m_isStereoUniformLoc, false);
	}
	ArtShader::getInstance()->render();

	/*diffuse texture->IBL_DIFFUSE_LENGTH* IBL_DIFFUSE_LENGTH*/
	glBindFramebuffer(GL_FRAMEBUFFER, m_diffuseIBLFrameBuffer);
	//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
	//	std::cerr << "Framebuffer is not complete!" << std::endl;
	//}
	glViewport(0, 0, IBL_DIFFUSE_LENGTH, IBL_DIFFUSE_LENGTH);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ArtShader::getInstance()->render();

	m_diffuseIBLTexture->bind(2);
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
		glm::mat4 projectionMatrix = glm::mat4(1.0);

		//user needs to set fov even though it is orthographic camera.
		projectionMatrix = glm::perspective(
			glm::radians(m_activeCamera->fov), (float)m_width / m_height,
			0.1f, 10.0f);
		glm::mat4 viewMatrix = m_activeCamera->getViewMatrix();

		glm::mat4 viewProjMatrix = projectionMatrix * glm::mat4(glm::mat3(viewMatrix));
		glUniformMatrix4fv(m_skyboxViewProjMatLoc, 1, GL_FALSE, glm::value_ptr(viewProjMatrix));

		m_skybox->render();
	}

	glDepthMask(GL_TRUE);
	glUseProgram(m_shaderProgram);

	m_specularIBLTexture->bind(1);
	glUniform4fv(m_diffuseIBLColorLoc, 1, glm::value_ptr(diffuseIBLColor));
	glUniform1f(m_iblIntensityLoc, 1.0f);
	glm::mat4 projectionMatrix = glm::mat4(1.0f);
	glm::mat4 viewMatrix = glm::mat4(1.0f);
	if (m_activeCamera) {
		if (m_activeCamera->projectionType == ProjectionType::PERSPECTIVE) {
			projectionMatrix = glm::perspective(
				glm::radians(m_activeCamera->fov),
				(float)Renderer::getInstance()->getWidth() / Renderer::getInstance()->getHeight(),
				m_activeCamera->zNear, m_activeCamera->zFar);
		}
		else if (m_activeCamera->projectionType == ProjectionType::ORTHOGRAPHIC) {
			float aspectRatio = (float)Renderer::getInstance()->getWidth() / Renderer::getInstance()->getHeight();
			float left, right, bottom, top;

			if (aspectRatio > 1.0f) {
				left = -m_activeCamera->xMag * aspectRatio;
				right = m_activeCamera->xMag * aspectRatio;
				bottom = -m_activeCamera->yMag;
				top = m_activeCamera->yMag;
			}
			else {
				left = -m_activeCamera->xMag;
				right = m_activeCamera->xMag;
				bottom = -m_activeCamera->yMag / aspectRatio;
				top = m_activeCamera->yMag / aspectRatio;
			}

			projectionMatrix = glm::ortho(left, right, bottom, top, m_activeCamera->zNear, m_activeCamera->zFar);
		}

		viewMatrix = m_activeCamera->getViewMatrix();
		glUniform3fv(m_cameraWorldPosLoc, 1, glm::value_ptr(m_activeCamera->getNode()->getGlobalPosition()));
	}
	glUniformMatrix4fv(m_viewMatLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(m_projMatLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	for (auto iter = m_scenes.begin(); iter != m_scenes.end(); iter++) {
		(*iter)->render(m_shaderProgram);
	}
}

void Renderer::addScene(Scene* scene) { m_scenes.push_back(scene); }

void Renderer::removeScene(Scene* scene) { 
	m_scenes.remove(scene);
	delete scene;
}

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

Material* Renderer::getMaterial(cgltf_material* cgltfMaterial) { 
	std::string key = generateMaterialKey(cgltfMaterial);
	if (m_materials.find(key) == m_materials.end()) {
		return nullptr;
	}
	
	return m_materials[key];
}

void Renderer::addMaterial(cgltf_material* cgltfMaterial, Material* material) {
	std::string key = generateMaterialKey(cgltfMaterial);
	//if has key -> ignore
	if (m_materials.find(key) != m_materials.end()) {
		return;
	}
	//get key
	m_materials[key] = material;
}

Texture* Renderer::getTexture(cgltf_texture* cgltf_texture, std::string gltfPath, bool isSRGB) {
	std::string key = generateTextureKey(cgltf_texture, gltfPath);
	auto iter = m_textures.find(key);
	if (iter == m_textures.end()) {
		m_textures[key] = new Texture(gltfPath, cgltf_texture, isSRGB);
	}
	
	return m_textures[key];
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
	return m_soundTexture->loadAudioFile(filePath);
}

#version 300 es
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
uniform samplerCube diffuseIBLMap;
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

vec3 calculateIBLDiffuse(vec3 N) {
    return texture(diffuseIBLMap, N).rgb;
}
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
    
    
    vec3 diffuseIBL = calculateIBLDiffuse(normal) * (baseColorTextureColor / 3.141592);
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
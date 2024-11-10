#version 300 es
precision mediump float;

struct Material {
    vec4 baseColor;
    vec3 specularColor;
    vec3 emissionColor;
};
sampler2D baseColorTexture;
sampler2D metallicRoughnessTexture;
sampler2D emissiveTexture;
sampler2D normalTexture;

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

uniform DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform vec3 cameraWorldPos;

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexcoord;
in mat3 TBN;

uniform int numDirectionalLights;
uniform int numPointLights;
uniform int numSpotLights;

out vec4 FragColor;

void main() {
    vec3 lightPower = vec3(0.0, 0.0, 0.0);

    //TODO: normal texture 있다면.. 없으면 fragNormal 사용하기
    vec3 normal = normalize(TBN * (2.0 * texture(normalTexture, fragTexcoord).rgb - 1.0));

    //Compute Directional Lights
    for(int i=0; i<numDirectionalLights; i++)
    {
        vec3 currentLightPower = vec3(0.0, 0.0, 0.0);
        
        vec3 lightDirection = normalize(-directionalLights[i].direction);
        //diffuse
        float diffuseFactor = max(dot(normal, lightDirection), 0.0);
        currentLightPower += directionalLights[i].color * directionalLights[i].intensity * diffuseFactor;

        //specular
        vec3 view = normalize(cameraWorldPos - fragPos);
        vec3 reflection = reflect(-lightDirection, normal);
        float specularFactor = pow(max(dot(view, reflection), 0.0), 30.0);
        currentLightPower += directionalLights[i].color * directionalLights[i].intensity * specularFactor;

        lightPower += currentLightPower;
    }
    
    //Compute Point Lights
    for(int i=0; i<numPointLights; i++)
    {
        vec3 currentLightPower = vec3(0.0, 0.0, 0.0);

        float distance = length(pointLights[i].position - fragPos);
        float range = 0.5;//TODO: pointLights[i].range;, 편집기에서 수정하도록. gltf로 안들어온다.

        float attenuation = clamp((range * range - distance * distance) / (range * range), 0.0, 1.0);

        vec3 lightDirection = normalize(pointLights[i].position - fragPos);
        
        //diffuse
        float diffuseFactor = max(dot(normal, lightDirection), 0.0);
        //specular
        vec3 view = normalize(cameraWorldPos - fragPos);
        vec3 reflection = reflect(-lightDirection, normal);
        float specularFactor = pow(max(dot(view, reflection), 0.0), 30.0);

        currentLightPower += pointLights[i].color * pointLights[i].intensity * (diffuseFactor + specularFactor) * attenuation;
        
        lightPower += currentLightPower;
    }

    //Compute Spot Lights
    for(int i=0; i<numSpotLights; i++)
    {
        vec3 currentLightPower = vec3(0.0, 0.0, 0.0);
        
        float distance = length(spotLights[i].position - fragPos);
        float range = 3.0;//TODO: spotLights[i].range;
        
        float attenuation = clamp((range * range - distance * distance) / (range * range), 0.0, 1.0);
        
        vec3 lightDirection = normalize(spotLights[i].position - fragPos);

        float spotEffect = dot(-lightDirection, normalize(spotLights[i].direction));
        float innerCutOff = cos(spotLights[i].innerConeAngle);
        float outerCutOff = cos(spotLights[i].outerConeAngle);

        float spotAttenuation = clamp((spotEffect - outerCutOff) / (innerCutOff - outerCutOff), 0.0, 1.0);
        
        attenuation *= spotAttenuation;
        
        // Diffuse
        float diffuseFactor = max(dot(normal, lightDirection), 0.0);

        // Specular
        vec3 view = normalize(cameraWorldPos - fragPos);
        vec3 reflection = reflect(-lightDirection, normal);
        float specularFactor = pow(max(dot(view, reflection), 0.0), 30.0);

        currentLightPower += spotLights[i].color * spotLights[i].intensity * (diffuseFactor + specularFactor) * attenuation;

        lightPower += currentLightPower;
    }
    
    vec3 baseColorTextureColor = texture(baseColorTexture, fragTexcoord).rgb;
    vec3 emissiveTextureColor = texture(emissiveTexture, fragTexcoord).rgb;
    
	FragColor = vec4(lightPower * baseColorTextureColor + emissiveTextureColor * material.emissionColor, 1.0);
}
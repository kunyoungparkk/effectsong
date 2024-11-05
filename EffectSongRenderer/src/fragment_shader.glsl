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

vec3 calculateDirectionalLight(DirectionalLight light)
{
    return vec3(0,0,0);
}
vec3 calculatePointLight(PointLight light)
{
    return vec3(0,0,0);
}
vec3 calculateSpotLight(SpotLight light)
{
    return vec3(0,0,0);
}

void main() {
    vec3 lightPower = vec3(0.0, 0.0, 0.0);
    //TODO: normal texture 있다면.. 없으면 fragNormal 사용하기
    
    vec3 normal = normalize(TBN * (2.0 * texture(normalTexture, fragTexcoord).rgb - 1.0));

    for(int i=0; i<numDirectionalLights; i++)
    {
        vec3 currentLightPower = vec3(0.0, 0.0, 0.0);
        
        vec3 lightDirection = normalize(-directionalLights[i].direction);
        //ambient

        //diffuse
        float diffuseFactor = max(dot(normal, lightDirection), 0.0);
        currentLightPower += vec3(0.5, 0.5, 0.5) * diffuseFactor;

        //specular
        vec3 view = normalize(cameraWorldPos - fragPos);
        vec3 reflection = reflect(lightDirection, normal);
        float specularFactor = pow(max(dot(view, reflection), 0.0), 50.0);
        currentLightPower += vec3(1.0, 1.0, 1.0) * specularFactor;

        lightPower += currentLightPower;
    }
    
    vec3 baseColorTextureColor = texture(baseColorTexture, fragTexcoord).rgb;
    vec3 emissiveTextureColor = texture(emissiveTexture, fragTexcoord).rgb;
    
	FragColor = vec4(lightPower * baseColorTextureColor + emissiveTextureColor * material.emissionColor, 1.0);
}
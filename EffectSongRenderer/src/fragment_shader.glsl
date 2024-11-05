#version 300 es
precision mediump float;

struct Material {
    vec4 baseColor;
    vec3 specularColor;
    vec3 emissionColor;
};
sampler2D baseColorTexture;
sampler2D emissiveTexture;

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
    vec3 texColor = texture(baseColorTexture, fragTexcoord).rgb;
    
    vec3 lightPower = vec3(0.0, 0.0, 0.0);
    
    for(int i=0; i<numDirectionalLights; i++)
    {
        vec3 currentLightPower = vec3(0.0, 0.0, 0.0);
        //ambient

        //diffuse
        
        //specular
        vec3 view = normalize(cameraWorldPos - fragPos);
        vec3 reflection = reflect(-normalize(directionalLights[i].direction), normalize(fragNormal));
        float specularFactor = pow(max(dot(view, reflection), 0.0), 30.0);
        currentLightPower += vec3(1.0, 1.0, 1.0) * specularFactor;

        //emission
        //currentLightPower += material.emissionColor;

        lightPower += currentLightPower;
    }

	FragColor = vec4(lightPower * texColor, 1.0);
}
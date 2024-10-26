#version 300 es
precision mediump float;

struct Material {
    float ambient;
    float diffuse;
    float specular;
    float emission;
    sampler2D diffuseTexture;
};

struct DirectionalLight {
    vec3 position;
    float intensity;
    vec3 color;
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
    float innerConeAngle;
    float outerConeAngle;
};

const int MAX_DIRECTIONAL_LIGHTS = 10;
const int MAX_POINT_LIGHTS = 10;
const int MAX_SPOT_LIGHTS = 10;

uniform Material material;
uniform DirectionalLight[MAX_DIRECTIONAL_LIGHTS];
uniform PointLight[MAX_POINT_LIGHTS];
uniform SpotLight[MAX_SPOT_LIGHTS];
uniform vec3 cameraPos;

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
    vec3 texColor = texture(material.diffuseTexture, fragTexcoord).rgb;

    vec3 lightPower = vec3(0,0,0);
    


	FragColor = vec4(texColor, 1.0);
    //FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
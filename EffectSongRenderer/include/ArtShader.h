#pragma once
#include "common.h"

class ArtShader
{
public:
	~ArtShader();
	static ArtShader* getInstance()
	{
		if (instance_ == nullptr)
		{
			instance_ = new ArtShader();
		}
		return instance_;
	}
	static void deleteInstance() {
		delete instance_;
		instance_ = nullptr;
	}
	std::string getFragmentShader(){ return fs_; }
	std::string getVertexShaderHeader(){ return vsHeader_; }
	std::string getVertexShader(){ return vsHeader_ + vs_; }
	std::string getUserVertexShader(){ return vs_; }
	bool setVertexShader(std::string vs){ 
		if (compileShader(vs)) {
			vs_ = vs;
			return true;
		}
		return false;
	}

	bool compileShader(std::string tempVS);

	void render() const;

	GLenum getPrimitiveMode() const { return m_primitiveMode; }
	void setPrimitiveMode(GLenum primitiveMode) { m_primitiveMode = primitiveMode;  }

	int getVertexCount() const { return m_vertexCount; }
	void setVertexCount(int vertexCount) {
		m_vertices.resize(vertexCount);
		for (int i = m_vertexCount; i < vertexCount; i++) {
			m_vertices[i] = (float)i;
		}

		m_vertexCount = vertexCount;

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), GL_STATIC_DRAW);
	}

	GLuint getProgram() { return m_artShaderProgram; }

	std::string getLastCompileError() { return m_lastVSCompileError; }
private:
	ArtShader();
	static ArtShader* instance_;
	GLuint VAO, VBO;
	GLenum m_primitiveMode = GL_POINTS;
	std::vector<float> m_vertices;
	int m_vertexCount = 30000;
	GLuint m_artShaderProgram;

	std::string m_lastVSCompileError = "";

	std::string fs_ = R"(#version 300 es
	precision highp float;
	in vec4 v_color;
	layout(location = 0) out vec4 FragColor1;
	layout(location = 1) out vec4 FragColor2;
	layout(location = 2) out vec4 FragColor3;
	layout(location = 3) out vec4 FragColor4;
	layout(location = 4) out vec4 FragColor5;
	layout(location = 5) out vec4 FragColor6;
	void main() {
		FragColor1 = v_color;
		FragColor2 = v_color;
		FragColor3 = v_color;
		FragColor4 = v_color;
		FragColor5 = v_color;
		FragColor6 = v_color;
	}
	)";
	std::string vsHeader_ = R"(#version 300 es
	precision highp float;
	layout (location = 0) in float vertexId;
	uniform float volume;
	uniform vec2 resolution;
	uniform vec4 background;
	uniform float time;
	uniform float vertexCount;
	uniform sampler2D sound;
	uniform sampler2D sound2;
	uniform bool isStereo;
	out vec4 v_color;
	//uniform vec2 touch;
	//uniform vec2 mouse;
	//uniform sampler2D volume;
	//uniform sampler2D floatSound;
	//uniform vec2 soundRes;
	//uniform float _dontUseDirectly_pointSize; : multiflydevicePixelRatio
	)";
	std::string vs_ = R"(//shader art sample
#define PI 3.14159
#define NUM_SEGMENTS 51.0
#define NUM_POINTS (NUM_SEGMENTS * 2.0)
#define STEP 5.0

vec3 hsv2rgb(vec3 c) {
    c = vec3(c.x, clamp(c.yz, 0.0, 1.0));
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    float num = mod(vertexId, 2.0);
    float point = mod(floor(vertexId / 2.0) + mod(vertexId, 2.0) * STEP, NUM_SEGMENTS);
    float count = floor(vertexId / NUM_POINTS);
    float offset = count * 0.02;
    float angle = point * PI * 2.0 / NUM_SEGMENTS - offset;
    float spread = 0.02;
    float off = 0.01;
    float speed = count * 0.004;
    float snd = 1.0;

    if (isStereo && num > 0.9) {
        snd *= (
            texture(sound2, vec2(off + spread * 0., speed)).r +
            texture(sound2, vec2(off + spread * 1., speed)).r +
            texture(sound2, vec2(off + spread * 2., speed)).r) / 3.;
    } else {
        snd *= (
            texture(sound, vec2(off + spread * 0., speed)).r +
            texture(sound, vec2(off + spread * 1., speed)).r +
            texture(sound, vec2(off + spread * 2., speed)).r) / 3.;
    }
    
    float rPulse = pow(snd, 5.0);
    float radius = count * 0.02 + rPulse * 0.4; 

    float rotationSpeed = num < 0.9 ? 1.0 : -1.0; 
    float c = cos(angle + time * rotationSpeed) * radius;
    float s = sin(angle + time * rotationSpeed) * radius;

    vec2 aspect = vec2(1, resolution.x / resolution.y);
    vec2 xy = vec2(c, s);

    gl_Position = vec4(xy * aspect, 0, 1);
    gl_Position.x = num < 0.9 
        ? gl_Position.x * 0.5 - 0.4
        : gl_Position.x * 0.5 + 0.4; 
    
    gl_PointSize = 2.0 + length(xy) * 20. * resolution.x / 1600.0;

    float hue = time * 0.03 + count * 1.001 + (num < 0.9 ? 0.0 : 0.5);
    float cPulse = pow(rPulse, 2.0);
    float invCPulse = 1.0 - cPulse;
    vec4 color = vec4(hsv2rgb(vec3(hue, invCPulse, 1.0)), 1.0);
    v_color = mix(color, background, radius - cPulse);
}
)";
};
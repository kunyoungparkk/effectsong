#pragma once
#include "common.h"

class ArtShader
{
public:
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
	void setVertexShader(std::string vs){ 
		vs_ = vs;
		compileShader();
	}

	void compileShader();

	void render();

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
private:
	ArtShader();
	~ArtShader();
	static ArtShader* instance_;
	GLuint VAO, VBO;
	GLenum m_primitiveMode = GL_POINTS;
	std::vector<float> m_vertices;
	int m_vertexCount = 40000;
	GLuint m_artShaderProgram;

	std::string fs_ = R"(#version 300 es
	precision highp float;
	in vec4 v_color;
	out vec4 FragColor;
	void main() {
		FragColor = v_color;
	}
	)";
	std::string vsHeader_ = R"(#version 300 es
	precision highp float;
	layout (location = 0) in float vertexId;
	uniform vec2 touch; // 누적해서 텍스처로 제공?
	uniform vec2 resolution;
	uniform vec4 background;
	uniform float time;
	uniform float vertexCount;
	uniform sampler2D sound;
	uniform sampler2D sound2;
	uniform bool isStereo;
	out vec4 v_color;
	//uniform vec2 mouse;
	//uniform sampler2D volume;
	//uniform sampler2D floatSound;
	//uniform vec2 soundRes;
	//uniform float _dontUseDirectly_pointSize; : multiflydevicePixelRatio
	)";
	std::string vs_ = R"(
#define PI radians(180.0)

vec3 hsv2rgb(vec3 c) {
  c = vec3(c.x, clamp(c.yz, 0.0, 1.0));
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
void main() {
  float across = floor(sqrt(vertexCount));
  float down = floor(vertexCount / across);
  
  float x = mod(vertexId, across);
  float y = floor(vertexId / across);
  
  float u = x / across;
  float v = y / across;
  
  vec2 xy = vec2(u * 2.0 - 1.0, v * 2.0 - 1.0);
  gl_Position = vec4(xy, 0, 1);
  gl_PointSize = max(0.1, ceil(resolution.x / across));
  
  float s;
  if(!isStereo || u<0.5){
    s = texture(sound, vec2(u * 2.0, v)).r;
  }else{
    s = texture(sound2, vec2((u - 0.5) * 2.0, v)).r;
  }  
  v_color = vec4(s, s, s, 1.0);
}
	)";
};
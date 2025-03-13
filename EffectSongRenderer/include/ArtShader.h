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
	int m_vertexCount = 10000;
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
	std::string vs_ = R"(
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
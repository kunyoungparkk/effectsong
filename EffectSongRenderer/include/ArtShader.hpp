#pragma once
#include <string>
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
	std::string getFragmentShader()
	{
		return fs_;
	}
	std::string getVertexShaderHeader()
	{
		return vsHeader_;
	}
	std::string getVertexShader()
	{
		return vs_;
	}
	void setVertexShader(std::string vs)
	{
		vs_ = vs;
	}
private:
	static ArtShader* instance_;
	std::string fs_ = R"(precision mediump float;
	in vec4 v_color;
	out vec4 FragColor;
	void main() {
		FragColor = v_color;
	}
	)";
	std::string vsHeader_ = R"(attribute float vertexId;
	uniform vec2 mouse;
	uniform vec2 resolution;
	uniform vec4 background;
	uniform float time;
	uniform float vertexCount;
	uniform sampler2D volume;
	uniform sampler2D sound;
	uniform sampler2D floatSound;
	uniform sampler2D touch;
	uniform vec2 soundRes;
	uniform float _dontUseDirectly_pointSize;
	out vec4 v_color;
	)";
	std::string vs_ = R"()";
};
ArtShader* ArtShader::instance_ = nullptr;
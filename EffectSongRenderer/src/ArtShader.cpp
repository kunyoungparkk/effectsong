#include "ArtShader.h"

ArtShader* ArtShader::instance_ = nullptr;
ArtShader::ArtShader() {
	m_artShaderProgram = glCreateProgram();
	//shader art program
	std::string vertexShader = getVertexShader();
	const char* cArtVertexShaderSource = vertexShader.c_str();
	const char* cArtFragmentShaderSource = fs_.c_str();

	GLuint artVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(artVertexShader, 1, &cArtVertexShaderSource, NULL);
	glCompileShader(artVertexShader);
	GLuint artFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(artFragmentShader, 1, &cArtFragmentShaderSource, NULL);
	glCompileShader(artFragmentShader);
	glAttachShader(m_artShaderProgram, artVertexShader);
	glAttachShader(m_artShaderProgram, artFragmentShader);
	glLinkProgram(m_artShaderProgram);
	glDeleteShader(artVertexShader);
	glDeleteShader(artFragmentShader);

	m_vertices.resize(m_vertexCount);
	for (int i = 0; i < m_vertexCount; i++) {
		m_vertices[i] = (float)i;
	}

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(float), 0);

	//����
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
ArtShader::~ArtShader()
{
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}
bool ArtShader::compileShader(std::string tempVS)
{
	GLuint tempProgram = glCreateProgram();
	//shader art program
	std::string vertexShader = vsHeader_ + tempVS;//getVertexShader();
	const char* cArtVertexShaderSource = vertexShader.c_str();
	const char* cArtFragmentShaderSource = fs_.c_str();

	GLuint artVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(artVertexShader, 1, &cArtVertexShaderSource, NULL);
	glCompileShader(artVertexShader);
	GLint success;
	glGetShaderiv(artVertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[1024];
		glGetShaderInfoLog(artVertexShader, 1024, NULL, infoLog);
		m_lastVSCompileError = infoLog;
		
		glDeleteShader(artVertexShader);		
		return false;
	}
	GLuint artFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(artFragmentShader, 1, &cArtFragmentShaderSource, NULL);
	glCompileShader(artFragmentShader);
	glAttachShader(tempProgram, artVertexShader);
	glAttachShader(tempProgram, artFragmentShader);
	glLinkProgram(tempProgram);
	glDeleteShader(artVertexShader);
	glDeleteShader(artFragmentShader);

	glDeleteProgram(m_artShaderProgram);
	m_artShaderProgram = tempProgram;
	return true;
}
void ArtShader::render() const
{
	glBindVertexArray(VAO);
	glDrawArrays(m_primitiveMode, 0, m_vertexCount);
	glBindVertexArray(0);
}
#include "IBLPrimitive.h"
#include "Renderer.h"

IBLPrimitive::IBLPrimitive(std::vector<glm::vec3>& vertices, std::vector<GLuint>& indices)
{
	m_vertices = vertices;
	m_indices = indices;
	//bind
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// VBO, EBO : VAO에 종속적
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3),
		m_vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint),
		m_indices.data(), GL_STATIC_DRAW);

	// attribute 설정 : VAO에 종속적 - > VAO에 정점 속성 정보 저장
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
		(const void*)0);

	//바인딩 해제
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void IBLPrimitive::render() {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
}

void IBLPrimitive::bind(GLuint shaderProgram)
{
	GLint m_specularIBLTexLoc = glGetUniformLocation(shaderProgram, "backgroundCubeMap");
	glUniform1i(m_specularIBLTexLoc, 1);
}

#include "Primitive.h"
#include "Renderer.h"

Primitive::Primitive(cgltf_primitive* cgltfPrimitive) {
	std::vector<float> vertices;
	std::vector<float> texcoords;
	std::vector<float> normals;
	std::vector<float> tangents;
	for (size_t i = 0; i < cgltfPrimitive->attributes_count; ++i) {
		cgltf_accessor* accessor = cgltfPrimitive->attributes[i].data;

		switch (cgltfPrimitive->attributes[i].type) {
		case cgltf_attribute_type_position: {
			for (size_t j = 0; j < accessor->count; ++j) {
				float vertex[3];
				cgltf_accessor_read_float(accessor, j, vertex, 3);
				vertices.push_back(vertex[0]);
				vertices.push_back(vertex[1]);
				vertices.push_back(vertex[2]);
			}
			break;
		}
		case cgltf_attribute_type_normal: {
			for (size_t j = 0; j < accessor->count; ++j) {
				float normal[3];
				cgltf_accessor_read_float(accessor, j, normal, 3);
				normals.push_back(normal[0]);
				normals.push_back(normal[1]);
				normals.push_back(normal[2]);
			}
			break;
		}
		case cgltf_attribute_type_texcoord: {
			for (size_t j = 0; j < accessor->count; ++j) {
				float texcoord[2];
				cgltf_accessor_read_float(accessor, j, texcoord, 2);
				texcoords.push_back(texcoord[0]);
				texcoords.push_back(texcoord[1]);
			}
			break;
		}
		case cgltf_attribute_type_tangent: {
			for (size_t j = 0; j < accessor->count; ++j) {
				float tangent[4];
				cgltf_accessor_read_float(accessor, j, tangent, 4);
				tangents.push_back(tangent[0]);
				tangents.push_back(tangent[1]);
				tangents.push_back(tangent[2]);
				tangents.push_back(tangent[3]);
			}
			break;
		}
		}

		if (cgltfPrimitive->indices != nullptr) {
			cgltf_accessor* accessor = cgltfPrimitive->indices;
			for (size_t i = 0; i < accessor->count; ++i) {
				unsigned int index;
				cgltf_accessor_read_uint(accessor, i, &index, 1);
				m_indices.push_back(index);
			}
		}

	}
	for (size_t i = 0; i < vertices.size() / 3; i++) {
		Vertex vertex;
		vertex.position =
			glm::vec3(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]);

		if (!normals.empty()) {
			vertex.normal =
				glm::vec3(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]);
		}

		if (!texcoords.empty()) {
			vertex.texcoord = glm::vec2(texcoords[i * 2], texcoords[i * 2 + 1]);
		}

		if (!tangents.empty()) {
			vertex.tangent = glm::vec4(tangents[i * 4], tangents[i * 4 + 1], tangents[i * 4 + 2], tangents[i * 4 + 3]);
		}

		m_vertices.push_back(vertex);
	}
	switch (cgltfPrimitive->type) {
	case cgltf_primitive_type_points:
		m_primitiveMode = GL_POINTS;
		break;
	case cgltf_primitive_type_lines:
		m_primitiveMode = GL_LINES;
		break;
	case cgltf_primitive_type_line_loop:
		m_primitiveMode = GL_LINE_LOOP;
		break;
	case cgltf_primitive_type_line_strip:
		m_primitiveMode = GL_LINE_STRIP;
		break;
	case cgltf_primitive_type_triangles:
		m_primitiveMode = GL_TRIANGLES;
		break;
	case cgltf_primitive_type_triangle_strip:
		m_primitiveMode = GL_TRIANGLE_STRIP;
		break;
	case cgltf_primitive_type_triangle_fan:
		m_primitiveMode = GL_TRIANGLE_FAN;
		break;
	case cgltf_primitive_type_invalid:
	case cgltf_primitive_type_max_enum:
	default:
		exit(1);
		break;
	}

	//material 연결
	m_material = Renderer::getInstance()->getMaterial(cgltfPrimitive->material->name);

	//bind
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// VBO, EBO : VAO에 종속적
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex),
		m_vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint),
		m_indices.data(), GL_STATIC_DRAW);

	// attribute 설정 : VAO에 종속적 - > VAO에 정점 속성 정보 저장
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(const void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(const void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(const void*)offsetof(Vertex, texcoord));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(const void*)offsetof(Vertex, tangent));

	//바인딩 해제
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Primitive::render(GLuint shaderProgram) {
	glBindVertexArray(VAO);

	if (m_material) {
		m_material->bind(shaderProgram);
	}

	// Draw
	glDrawElements(m_primitiveMode, m_indices.size(), GL_UNSIGNED_INT, 0);
}

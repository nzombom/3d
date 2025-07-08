#include <vector>
#include <GL/glew.h>

#include "mesh.hpp"
#include "math.hpp"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
	vs = vertices;
	idxs = indices;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vs.size() * sizeof(Vertex),
			vs.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxs.size() * sizeof(unsigned int),
			idxs.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false,
			sizeof(Vertex), (void*)offsetof(Vertex, p));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, false,
			sizeof(Vertex), (void*)offsetof(Vertex, n));

	glBindVertexArray(0);
}
Mesh::~Mesh() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Mesh::draw() {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, idxs.size(), GL_UNSIGNED_INT, 0);
}

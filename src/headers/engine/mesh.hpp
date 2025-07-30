#ifndef MESH_H
#define MESH_H

#include <vector>

#include "math.hpp"

struct Vertex {
	vector p;
	vector n;
};

class Mesh {
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

  public:
	std::vector<Vertex> vs;
	std::vector<unsigned int> idxs;

	Mesh(std::vector<Vertex>, std::vector<unsigned int>);
	~Mesh();

	void updateVAO();

	void draw();
};

#endif

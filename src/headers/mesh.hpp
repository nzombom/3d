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

	void draw();
};

struct Transform {
	vector p;
	quat r;
	vector s;
};
inline vector IDP = { 0, 0, 0 };
inline quat IDR = { 1, { 0, 0, 0 } };
inline vector IDS = { 1, 1, 1 };

#endif

#include <iostream>
#include <vector>

#include "meshUtils.hpp"
#include "math.hpp"
#include "mesh.hpp"

Mesh generateCube() {
	std::vector<Vertex> vs = {
		{ { -1, -1, -1 }, { -1, 0, 0 } },
		{ { -1, -1, -1 }, { 0, -1, 0 } },
		{ { -1, -1, -1 }, { 0, 0, -1 } },
		{ { 1, -1, -1 }, { 1, 0, 0 } },
		{ { 1, -1, -1 }, { 0, -1, 0 } },
		{ { 1, -1, -1 }, { 0, 0, -1 } },
		{ { -1, 1, -1 }, { -1, 0, 0 } },
		{ { -1, 1, -1 }, { 0, 1, 0 } },
		{ { -1, 1, -1 }, { 0, 0, -1 } },
		{ { 1, 1, -1 }, { 1, 0, 0 } },
		{ { 1, 1, -1 }, { 0, 1, 0 } },
		{ { 1, 1, -1 }, { 0, 0, -1 } },
		{ { -1, -1, 1 }, { -1, 0, 0 } },
		{ { -1, -1, 1 }, { 0, -1, 0 } },
		{ { -1, -1, 1 }, { 0, 0, 1 } },
		{ { 1, -1, 1 }, { 1, 0, 0 } },
		{ { 1, -1, 1 }, { 0, -1, 0 } },
		{ { 1, -1, 1 }, { 0, 0, 1 } },
		{ { -1, 1, 1 }, { -1, 0, 0 } },
		{ { -1, 1, 1 }, { 0, 1, 0 } },
		{ { -1, 1, 1 }, { 0, 0, 1 } },
		{ { 1, 1, 1 }, { 1, 0, 0 } },
		{ { 1, 1, 1 }, { 0, 1, 0 } },
		{ { 1, 1, 1 }, { 0, 0, 1 } },
	};
	std::vector<unsigned int> idxs = {
		0, 12, 18, 0, 18, 6,
		3, 9, 21, 3, 21, 15,
		1, 4, 16, 1, 16, 13,
		7, 19, 22, 7, 22, 10,
		2, 8, 11, 2, 11, 5,
		14, 17, 23, 14, 23, 20,
	};
	return Mesh(vs, idxs);
}

Mesh subdivideMesh(Mesh m, unsigned int n) {
	std::vector<Vertex> vs;
	std::vector<unsigned int> idxs;
	for (int f = 0; f < 12; f++) {
		vector p0 = m.vs[m.idxs[3 * f]].p;
		vector p1 = m.vs[m.idxs[3 * f + 1]].p;
		vector p2 = m.vs[m.idxs[3 * f + 2]].p;
		vector n0 = m.vs[m.idxs[3 * f]].n;
		vector n1 = m.vs[m.idxs[3 * f + 1]].n;
		vector n2 = m.vs[m.idxs[3 * f + 2]].n;
		for (unsigned int i = 0; i < n + 1; i++) {
			for (unsigned int j = 0; j < i + 1; j++) {
				float li = (float)i / (float)n;
				float lj = (float)j / (float)n;
				vector vp = p0 + (p1 - p0) * li + (p2 - p1) * lj;
				vector vn = n0 + (n1 - n0) * li + (n2 - n1) * lj;
				unsigned int iv = vs.size();
				vs.push_back({ vp, vn });
				if (i > 0 && j > 0) {
					idxs.push_back(iv - i - 1);
					idxs.push_back(iv - 1);
					idxs.push_back(iv);
				}
				if (i < n && j < i) {
					idxs.push_back(iv);
					idxs.push_back(iv + i + 2);
					idxs.push_back(iv + 1);
				}
			}
		}
	}
	return Mesh(vs, idxs);
}

Mesh generateSphere(unsigned int n) {
	std::vector<Vertex> vs;
	Mesh m = subdivideMesh(generateCube(), n);
	for (unsigned int i = 0; i < m.vs.size(); i++) {
		vector vp = m.vs.at(i).p;
		vs.push_back({ vp.normalize(), vp.normalize() });
	}
	return { vs, m.idxs };
}

Mesh simpleQuad() {
	std::vector<Vertex> vs = {
		{ { -1, -1, 0 }, { 0, 0, 1 } },
		{ { 1, -1, 0 }, { 0, 0, 1 } },
		{ { 1, 1, 0 }, { 0, 0, 1 } },
		{ { -1, 1, 0 }, { 0, 0, 1 } },
	};
	std::vector<unsigned int> idxs = { 0, 1, 2, 0, 2, 3 };
	return { vs, idxs };
}

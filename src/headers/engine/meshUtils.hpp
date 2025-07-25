#ifndef MESHUTILS_H
#define MESHUTILS_H

#include "mesh.hpp"

Mesh generateCube();

Mesh subdivideMesh(Mesh m, unsigned int n);

Mesh generateSphere(unsigned int n);

Mesh simpleQuad();

#endif

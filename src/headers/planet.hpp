#ifndef PLANET_H
#define PLANET_H

#include <string>
#include <vector>

#include "engine/mesh.hpp"
#include "celestial.hpp"

class Planet : public Celestial {
	std::string name;
	vector pos;
	vector vel;
	float mass;
	float surfaceGravity;
	float surfaceRadius;
	Mesh  &mesh;

  public:
	Planet(std::string, vector, vector, float, float, Mesh &);

	virtual std::string getName();
	virtual float getMass();
	virtual vector getPos();

	virtual void updateVel(std::vector<Celestial *>, unsigned int);
	virtual void updatePos(std::vector<Celestial *>, unsigned int);

	virtual void draw(Shader &s);
};

#endif

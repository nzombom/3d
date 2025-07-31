#include <string>
#include <vector>

#include "engine/math.hpp"
#include "engine/shader.hpp"
#include "engine/mesh.hpp"
#include "planet.hpp"

Planet::Planet(std::string _name, vector _pos, vector _vel,
	float _surfaceGravity, float _surfaceRadius, Mesh &_mesh)
	: name(_name), pos(_pos), vel(_vel), surfaceGravity(_surfaceGravity),
	  surfaceRadius(_surfaceRadius), mesh(_mesh) {
	mass = surfaceGravity * surfaceRadius * surfaceRadius / G;
}

std::string Planet::getName() {
	return name;
}
float Planet::getMass() {
	return mass;
}
vector Planet::getPos() {
	return pos;
}

void Planet::updateVel(std::vector<Celestial *> all, double dt) {
	for (unsigned int i = 0; i < all.size(); i++) {
		if (all.at(i) == this) continue;
		Celestial * other = all.at(i);
		vector dir = other->getPos() - pos;
		float accel = G * other->getMass() / dir.slen();
		vel += dir.normalize() * accel * dt;
	}
}
void Planet::updatePos(std::vector<Celestial *> all, double dt) {
	(void)all;
	pos += vel * dt;
}

void Planet::draw(Shader &s) {
	s.use();
	s.setVector("model.translate", pos);
	s.setQuat("model.rotate", identity::rotation);
	s.setVector("model.scale", identity::scale * surfaceRadius);
	mesh.draw();
}

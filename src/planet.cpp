#include <string>
#include <vector>
#include <iostream>

#include "engine/math.hpp"
#include "engine/shader.hpp"
#include "engine/mesh.hpp"
#include "planet.hpp"

Planet::Planet(std::string n, vector p, vector v, float sr, float sg, Mesh & m)
	: name(n), pos(p), vel(v), surfaceGravity(sg), surfaceRadius(sr), mesh(m) {
	mass = sg * sr * sr / G;
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

void Planet::updateVel(std::vector<Celestial *> all, unsigned int dt) {
	for (unsigned int i = 0; i < all.size(); i++) {
		if (all.at(i) == this) continue;
		Celestial * other = all.at(i);
		vector dir = other->getPos() - pos;
		float accel = G * other->getMass() / dir.slen();
		vel += dir.normalize() * accel * dt / 1000.0;
	}
}
void Planet::updatePos(std::vector<Celestial *> all, unsigned int dt) {
	(void)all;
	pos += vel * dt / 1000.0;
}

void Planet::draw(Shader &s) {
	s.use();
	s.setVector("model.translate", pos);
	s.setQuat("model.rotate", IDR);
	s.setVector("model.scale", IDS * surfaceRadius);
	mesh.draw();
}

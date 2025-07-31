#ifndef CELESTIAL_H
#define CELESTIAL_H

#include <string>
#include <vector>

#include "engine/math.hpp"

const float G = 1.0;
class Celestial {
  public:
	virtual std::string getName() = 0;
	virtual float getMass() = 0;
	virtual vector getPos() = 0;
	virtual void updateVel(std::vector<Celestial *>, double) = 0;
	virtual void updatePos(std::vector<Celestial *>, double) = 0;
	virtual void draw(Shader &) = 0;
};

#endif

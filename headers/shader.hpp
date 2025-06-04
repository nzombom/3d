#ifndef SHADER_H
#define SHADER_H

#include <string>

#include "math.hpp"

class Shader {
	unsigned int id;

	public:
	Shader(std::string, std::string);
	~Shader();

	void use();
	void setBool(std::string, bool);
	void setInt(std::string, int);
	void setFloat(std::string, float);
	void setVector(std::string, vector);
	void setMatrix(std::string, matrix);
};

#endif

#ifndef SHADER_H
#define SHADER_H

#include <cstring>
#include <string>

#include "math.hpp"

struct Camera {
	vector p;
	quat r;
	float near;
	float far;
	float fov;
	float aspect;

	matrix projectionMatrix() {
		float right = near * std::tan(fov / 2.0);
		float top = right / aspect;
		return { near / right, 0, 0, 0,
				0, near / top, 0, 0,
				0, 0, -(far + near) / (far - near), -1,
				0, 0, -2 * far * near / (far - near), 0 };
	}
};

struct Light {
	vector p;
	vector c;
	float r;
};

class Shader {
	unsigned int id;
	std::string vPath, fPath;

  public:
	Shader(std::string, std::string);
	~Shader();

	void compile();

	void use();
	void setBool(std::string, bool);
	void setInt(std::string, int);
	void setFloat(std::string, float);
	void setVector(std::string, vector);
	void setQuat(std::string, quat);
	void setMatrix(std::string, matrix);
};

#endif

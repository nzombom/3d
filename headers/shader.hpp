#ifndef SHADER_H
#define SHADER_H

#include <string>
using string = std::string;

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

	public:
	Shader();
	Shader(string, string);
	~Shader();

	void use();
	void setBool(string, bool);
	void setInt(string, int);
	void setFloat(string, float);
	void setVector(string, vector);
	void setQuat(string, quat);
	void setMatrix(string, matrix);
	void applyCamera(Camera);
	void applyLight(Light);
};

#endif

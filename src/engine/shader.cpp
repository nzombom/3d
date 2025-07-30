#include <stdexcept>
#include <cstring>
#include <string>
using string = std::string;
#include <sstream>
#include <fstream>
#include <GL/glew.h>

#include "engine/shader.hpp"
#include "engine/math.hpp"

Shader::Shader(string _vPath, string _fPath)
	: vPath(_vPath), fPath(_fPath) {}

void Shader::compile() {
	std::ifstream vFile;
	std::ifstream fFile;
	vFile.open(vPath);
	if (!vFile.is_open()) {
		throw std::runtime_error("\e[31;1merror with shader file read:\e[m "
			+ vPath + ":\n" + std::strerror(errno));
	}
	fFile.open(fPath);
	if (!fFile.is_open()) {
		throw std::runtime_error("\e[31;1merror with shader file read:\e[m "
			+ vPath + ":\n" + std::strerror(errno));
	}

	std::stringstream vCodeStream;
	std::stringstream fCodeStream;
	vCodeStream << vFile.rdbuf();
	fCodeStream << fFile.rdbuf();
	string vCode = vCodeStream.str();
	string fCode = fCodeStream.str();
	const char * vcCode = vCode.c_str();
	const char * fcCode = fCode.c_str();

	vFile.close();
	fFile.close();

	unsigned int v;
	unsigned int f;
	int success;
	char infoLog[4096];


	v = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(v, 1, &vcCode, NULL);
	glCompileShader(v);
	glGetShaderiv(v, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(v, 4096, NULL, infoLog);
		throw std::runtime_error("\e[31;1merror with shader compilation:\e[m "
			+ vPath + ":\n" + infoLog);
	}

	f = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(f, 1, &fcCode, NULL);
	glCompileShader(f);
	glGetShaderiv(f, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(f, 4096, NULL, infoLog);
		throw std::runtime_error("\e[31;1merror with shader compilation:\e[m "
			+ fPath + ":\n" + infoLog);
	}

	id = glCreateProgram();
	glAttachShader(id, v);
	glAttachShader(id, f);
	glLinkProgram(id);
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(id, 4096, NULL, infoLog);
		throw std::runtime_error("\e[31;1merror with program linking:\e[m "
			+ vPath + ", " + fPath + ":\n" + infoLog);
	}

	glDeleteShader(v);
	glDeleteShader(f);
}
Shader::~Shader() {
	glDeleteProgram(id);
}

void Shader::use() {
	glUseProgram(id);
}
void Shader::setBool(string name, bool value) {
	glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}
void Shader::setInt(string name, int value) {
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}
void Shader::setFloat(string name, float value) {
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}
void Shader::setVector(string name, vector value) {
	glUniform3f(glGetUniformLocation(id, name.c_str()),
		value.x, value.y, value.z);
}
void Shader::setQuat(string name, quat value) {
	glUniform4f(glGetUniformLocation(id, name.c_str()),
		value.r.x, value.r.y, value.r.z, value.w);
}
void Shader::setMatrix(string name, matrix value) {
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()),
		1, false, value.v);
}

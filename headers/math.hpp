#ifndef MATH_H
#define MATH_H

#include <cmath>

struct vector {
	float x;
	float y;
	float z;

	vector operator-() {
		return { -x, -y, -z };
	}
	vector operator+(vector b) {
		return { x + b.x, y + b.y, z + b.z };
	}
	vector operator-(vector b) {
		return { x - b.x, y - b.y, z - b.z };
	}
	vector operator*(vector b) {
		return { x * b.x, y * b.y, z * b.z };
	}
	vector operator/(vector b) {
		return { x / b.x, y / b.y, z / b.z };
	}
	vector operator*(float s) {
		return { s * x, s * y, s * z };
	}
	vector operator/(float s) {
		return { s / x, s / y, s / z };
	}
	float operator|(vector b) {
		return x * b.x + y * b.y + z * b.z;
	}
	vector operator&(vector b) {
		return {
			y * b.z - z * b.y,
			z * b.x - x * b.z,
			x * b.y - y * b.x,
		};
	}
	float slen() {
		return x * x + y * y + z * z;
	}
	float len() {
		return std::sqrt(slen());
	}
	vector normalize() {
		return *this / len();
	}
};

struct vector2 {
	float x;
	float y;

	vector2 operator-() {
		return { -x, -y };
	}
	vector2 operator+(vector2 b) {
		return { x + b.x, y + b.y };
	}
	vector2 operator-(vector2 b) {
		return { x - b.x, y - b.y };
	}
	vector2 operator*(vector2 b) {
		return { x * b.x, y * b.y };
	}
	vector2 operator/(vector2 b) {
		return { x / b.x, y / b.y };
	}
	vector2 operator*(float s) {
		return { s * x, s * y };
	}
	vector2 operator/(float s) {
		return { s / x, s / y };
	}
	float operator|(vector2 b) {
		return x * b.x + y * b.y;
	}
	float slen() {
		return x * x + y * y;
	}
	float len() {
		return std::sqrt(slen());
	}
	vector2 normalize() {
		return *this / len();
	}
};

struct quat {
	float w;
	vector r;

	quat normalize() {
		float len = std::sqrt(w * w + r.slen());
		return { w / len, r / len };
	}
	quat conj() {
		return { w, -r };
	}
	quat operator*(quat b) {
		return { w * b.w - (r | b.r), b.r * w + r * b.w + (r & b.r) };
	}
	vector rotate(vector v) {
		return v + ((r + r) & ((r & v) + v * w));
	}
};

struct matrix {
	float v[16];
};

#endif

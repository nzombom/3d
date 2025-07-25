#ifndef MATH_H
#define MATH_H

#include <cmath>

struct vector {
	float x;
	float y;
	float z;

	vector operator-() const {
		return { -x, -y, -z };
	}
	vector operator+(vector b) const {
		return { x + b.x, y + b.y, z + b.z };
	}
	vector * operator+=(vector b) {
		x += b.x;
		y += b.y;
		z += b.z;
		return this;
	}
	vector operator-(vector b) const {
		return { x - b.x, y - b.y, z - b.z };
	}
	vector * operator-=(vector b) {
		x -= b.x;
		y -= b.y;
		z -= b.z;
		return this;
	}
	vector operator*(vector b) const {
		return { x * b.x, y * b.y, z * b.z };
	}
	vector operator/(vector b) const {
		return { x / b.x, y / b.y, z / b.z };
	}
	vector operator*(float s) const {
		return { x * s, y * s, z * s };
	}
	vector operator/(float s) const {
		return { x / s, y / s, z / s };
	}
	float operator|(vector b) const {
		return x * b.x + y * b.y + z * b.z;
	}
	vector operator&(vector b) const {
		return {
			y * b.z - z * b.y,
			z * b.x - x * b.z,
			x * b.y - y * b.x,
		};
	}
	float slen() const {
		return x * x + y * y + z * z;
	}
	float len() const {
		return std::sqrt(slen());
	}
	vector normalize() const {
		if (slen() > 0) return *this / len();
		return *this;
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
		if (slen() > 0) return *this / len();
		return *this;
	}
};

struct quat {
	float w;
	vector r;

	quat normalize() const {
		float len = std::sqrt(w * w + r.slen());
		return { w / len, r / len };
	}
	quat * renormalize() {
		float len = std::sqrt(w * w + r.slen());
		w = w / len;
		r = r / len;
		return this;
	}
	quat conj() const {
		return { w, -r };
	}
	quat operator*(quat b) const {
		return { w * b.w - (r | b.r), b.r * w + r * b.w + (r & b.r) };
	}
	quat * operator*=(quat b) {
		w = w * b.w - (r | b.r);
		r = { b.r * w + r * b.w + (r & b.r) };
		return this;
	}
	quat operator/(quat b) const {
		return *this * b.conj();
	}
	quat * operator/=(quat b) {
		quat q = *this / b;
		w = q.w;
		r = q.r;
		return this;
	}
	vector rotate(vector v) const {
		return v + ((r + r) & ((r & v) + v * w));
	}
};
inline quat qFromAA(vector axis, float angle) {
	return { std::cos(angle), axis * std::sin(angle) };
}

struct matrix {
	float v[16];
};

struct Transform {
	vector p;
	quat r;
	vector s;
};
inline const vector IDP = { 0, 0, 0 };
inline const quat IDR = { 1, { 0, 0, 0 } };
inline const vector IDS = { 1, 1, 1 };

#endif

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
	vector * operator+=(vector b) {
		x += b.x;
		y += b.y;
		z += b.z;
		return this;
	}
	vector * operator-=(vector b) {
		x -= b.x;
		y -= b.y;
		z -= b.z;
		return this;
	}
	vector * operator*=(vector b) {
		x *= b.x;
		y *= b.y;
		z *= b.z;
		return this;
	}
	vector * operator/=(vector b) {
		x /= b.x;
		y /= b.y;
		z /= b.z;
		return this;
	}
	vector * operator*=(float s) {
		x *= s;
		y *= s;
		z *= s;
		return this;
	}
	vector * operator/=(float s) {
		x /= s;
		y /= s;
		z /= s;
		return this;
	}
	float slen() const {
		return x * x + y * y + z * z;
	}
	float len() const {
		return std::sqrt(slen());
	}
	vector normalize() const {
		if (slen() > 0) {
			float l = len();
			return { x / l, y / l, z / l };
		}
		return *this;
	}
};
[[maybe_unused]] static const vector operator+(const vector a, const vector b) {
	return { a.x + b.x, a.y + b.y, a.z + b.z };
}
[[maybe_unused]] static const vector operator-(const vector a, const vector b) {
	return { a.x - b.x, a.y - b.y, a.z - b.z };
}
[[maybe_unused]] static const vector operator*(const vector a, const vector b) {
	return { a.x * b.x, a.y * b.y, a.z * b.z };
}
[[maybe_unused]] static const vector operator/(const vector a, const vector b) {
	return { a.x / b.x, a.y / b.y, a.z / b.z };
}
[[maybe_unused]] static const vector operator*(const vector a, const float s) {
	return { a.x * s, a.y * s, a.z * s };
}
[[maybe_unused]] static const vector operator/(const vector a, const float s) {
	return { a.x / s, a.y / s, a.z / s };
}
[[maybe_unused]] static const vector operator*(const float s, const vector a) {
	return { a.x * s, a.y * s, a.z * s };
}
[[maybe_unused]] static const vector operator/(const float s, const vector a) {
	return { a.x / s, a.y / s, a.z / s };
}
[[maybe_unused]] static float operator|(const vector a, const vector b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
[[maybe_unused]] static const vector operator&(const vector a, const vector b) {
	return {
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x,
	};
}

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
	quat * operator*=(quat b) {
		w = w * b.w - (r | b.r);
		r = { b.r * w + r * b.w + (r & b.r) };
		return this;
	}
	quat * operator/=(quat b) {
		quat d = b.conj();
		w = w * d.w - (r | d.r);
		r = d.r * w + r * d.w + (r & d.r);
		return this;
	}
	vector rotate(vector v) const {
		return v + ((r + r) & ((r & v) + v * w));
	}

	static inline quat fromAA(vector axis, float angle) {
		return { std::cos(angle), axis * std::sin(angle) };
	}
};
[[maybe_unused]] static quat operator*(const quat a, const quat b) {
	return { a.w * b.w - (a.r | b.r), b.r * a.w + a.r * b.w + (a.r & b.r) };
}
[[maybe_unused]] static quat operator/(const quat a, const quat b) {
	return a * b.conj();
}

struct matrix {
	float v[16];
};

struct Transform {
	vector p;
	quat r;
	vector s;
};

namespace identity {
static constexpr vector position{ 0, 0, 0 };
static constexpr quat rotation{ 1, { 0, 0, 0 } };
static constexpr vector scale{ 1, 1, 1 };
static constexpr Transform transform{ position, rotation, scale };
}

#endif

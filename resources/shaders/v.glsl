#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform vec3 mTranslate;
uniform vec4 mRotate;
uniform vec3 mScale;
uniform vec3 vTranslate;
uniform vec4 vRotate;
uniform mat4 projection;

out vec3 pos;
out vec3 normal;
out vec2 texCoord;
out vec3 cameraPos;

vec3 rotate(vec3 v, vec4 q) {
	float w = q.w;
	vec3 r = q.xyz;
	return v + cross(r + r, cross(r, v) + w * v);
}

void main() {
	mPos = rotate(aPos * mScale, mRotate) + mTranslate;
	pos = mPos;
	vPos = rotate(mPos + viTranslate, viRotate);
	gl_Position = projection * vPos;
}

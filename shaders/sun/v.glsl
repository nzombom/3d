#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform struct Model {
	vec3 translate;
	vec4 rotate;
	vec3 scale;
} model;
uniform struct View {
	vec3 translate;
	vec4 rotate;
	mat4 projection;
} view;

out vec3 pos;
out vec3 vNormal;
out vec3 cameraPos;

vec3 rotate(vec3 v, vec4 q) {
	float w = q.w;
	vec3 r = q.xyz;
	return v + cross(r + r, cross(r, v) + w * v);
}

void main() {
	vec3 mPos = rotate(aPos * model.scale, model.rotate) + model.translate;
	pos = mPos;
	vNormal = rotate(normalize(aNormal / model.scale), model.rotate);
	cameraPos = -view.translate;
	vec3 vPos = rotate(mPos + view.translate, view.rotate);
	gl_Position = view.projection * vec4(vPos, 1.0);
}

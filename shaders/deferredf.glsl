#version 460 core

in vec3 cameraPos;

layout (binding = 0) uniform sampler2D gPosition;
layout (binding = 1) uniform sampler2D gNormal;
layout (binding = 2) uniform sampler2D gColor;

uniform vec3 lightPos;
uniform vec3 lightStrength;
uniform float lightRadius;
uniform vec4 color;

out vec4 FragColor;

void main() {
	vec2 texCoords = gl_FragCoord.xy / vec2(960, 540);
	if (texture(gNormal, texCoords).w == 0) discard;
	vec3 pos = texture(gPosition, texCoords).rgb;
	vec3 normal = texture(gNormal, texCoords).rgb;
	vec3 color = texture(gColor, texCoords).rgb;
	float specular = texture(gColor, texCoords).a;
	float d = length(lightPos - pos);

	vec3 light = normalize(lightPos - pos);
	vec3 view = normalize(cameraPos - pos);
	vec3 halfway = normalize(light + view);
	float ln = dot(light, normal);
	float hn = dot(halfway, normal);

	float av, dv, sv;
	av = 0.25;
	if (ln > 0) {
		dv = ln;
		if (hn > 0) {
			sv = pow(hn, 64);
		}
	}

	float s = d / lightRadius;
	float attenuation;
	if (s < 0 || s > 1) attenuation = 0;
	else attenuation = pow(1 - s * s, 2) / (1 + 8 * s * s);
	vec3 lv = lightStrength * color
		* (av + dv + specular *  sv)
		* attenuation;
	FragColor = vec4(lv, 1.0);
}

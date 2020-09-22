// default.frag

#version 330 core

in vec2 TexCoord;
in vec3 LightDelta;
in vec3 SurfaceNormal;

out vec4 OutColor;

uniform sampler2D Texture;

void main() {
	float LightDistance = length(LightDelta);
	vec3 LightNormal = normalize(LightDelta);
	float Attenuation = 1.0 / (LightDistance * LightDistance);

	float DotProduct = dot(SurfaceNormal, LightNormal);
	vec3 Diffuse = max(DotProduct, 0.05) * vec3(texture2D(Texture, TexCoord));

	OutColor = vec4(Diffuse, 1);
}

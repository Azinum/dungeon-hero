// default.frag

#version 330 core

in vec2 TexCoord;
in vec3 SurfaceNormal;
in vec3 LightDelta;

out vec4 OutColor;

uniform sampler2D Texture;

void main() {
	vec3 UnitNormal = normalize(SurfaceNormal);
	vec3 UnitLight = normalize(LightDelta);

	float DotProduct = dot(UnitNormal, UnitLight);
	float Brightness = max(DotProduct, 0.1f);
	float BrightnessFactor = 1.5f;
	vec3 LightFactor = Brightness * vec3(1, 1, 1) * BrightnessFactor;
	OutColor = vec4(LightFactor, 1) * texture2D(Texture, TexCoord);
}

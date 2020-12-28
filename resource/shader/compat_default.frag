// compat_default.frag

#version 130

in vec2 TexCoord;
in vec3 SurfaceNormal;
in vec3 FragPos;

out vec4 OutColor;

uniform sampler2D Texture;
uniform vec3 Light;

vec3 LightColor = vec3(1, 1, 1);

void main() {
	float AmbientStrength = 0.1;
	vec3 Ambient = AmbientStrength * LightColor;

	vec3 Normal = normalize(SurfaceNormal);
	vec3 LightDirection = normalize(Light - FragPos);
	float Delta = max(dot(Normal, LightDirection), 0.0);
	vec3 Diffuse = Delta * LightColor;

	OutColor = vec4((Ambient + Diffuse), 1.0) * texture2D(Texture, TexCoord);
}

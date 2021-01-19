// default.frag

#version 330 compatibility

in vec2 TexCoord;
in vec3 SurfaceNormal;
in vec3 FragPos;

out vec4 OutColor;

uniform sampler2D Texture;
uniform vec3 Light;
uniform float LightStrength;

vec3 LightColor = vec3(1, 1, 1);

void main() {
	float AmbientStrength = 0.02;
	vec3 Ambient = AmbientStrength * LightColor;

	vec3 Normal = normalize(SurfaceNormal);
	vec3 LightDirection = Light - FragPos;

	float Brightness = dot(Normal, LightDirection) / (length(LightDirection) * length(Normal));
	Brightness = clamp(Brightness, 0, 1);

	vec3 Diffuse = Brightness * LightColor;

	OutColor = vec4((Diffuse * Ambient * LightStrength), 1.0) * texture(Texture, TexCoord);
}
